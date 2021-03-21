#include <Arduino.h>
#include <Ethernet.h>

#include <PN532_debug.h>
#include <PN532_HSU.h>
#include <PN532.h>
#include <syslog.h>
#include <SPI.h>
#include <SD.h>
#include <OneMsTaskTimer.h>

#include "broadcastannouncer.h"
#include "mqttannouncer.h"
#include "settings.h"
#include "microrl.h"
#include "cli.h"
#include "cacheoverlay.h"
#include "card.h"
#include "utils.h"
#include "tool.h"
#include "network.h"
#include "acnode.h"
#include "rgb.h"
#include "colour.h"
#include "every.h"
#include "version.h"
#include "watchdog.h"
#include "cache.h"
#include "card.h"
#include "sdcache.h"
#include "spiflash.h"
#include "littlefs.h"
#include "lfscache.h"
#include "eepromcache.h"
#include "ramcache.h"
#include "doorbot.h"
#include "doorbot_ac.h"
#include "maintainercachemaintainer.h"
#include "ACServer/RealACServer.h"
#include "CardReader/cardreader.h"
#include "CardReader/pn532cardreader.h"

// create microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

// PN532_HSU pnhsu(Serial6, PD_2);
PN532_HSU pnhsu(Serial6);
PN532 nfc(pnhsu);

CardReader* cardReader = nullptr;

EthernetClient client;
Syslog syslog;

boolean network = false;

// PG_1 to switch tool on, PE_4 is low when the tool is running
Tool tool(PG_1, PE_4);

RGB rgb(PM_0, PM_1, PM_2, nullptr);

Watchdog wdog;

Cache *cache = NULL;
MaintainerCacheMaintainer* mcmaintainer = nullptr;
ACServer* acserver = nullptr;

SPIFlash* spiFlash = nullptr;
LittleFS* littleFS = nullptr;

Door *door = NULL;
Doorbot *doorbot = NULL;
ACNode *acnode = NULL;
int active_role;
Announcer* announcer = NULL;
int alive_check_count = 0;

Every am_i_alive(60000);

#define BUTTON_PIN PF_1
#define DOOR_RELEASE_PIN PM_6
#define SD_CS_PIN PC_7
#define FLASH_CS_PIN PD_7
#define ACNODE_DIR "ACNODE"

void serviceLed() { rgb.run(); };
OneMsTaskTimer_t ledBlinker = { 300, serviceLed, 0, 0 };

