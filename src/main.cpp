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
#include "settings.h"
#include "microrl.h"
#include "cli.h"
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
#include "eepromcache.h"
#include "ramcache.h"
#include "doorbot.h"
#include "doorbot_ac.h"

// create microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

// PN532_HSU pnhsu(Serial6, PD_2);
PN532_HSU pnhsu(Serial6);
PN532 nfc(pnhsu);

EthernetClient client;
Syslog syslog;

boolean network = false;

// PG_1 to switch tool on, PE_4 is low when the tool is running
Tool tool(PG_1, PE_4);

RGB rgb(PM_0, PM_1, PM_2);

Watchdog wdog;

Cache *cache = NULL;

Door *door = NULL;
Doorbot *doorbot = NULL;
ACNode *acnode = NULL;
int active_role;

#define BUTTON_PIN PM_7
#define SD_CS_PIN PC_7
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
    if (SD.begin(SD_CS_PIN, SPI_HALF_SPEED, 2)) {
      Serial.println("SD card is accessible");
      SD.mkdir(ACNODE_DIR);
      cache = new SDCache("CACHE");
      Serial.println("Using the SD Card to cache cards");
    } else {
      Serial.println("SD card could not be accessed");
      Serial.println("Please fix the SD card and try again. Caching cards in RAM for now.");
      cache = new RAMCache();
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

  Announcer* announcer;

  switch(acsettings.announce_mode) {
    case 1:
      announcer= new BroadcastAnnouncer(acsettings.announce_port);
      break;
    case 2:
      Serial.println("Warning: MQTT Announcer not currently supported! Disabling Announcements...");
    default:
      announcer = nullptr;
  }

  if(announcer) {
    announcer->START();
  }

  door = new Door(PG_1, 0, acsettings.door_keep_open_ms);

  // The role may change when the user changes it via settings.
  active_role = acsettings.role;
  switch (active_role) {
    case 1:
      doorbot = new Doorbot(*door, wdog, nfc, rgb);
      doorbot->enableAnnouncer(announcer);
      break;
    case 2:
      doorbot = new DoorbotWithAccessControl(*door, wdog, nfc, rgb);
      doorbot->enableAnnouncer(announcer);
      break;
    default:
      acnode = new ACNode(nfc, rgb, tool, BUTTON_PIN);
      break;
  }

  wdog.feed();

  Serial.println("Initialising Card reader...");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();

  if (! versiondata) {
    Serial.println("Didn't find PN53x board");
    syslog.syslog(LOG_EMERG, "Couldn't find PN532 board");
    while (1); // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.print((versiondata>>24) & 0xFF, HEX);
  Serial.print(", Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  //
  // with the powerdown thing we don't need a long timeout
  // so use 16...
  nfc.setPassiveActivationRetries(0x10);

  // configure board to read RFID tags
  nfc.SAMConfig();

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

  // For now, just purge the cache. Checking a card against the server at startup
  // takes ~3s per card. TODO: Add code to check against the server when card is
  // scanned and TTL is deemed to be expired.
  cache->purge();

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
}
