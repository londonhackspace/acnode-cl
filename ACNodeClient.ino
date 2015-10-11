/*
ACNodeClient
 v0.2+
 
 */
#include <Ethernet.h>

#define DEBUG 1
#include <PN532_debug.h>
#include <PN532_HSU.h>
#include <PN532.h>
#include <syslog.h>
#include <SPI.h>
#include <SD.h>
#include <DateTimeLibrary.h>

#include "settings.h"
#include "microrl.h"
#include "cli.h"
#include "card.h"
#include "utils.h"
#include "tool.h"
#include "network.h"
#include "acnode.h"
#include "rgb.h"
#include "button.h"
#include "every.h"
#include "version.h"
#include "watchdog.h"
#include "cache.h"
#include "card.h"
#include "sdcache.h"
#include "eepromcache.h"

// create microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

// PN532_HSU pnhsu(Serial6, PD_2);
PN532_HSU pnhsu(Serial6);
PN532 nfc(pnhsu);

settings acsettings;
Card cc; // current card on the reader

// if we have a maintainer and they are adding a card keep track of who is doing the adding
Card maintainer;

EthernetClient client;
Syslog syslog;
DateTime rtc;

boolean network = false;

// PG_1 to switch tool on, PE_4 is low when the tool is running
Tool tool(PG_1, PE_4);

RGB rgb(PM_0, PM_1, PM_2);

Every one_sec(1000);
Every five_sec(5000);

Every heart_every(1000);
boolean heartbeat = true;

Every card_every(500);

Every tool_status_check(2000);

Button button(PF_1);

Watchdog wdog;

Cache *cache = NULL;

#define SD_CS_PIN PC_7
#define ACNODE_DIR "ACNODE"