void setup() {
  Serial.begin(9600);
  Serial.println("");
  print_buildinfo();
  Serial.println("Starting up.");
  // lets use all the LED's
  pinMode(D1_LED, OUTPUT);
  pinMode(D2_LED, OUTPUT);

  digitalWrite(D1_LED, HIGH); // heartbeat
  digitalWrite(D2_LED, LOW);  // lit when we detect a card

  // used for ethernet link and activity
  pinMode(D3_LED, OUTPUT);
  pinMode(D4_LED, OUTPUT);

  // start the watchdog early in case of hangs
  wdog.begin();

  if(SPIFlash::detect(FLASH_CS_PIN))
  {
    Serial.println("SPI Flash is available!");
    spiFlash = SPIFlash::get(FLASH_CS_PIN);
    littleFS = new LittleFS(spiFlash);
  }
  else
  {
    Serial.println("SPI Flash is not available");
  }

  // need settings to configure the tool
  init_settings();

  acsettings = get_settings();
  dump_settings(acsettings);

  // start the tool early so it can be switched off(!?)
  tool.begin();

  // start the rgb early so we can get some feedback
  rgb.begin();

  // Workaround for a missing declaration in the library
  uint32_t timer_index_ = 0;
  OneMsTaskTimer::add(&ledBlinker);
  rgb.rainbow();
  OneMsTaskTimer::start();

  wdog.feed();

  if (acsettings.sdcache) {
    if(littleFS && littleFS->isReady()) {
      Serial.println("Using LittleFS cache");
      cache = new LFSCache(littleFS);
    } else if (SD.begin(SD_CS_PIN, SPI_HALF_SPEED, 2)) {
      Serial.println("SD card is accessible");
      SD.mkdir(ACNODE_DIR);
      cache = new SDCache("CACHE");
      Serial.println("Using the SD Card to cache cards");
    } else {
      Serial.println("SD card could not be accessed");
      Serial.println("Please fix the SD card and try again. Caching cards in RAM for now.");
      cache = new RAMCache();

      if(acsettings.maintainer_cache) {
        // insert overlay
        Cache* persistent = new EEPromCache();
        cache = new CacheOverlay(cache, persistent);
        acserver = new RealACServer(client, acsettings.servername, acsettings.port, acsettings.nodeid);
        mcmaintainer = new MaintainerCacheMaintainer(persistent, acserver);
      }
    }
  } else {
      Serial.println("Using the eeprom to cache cards");
      cache = new EEPromCache();
  }

  cache->begin();

  microrl_init (prl, mrlprint);
  // set callback for execute
  microrl_set_execute_callback (prl, mrlexecute);

  // set callback for completion
  microrl_set_complete_callback (prl, mrlcomplete);

  wdog.feed();

  // start the Ethernet connection. If the network is down this takes some time (60 secs?)
  // so disable the watchdog while it's happening
  wdog.disable();
  if (!Ethernet.begin(acsettings.mac)) {  
    Serial.println("Failed to configure Ethernet using DHCP");
  } else {
    network = true;
    Ethernet.enableLinkLed();
    Ethernet.enableActivityLed();
  }
  wdog.feed();
  wdog.enable();

  if (Ethernet.localIP() == INADDR_NONE) {
    Serial.println("Didn't get a valid ip");
    network = false;
  }

  if (network) {
    // print your local IP address:
    Serial.print("My IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      Serial.print(Ethernet.localIP()[thisByte], DEC);
      Serial.print(".");
    }
    Serial.println();
  }

  wdog.feed();

  if (!network) {
    syslog.offline();
  }

  syslog.begin(acsettings.syslogserver, acsettings.toolname, LOG_LOCAL0);
  char tmp[42];
  snprintf(tmp, 42, "Starting up, version %s", ACVERSION);
  syslog.syslog(LOG_NOTICE, tmp);

  if (wdog.was_reset()) {
    syslog.syslog(LOG_ALERT, "Alert! Was previously reset by the watchdog!");
  }
  wdog.feed();
  Serial.println("Determining announce_mode");
  switch(acsettings.announce_mode) {
    case 1:
      Serial.println("Broadcast announcer");
      if (network) {
        announcer = new BroadcastAnnouncer(acsettings.announce_port);
      } else {
        Serial.println("Would be Broadcast but no network");
        announcer = nullptr;
      }
      break;
    case 2:
      if(network) {
        Serial.println("MQTT announcer");
        announcer = new MQTTAnnouncer(acsettings.mqtt_server, acsettings.mqtt_port, acsettings.mqtt_topic_base);
      } else {
        Serial.println("Not creating MQTT announcer because there is no network connection");
        announcer = nullptr;
      }
      break;
    default:
      Serial.println("No announcer");
      announcer = nullptr;
  }

  if(announcer) {
    Serial.println("Announcer starting");
    announcer->START();
  }

  door = new Door(PG_1, 0, acsettings.door_keep_open_ms);

  Serial.println("Initialising Card reader...");


  cardReader = new PN532CardReader(nfc);
  cardReader->init();

  wdog.feed();

  // The role may change when the user changes it via settings.
  active_role = acsettings.role;
  switch (active_role) {
    case 1:
      Serial.println("Starting regular Doorbot role");
      doorbot = new Doorbot(*door, wdog, cardReader, rgb, BUTTON_PIN, DOOR_RELEASE_PIN);
      doorbot->enableAnnouncer(announcer);
      break;
    case 2:
      Serial.println("Starting restricted Doorbot role");
      doorbot = new DoorbotWithAccessControl(*door, wdog, cardReader, rgb, BUTTON_PIN, DOOR_RELEASE_PIN);
      doorbot->enableAnnouncer(announcer);
      break;
    default:
      Serial.println("Starting acnode role");
      acnode = new ACNode(cardReader, rgb, tool, BUTTON_PIN);
      acnode->enableAnnouncer(announcer);
      break;
  }

  wdog.feed();

  if (network) {
    Serial.println("Checking tool status");
    int status = networking::networkCheckToolStatus();

    if (status < 0) {
      network = false;
      Serial.println("acserver error");
      char tmp[42];
      snprintf(tmp, 42, "unable to contact acserver");
      syslog.syslog(LOG_ALERT, tmp);
    }

    if (status == 1 || status == 0) {
      if (acsettings.status != status) {
        // update them if they have changed.
        acsettings.status = status;
        set_settings(acsettings);
      }
    }

    wdog.feed();

    char tmp[42];
    snprintf(tmp, 42, "tool status: %s", acsettings.status ? "in service" : "out of service");
    syslog.syslog(LOG_INFO, tmp);
    snprintf(tmp, 42, "total runtime: ");
    duration_str(tmp + strlen(tmp), acsettings.runtime);
    syslog.syslog(LOG_INFO, tmp);
    rgb.solid(BLUE);
  } else {
    rgb.solid(ORANGE);
  }

  wdog.feed();

  Serial.println("press enter for a prompt");
}

void loop() {
  // put received char from stdin to microrl lib
  if (Serial.available()) {
    char c;
    c = Serial.read();
    microrl_insert_char (prl, c);
  }
  switch (active_role) {
    case 1:
    case 2:
      doorbot->run();
      break;
    default:
      acnode->run();
  }
  
  if(announcer)
  {
    announcer->run();
  }

  if(mcmaintainer && network)
  {
    mcmaintainer->run();
  }

  if (am_i_alive.check()) {
    if(announcer) {
      announcer->ALIVE();
    }
    // Some of the cache methods don't store time-to-live, so we will want to verify all cached entries every 30 times we do the alive announcement, which works out about every half an hour
    // For those that do store time-to-live, this has the useful side-effect of refreshing the TTL, which means a bit more useful runtime if networking to the acserver is lost.
    if (network) {
      if (alive_check_count > 29) {
        alive_check_count = 0;
        wdog.feed();
        rgb.flashing(BLUE);
        Serial.println("Cache refreshing");
        cache->verify();
        rgb.solid(BLUE);
      }
      alive_check_count++;
      if ((door->isOpen()) || (tool.status())) {
        alive_check_count=0; // Because verifying all the users takes a while, we want to do it at quiet times
      }
    }
  }
}

extern "C"
{

// littlefs uses abort, which calls _exit - but no matter what options I try,
// I can't get the toolchain to deal with that properly, so here's an _exit
// implementation to keep it happy
int _exit(int ret)
{
  while(1);
}

}