void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.print("\n\nACNode Client version ");
  Serial.print(ACVERSION);
  Serial.print(" built with Energia ");
  Serial.print(ENERGIA);
  Serial.print(", Arduino API version ");
  Serial.println(ARDUINO);
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
  rgb.yellow();

  wdog.feed();

  if (acsettings.sdcache) {
    if (SD.begin(SD_CS_PIN, SPI_HALF_SPEED, 2)) {
      Serial.println("SD card is accessible");
      SD.mkdir(ACNODE_DIR);
      cache = new SDCache("CACHE");
      Serial.println("Using the SD Card to cache cards");
    } else {
      Serial.println("SD card could not be accessed");
      Serial.println("Please fix the SD card and try again.");
      rgb.red();
      while (1) {;}
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

    // start the clock and set the time.
    rtc.begin();
    time_t now;
    // just use the ACServer for ntp
    getTimeNTP(now, acsettings.servername);
    rtc.setTime(now);
    Serial.println(stringDateTime(rtc.getLocalTime()));
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

  button.begin();
  one_sec.begin();
  five_sec.begin();
  card_every.begin();
  heart_every.begin();
  tool_status_check.begin();

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
    int status = networkCheckToolStatus();

    if (status == -1) {
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
    rgb.blue();
  } else {
    rgb.orange();
  }

  wdog.feed();

  if (network) {
    // verify the users in the cache against the acserver
    cache->verify();
  }

  wdog.feed();

  cc.set_valid(false);
  maintainer.set_valid(false);

  Serial.println("press enter for a prompt");
}

// current user
Card cu;
boolean card_on_reader = false;

// true if we are adding a card
boolean adding = false;
unsigned long adding_timeout = 0;

// are we in a menu?
enum MenuType {NOMENU, USER, MAINTAINER};
enum MenuState {ADD, OFFLINE, ONLINE};

MenuType menu = NOMENU;
MenuState menu_state;

void loop() {
  Card tu;
  boolean check = true;
  
  // some housekeeping stuff
  if (heart_every.check()) {
    wdog.feed();

    if (heartbeat) {
      digitalWrite(D1_LED, HIGH);
      heartbeat = false;

      if (menu == NOMENU && !network && !card_on_reader) {
        rgb.orange();
      }

      if (menu != NOMENU) {
        Serial.println("Menu colour stuff");
        switch (menu_state) {
          case ADD:
            // Amber for Adding
            rgb.orange();
            break;
          case OFFLINE:
            rgb.red();
            break;
          case ONLINE:
            rgb.green();
            break;
        }
      } else {
        if (!card_on_reader) {
          if (!acsettings.status) {
            // offline
            rgb.red();
          }
        }
      }
    } else {
      digitalWrite(D1_LED, LOW);
      heartbeat = true;
      if (!card_on_reader) {
        if (acsettings.status) {
          // green if the tool is running, otherwise blue
          if (tool.status()) {
            rgb.green();
          } else {
            rgb.blue();
          }
        } else {
          // offline
          rgb.red();
        }
      } else {
        // we have a user
        if (cu.is_maintainer()) {
          // maintainer
          rgb.yellow();
        } else {
          if (cu.is_user()) {
            if (acsettings.status) {
              if (tool.status()) {
                // green if the tool is running, otherwise blue
                rgb.green();
              } else {
                rgb.blue();
              }
            } else {
              // offline
              rgb.red();
            }
          } else {
            rgb.orange();
          }
        }
      }
    }
  }

  // put received char from stdin to microrl lib
  if (Serial.available()) {
    char c;
    c = Serial.read();
    microrl_insert_char (prl, c);
  }

  // used to check the tool on state via an ISR.
  tool.poll();

  /*
   *
   * State here:
   *
   * cc = the current card, updated from the card reader
   * tu = temp user, used for working things out as we go alone
   * nu = new user, in the process from going from cc -> cu
   * cu = current user, i.e. the current card has been checked against the db, and the users permissions etc obtained
   * maintainer = if we have a maintainer store there details here - we need it when adding a new card.
   *
   */

  // were we adding a card?
  // if so did one actually get added?
  if (adding) {
    // 30 secs to add a card
    if (adding_timeout < millis()) {
      adding = false;
    }
  }

  if (card_every.check()) {
    // does the actuall card reading, updates cc.
    cc = readcard();

    /*
     * we have to:
     *
     * use the uid on the card to fill in the other info about the user
     * if the card is in the cache start with the data there
     * if we can get to the acserver update the data from there,
     * and save it to the cache if needed
     */
    if (card_on_reader) // we have a valid card?
    {
      int status;
      digitalWrite(D2_LED, HIGH); // indicate that we have a card.
      Card nu = cc;

//      Serial.print("got a card : ");
//      dump_user(nu);

      // is the new card different from the maintainer card?
      if (maintainer.is_valid() && !(nu == maintainer)) {
        // are we adding a new user?
        if (adding) {
          char tmp[128];

          adding = false;

          sprintf(tmp, "%s", "Maintainer ");
          maintainer.str(tmp + strlen(tmp));
          sprintf(tmp + strlen(tmp), " adding a card ");
          nu.str(tmp + strlen(tmp));
          Serial.println(tmp);
          syslog.syslog(LOG_NOTICE, tmp);

          addNewUser(nu, maintainer);
          adding = false;
        }
      }

      // we have a possibly new card, check it against the cache.
      tu = cache->get(nu);

      // was it in the cache?
      if (tu.is_valid() && cu.is_valid()) {
        // same as current card?
        if (cu == tu) {
          // card not changed
          // no need to check against the server.
          check = false;
        } else {
          Serial.println("card changed, tu: ");
          tu.dump();
          Serial.println("cu: ");
          cu.dump();
        }
      } else if (cu.is_valid() && !tu.is_valid()) {
          // not in the cache.
  //        TRACE
  //        dump_user(cu);
  //        dump_user(nu);
        if (cu.compare_uid(nu)) {
          // no need to check against the server, we've already got the card
          check = false;
        }
      }

      if (check) {
        status = querycard(cc);

        // if we've lost contact with the acserver at some point
        // we need to check and re-establish it, might as well use this as the test.
        if (status >= 0) {
          network = true;
        } else {
          network = false;
        }

        if (status >= 0) {
          nu.set_user(true);

          // maintainer?
          if (status == 2) {
            nu.set_maintainer(true);
          }

          // user exists but can't do anything
          // so mark as invalid
          if (status == 0) {
            nu.set_valid(false);
          }

          tu = cache->get(nu);

          if (tu.is_valid()) {
            if (tu != nu) {
              Serial.println("user changed, updating cache");
              cache->set(nu);
            }
          } else {
            // new user, so just store it.
            if (nu.is_user()) {
              cache->set(nu);
            }
          }
        } else {
          // network error
          Serial.println("network error, trying to find cached card");

          tu = cache->get(nu);

          if (tu.is_valid()) {
            nu = tu;
            Serial.print("Found cached user: ");
            tu.dump();
          }
        }
      } else if (check) {
        tu = cache->get(nu);

        if (tu.is_valid()) {
          nu = tu;
          Serial.print("Found cached user (check): ");
          tu.dump();
        }
      } else {
        // no network or no need to check, cached users only.
        if (cu.is_valid() && !nu.compare_uid(cu)) {

          Serial.println("trying to find cached card");

          tu = cache->get(nu);

          if (tu.is_valid()) {
            nu = tu;
          }
        }
      }

      // we've checked the card, so update cu.
      if (cu.is_valid() && check == true) {
        Serial.println("deleting old cu:");
        cu.dump();
        cu.set_valid(false);
      }

      // we had no current user, so set one up
      if (!cu.is_valid()) {
        cu = nu;
        Serial.println("new cu:");
        cu.dump();

        if (cu.is_valid()) {
          if (cu.is_user() && !cu.is_maintainer()) {
            rgb.green();
          } else if (cu.is_maintainer()) {
            rgb.yellow();
          }
        } else {
          rgb.orange();
        }

        if (cu.is_user()) {
          // only report tool usage if the network is ok.
          if (network) {
            // tell the server this user has started using the tool.
            reportToolUse(cu, 1);
          }
        }

      }

      nu.set_valid(false);
    } else {
      // no card on the reader
      if (cu.is_valid()) {
        Serial.println("Card removed");
        tool.off();

        if (cu.is_user()) {
          // only report tool usage if the network is ok.
          if (network) {
            // tell the server this user has stopped using the tool.
            reportToolUse(cu, 0);
          }
        }

        cu.set_valid(false);
        // nuke the maintainer just in case.
        if (!adding) {
          maintainer.set_valid(false);
        }
      }
      digitalWrite(D2_LED, LOW);
      if (acsettings.status) {
        if (tool.status()) {
          // green if the tool is running, otherwise blue
          rgb.green();
        } else {
          rgb.blue();
        }
      }
    }
  }

  // do we have a card? if so, do things with it.
  if (cu.is_valid()) {
    card_loop();
    menu_loop();
  }
}

// When we have a card on the reader go round this loop.
void card_loop() {
  
  if (one_sec.check()) {
    Serial.print("card on reader: ");
    cu.dump();
  }

  // tool enable etc here.
  if (cu.is_user()) {
    // is the tool running
    if (acsettings.status || cu.is_maintainer()) {
      // this card is authorised to switch the tool on, so switch it on.
      // check tool status against the acserver incase someone has set the tool out of service
      int status = acsettings.status;

      if (network) {
        // only check every 2 seconds
        if (tool_status_check.check()) {
          status = networkCheckToolStatus();
        }
      }

      if (status == 1 || cu.is_maintainer()) {
        tool.on(cu);
      } else if (status == 0) {
        // tool out of service, so don't switch it on!
        acsettings.status = 0;
        set_settings(acsettings);
      } else {
        Serial.println("acserver error");
        char tmp[42];
        snprintf(tmp, 42, "unable to contact acserver");
        syslog.syslog(LOG_ALERT, tmp);
      }
    }
  } else {
    // not a valid user.
    rgb.orange();
  }
}


unsigned long menu_timeout;

void menu_loop(void) {
  int button_state = NO_PRESS;

  button_state = button.poll();

/*  if (button_state != NO_PRESS) {
    Serial.print("Button: ");
    Serial.println(button_state);
  }*/

  if (button_state == SHORT_PRESS) {
    Serial.println("Short button down");
  } else if (button_state == LONG_PRESS) {
    Serial.println("Long button press");
  }

  if (button_state == NO_PRESS && menu == NOMENU) {
    // nothing to do
    return;
  }
  
  if (menu == NOMENU) {
    if (button_state == LONG_PRESS) {
      // long press outside of a menu does nothing
      return;
    }
    
    if (button_state == SHORT_PRESS) {
      // menu now activated
      menu_timeout = millis() + (60 * 1000);

      if (cu.is_user() && !cu.is_maintainer()) {
        menu = USER;
        // if we are a user the 1st item on the menu is OFFLINE
        menu_state = OFFLINE;
        // but if we are already offline don't do anything, normal users can't put us offline
        if (acsettings.status == 0) {
          char tmp[128];

          sprintf(tmp, "%s", "User ");
          cu.str(tmp + strlen(tmp));
          sprintf(tmp + strlen(tmp), " tried to use the menu when the tool is offline");
          Serial.println(tmp);
          syslog.syslog(LOG_NOTICE, tmp);

          menu = NOMENU;
        }
      }
      if (cu.is_maintainer()) {
        menu = MAINTAINER;
        // if we are on the maintainer menu, the 1st item is adding a user
        menu_state = ADD;
      }
    }
  } else {

    // menu currently active
    switch (menu) {
      case MAINTAINER:
        maintainer_menu(button_state);
        break;
      case USER:
        user_menu(button_state);
        break;
      case NOMENU:
        // we can't get here, but the compiler complains that we don't check this state, so put this in.
        break;
    }

    if (menu_timeout < millis()) {
      menu = NOMENU;
    }
  }
}


// these should probably be in tool.cpp
void offline(void) {
  acsettings.status = 0;
  set_settings(acsettings);

  char tmp[128];

  sprintf(tmp, "%s", "User ");
  cu.str(tmp + strlen(tmp));
  sprintf(tmp + strlen(tmp), " took the tool out of service.");
  Serial.println(tmp);
  syslog.syslog(LOG_WARNING, tmp);

  int ret = setToolStatus(0, cu);
  Serial.print("After setting ToolStatus: ");
  Serial.println(ret);

  // and switch the tool off.
  tool.off();
}

void online(void) {
  acsettings.status = 1;
  set_settings(acsettings);

  char tmp[128];

  sprintf(tmp, "%s", "User ");
  cu.str(tmp + strlen(tmp));
  sprintf(tmp + strlen(tmp), " put the tool in service.");
  Serial.println(tmp);
  syslog.syslog(LOG_WARNING, tmp);

  int ret = setToolStatus(1, cu);
  Serial.print("After setting ToolStatus: ");
  Serial.println(ret);

  // this is redundant since users can't put a tool back in service, and the tool will be on for a maintainer.
  // but leave it for completeness
  tool.on(cu);
}

void maintainer_menu(int press) {
  // short presses change menu item
  if (press == SHORT_PRESS) {
    switch (menu_state) {
      case ADD:
        // are we in service?
        if (acsettings.status) {
          // if so offer to take us out of service
          menu_state = OFFLINE;
        } else {
          menu_state = ONLINE;
        }
        break;
      case ONLINE:
      case OFFLINE:
        // exit the menu
        menu = NOMENU;
        break;
    }
  }
  // has a menu item bee activated?
  if (press == LONG_PRESS) {
    switch (menu_state) {
      case ADD:
        Serial.println("maintainer menu add selected");
        // the button is pressed, copy our current card details to the maintainer block
        maintainer = cu;
        // we are adding
        adding = true;
        // 30 sec adding timeout
        adding_timeout = millis() + (1000 * 30);
        Serial.println("*Adding*");

        menu = NOMENU;

        break;
      case OFFLINE:
        Serial.println("maintainer menu out of service selected");
        offline();
        menu = NOMENU;
        break;
      case ONLINE:
        Serial.println("maintainer menu in service selected");
        online();
        menu = NOMENU;
        break;
    }
  }
}

void user_menu(int press) {
  // exit the menu
  if (press == SHORT_PRESS) {
    Serial.println("Exiting the user menu");
    menu = NOMENU;
    return;
  }

  if (press == LONG_PRESS) {
    // we can only go offline here
    offline();
    menu = NOMENU;
  }
}

Card readcard()
{
  String mynum = "";

//  pnhsu.intr_check();

  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  nfc.powerDown();
  
  if (!success)
  {
    card_on_reader = false;
    return Card();
  }

/*
  Serial.print("Got card with uid: ");
  dumpHex(uid, uidLength);
  Serial.println("");
*/

  switch (uidLength) {
    case 4:
      card_on_reader = true;
      return Card(uid, 0, 0, 0);
      break;
    case 7:
      card_on_reader = true;
      return Card(uid, 1, 0, 0);
      break;
    default:
      Serial.print("Odd card length?: ");
      card_on_reader = false;
      Serial.println(uidLength);
      break;
  }
  card_on_reader = false;
  return Card();
}

