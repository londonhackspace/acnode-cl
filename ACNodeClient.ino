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

#include "settings.h"
#include "microrl.h"
#include "cli.h"
#include "user.h"
#include "utils.h"
#include "tool.h"
#include "network.h"
#include "acnode.h"
#include "rgb.h"
#include "button.h"
#include "every.h"
#include "version.h"
#include "door.h"
#include "announcer.h"

// create microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

// PN532_HSU pnhsu(Serial6, PD_2);
PN532_HSU pnhsu(Serial6);
PN532 nfc(pnhsu);

settings acsettings;
user cc; // current card on the reader

// if we have a maintainer and they are adding a card keep track of who is doing the adding
user maintainer;

EthernetClient client;
Syslog syslog;

boolean network = false;

// PG_1 to switch tool on, PE_4 is low when the tool is running
Tool tool(PG_1, PE_4);
Door door(PB_4);
Announcer *announcer;

RGB rgb(PM_0, PM_1, PM_2);

Every one_sec(1000);
Every five_sec(5000);

Every heart_every(1000);
boolean heartbeat = true;

Every card_every(500);

Every tool_status_check(2000);

Button button(PF_1);
Button bell_button(PM_3);


void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.print("\n\nACNode Client version ");
  Serial.print(ACVERSION);
  Serial.println(" Startup.");
  // lets use all the LED's
  pinMode(D1_LED, OUTPUT);
  pinMode(D2_LED, OUTPUT);

  digitalWrite(D1_LED, HIGH); // heartbeat
  digitalWrite(D2_LED, LOW);  // lit when we detect a card

  // used for ethernet link and activity
  pinMode(D3_LED, OUTPUT);
  pinMode(D4_LED, OUTPUT);

  // start the tool early so it can be switched off(!?)
  tool.begin();

  // start the rgb early so we can get some feedback
  rgb.begin();
  rgb.yellow();

  init_settings();

  acsettings = get_settings();
  dump_settings(acsettings);

  microrl_init (prl, mrlprint);
  // set callback for execute
  microrl_set_execute_callback (prl, mrlexecute);

  // set callback for completion
  microrl_set_complete_callback (prl, mrlcomplete);

  memset(&cc, 0, sizeof(user));
  memset(&maintainer, 0, sizeof(user));

  // start the Ethernet connection:
  if (!Ethernet.begin(acsettings.mac)) {
    Serial.println("Failed to configure Ethernet using DHCP");
  } else {
    network = true;
    Ethernet.enableLinkLed();
    Ethernet.enableActivityLed();
  }

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

  if (!network) {
    syslog.offline();
  }

  syslog.begin(acsettings.syslogserver, acsettings.toolname, LOG_LOCAL0);
  char tmp[42];
  snprintf(tmp, 42, "Starting up, version %s", ACVERSION);
  syslog.syslog(LOG_NOTICE, tmp);

  button.begin();
  bell_button.begin();
  one_sec.begin();
  five_sec.begin();
  card_every.begin();
  heart_every.begin();
  tool_status_check.begin();

  Serial.println("Initialising PN532");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();

  if (! versiondata) {
    Serial.println("Didn't find PN53x board");
    syslog.syslog(LOG_EMERG, "Couldn't find PN532 board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
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

  if (network) {
    Serial.println("Checking tool status");
    int status = networkCheckToolStatus();
    Serial.println(status);

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

  cc.invalid = 1;

  announcer = new Announcer(acsettings.announcer_port);
  announcer->START();

  Serial.println("press enter for a prompt");
}

// current user
user *cu = NULL;

// true if we are adding a card
boolean adding = false;
unsigned long adding_timeout = 0;

// are we in a menu?
enum MenuType {NOMENU, USER, MAINTAINER};
enum MenuState {ADD, OFFLINE, ONLINE};

MenuType menu = NOMENU;
MenuState menu_state;

void loop() {
  // put received char from stdin to microrl lib
  if (Serial.available()) {
    char c;
    c = Serial.read();
    microrl_insert_char (prl, c);
  }

  if (doorbot()) {
    doorbot_loop();
  } else {
    acnode_loop();
  }
}

void doorbot_loop() { 
  if(bell_button.poll() > 0) {
    Serial.println("DING DONG. Someone rang the bell!");
    announcer->BELL();
  }

  if (one_sec.check()) {
    if (door.maybe_close()) {
      rgb.blue();
    }
  }
  
  if (!door.opened()) {
    readcard();
    user read_user;

    if (!cc.invalid) { // we have a card of some sort
      int result = querycard(cc);
      switch (result) {
        case 2:
        case 1:
        case 0:
          // we know about these cards, let the user in
          doorbot_maybe_cache_card(result, &read_user);
          doorbot_open_door();
          break;
        case -1:
          // unknown card, go away
          doorbot_maybe_cache_card(result, &read_user);
          rgb.orange();
          delay(1000);
          break;
        default:
          // network error, have a look at the cache
          user *found_user = doorbot_cache_get();
          if (found_user && doorbot_has_access(found_user)) doorbot_open_door();
      }
      // regardless of what happened, tell the world that we've scanned a card.
      announcer->RFID(&read_user);
    }
  }
}

void doorbot_maybe_cache_card(int hint, user *read_user) {
  memset(read_user, 0, sizeof(user));
  memcpy(read_user, &cc, sizeof(user));

  user *found_user = get_user(read_user);

  switch (hint) {
    case 2:
      read_user->maintainer = 1; // set and fall through (!)
    case 1:
    case 0:
      read_user->status = 1;
      break;
    case -1:
      read_user->invalid = 1;
  }
  
  if (found_user) {
    // user is in the cache
    if (!compare_user(found_user, read_user)) {
      // user out of sync with cache
      store_user(read_user);
    }
  } else {
    // user not in the cache
    if (hint > -1) {
      // store if we know of this card
      store_user(read_user);
    }
  }
}

user* doorbot_cache_get() {
  user read_user;
  memset(&read_user, 0, sizeof(user));
  memcpy(&read_user, &cc, sizeof(user));
  
  return get_user(&read_user);
}

boolean doorbot_has_access(user *u) {
  return u->status;
}

void doorbot_open_door() {
  rgb.green();
  door.open();
}

void acnode_loop() {
  // some housekeeping stuff
  if (heart_every.check()) {
    if (heartbeat) {
      digitalWrite(D1_LED, HIGH);
      heartbeat = false;

      if (menu == NOMENU && !network && cu == NULL) {
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
        if (cu == NULL) {
          if (!acsettings.status) {
            // offline
            rgb.red();
          }
        }
      }
    } else {
      digitalWrite(D1_LED, LOW);
      heartbeat = true;
      if (cu == NULL) {
        if (acsettings.status) {
          rgb.blue();
        } else {
          // offline
          rgb.red();
        }
      } else {
        // we have a user
        if (cu->maintainer == 1) {
          // maintainer
          rgb.yellow();
        } else {
          if (cu->status == 1) {
            if (acsettings.status) {
              rgb.green();
            } else {
              // offline
              rgb.red();
            }
          }
        }
      }
    }
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
   * cu = current user, i.e. the current card have been chaged against the db, and the users permissions etc obtained
   * maintainer = if we have a maintainer store there details here.
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
    cu = read_user();
  }

  // do we have a card? if so, do things with it.
  if (cu != NULL) {
    card_loop();
    menu_loop();
  }
}

user *read_user() {
    user *tu;
    boolean check = true;
  
    // does the actuall card reading, updates cc.
    readcard();

    /*
     * we have to:
     *
     * use the uid on the card to fill in the other info about the user
     * if the card is in the cache start with the data there
     * if we can get to the acserver update the data from there,
     * and save it to the cache if needed
     */
    if (!cc.invalid) // we have a valid card?
    {
      int status;
      digitalWrite(D2_LED, HIGH); // indicate that we have a card.
      user *nu;
      nu = new user;
      memset(nu, 0, sizeof(user));
      memcpy(nu, &cc, sizeof(user));

//      Serial.print("got a card : ");
//      dump_user(nu);

      // is the new card different from the maintainer card?
      if (!compare_uid(nu, &maintainer)) {
        // are we adding a new user?
        if (adding) {
          char tmp[128];

          adding = false;

          sprintf(tmp, "%s", "Maintainer ");
          uid_str(tmp + strlen(tmp), &maintainer);
          sprintf(tmp + strlen(tmp), " adding a card ");
          uid_str(tmp + strlen(tmp), nu);
          Serial.println(tmp);
          syslog.syslog(LOG_NOTICE, tmp);

          addNewUser(*nu, maintainer);
          adding = false;
          memset(&maintainer, 0, sizeof(user));
        }
      }

      // we have a possibly new card, check it against the cache.
      tu = get_user(nu);

      // was it in the cache?
      if (tu != NULL && cu != NULL) {
        // same as current card?
        if (compare_user(cu, tu)) {
          // card not changed
          // no need to check against the server.
          check = false;
        } else {
          Serial.println("card changed, tu: ");
          dump_user(tu);
          Serial.println("cu: ");
          dump_user(cu);
        }
        delete tu;
      } else if (cu != NULL && tu == NULL) {
          // not in the cache.
  //        TRACE
  //        dump_user(cu);
  //        dump_user(nu);
        if (compare_uid(cu, nu)) {
          // no need to check against the server, we've already got the card
          check = false;
        }
      }

      if (check) {
        status = querycard(cc);
        Serial.println(status);

        // if we've lost contact with the acserver at some point
        // we need to check and re-establish it, might as well use this as the test.
        if (status >= -1) {
          network = true;
        } else {
          network = false;
        }

        if (status >= 0) {
          nu->status = 1;
          nu->invalid = 0;
          nu->end = 0;
          nu->maintainer = 0;

          // maintainer?
          if (status == 2) {
            nu->maintainer = 1;
          }

          // user exists but can't do anything
          // so mark as invalid
          if (status == 0) {
            nu->status = 0;
            return NULL;
          }

          tu = get_user(nu);

          if (tu != NULL) {
            if (!compare_user(tu, nu)) {
              Serial.println("user changed, updating cache");
              if (status == 0) {
                nu->invalid = 1;
              }
              store_user(nu);
            }
            delete tu;
          } else {
            // new user, so just store it.
            if (nu-> status == 1) {
              store_user(nu);
            }
          }
        } else {
          // network error
          Serial.println("network error, trying to find cached card");

          tu = get_user(nu);

          if (tu != NULL) {
            delete nu;
            nu = tu;
            Serial.print("Found cached user: ");
            dump_user(tu);
          }
        }
      } else if (check) {
        tu = get_user(nu);

        if (tu != NULL) {
          delete nu;
          nu = tu;
          Serial.print("Found cached user (check): ");
          dump_user(tu);
        }
      } else {
        // no network or no need to check, cached users only.
        if (cu != NULL && !compare_uid(nu, cu)) {

          Serial.println("trying to find cached card");

          tu = get_user(nu);

          if (tu != NULL) {
            delete nu;
            nu = tu;
          }
        }
      }

      // we've checked the card, so update cu.
      if (cu != NULL && check == true) {
        Serial.println("deleting old cu:");
        dump_user(cu);
        delete cu;
        cu = NULL;
      }

      // we had no current user, so set one up
      if (cu == NULL) {
        cu = new user;
        memcpy(cu, nu, sizeof(user));
        Serial.println("new cu:");
        dump_user(cu);

        if (cu->maintainer || cu->status) {
          // only report tool usage if the network is ok.
          if (network) {
            // tell the server this user has started using the tool.
            reportToolUse(*cu, 1);
          }
        }

      }

      delete nu;
      nu = NULL;
    } else {
      // no card on the reader
      if (cu != NULL) {
        Serial.println("Card removed");
        tool.off(*cu);

        if (cu->maintainer || cu->status) {
          // only report tool usage if the network is ok.
          if (network) {
            // tell the server this user has stopped using the tool.
            reportToolUse(*cu, 0);
          }
        }

        delete cu;
        cu = NULL;
        // nuke the maintainer just in case.
        if (!adding) {
          memset(&maintainer, 0, sizeof(user));
        }
      }
      digitalWrite(D2_LED, LOW);
      if (acsettings.status) {
        rgb.blue();
      }
    }
    return cu;
}

// When we have a card on the reader go round this loop.
void card_loop() {
  
  if (one_sec.check()) {
    Serial.print("card on reader: ");
    dump_user(cu);
  }

  // tool enable etc here.
  if (cu->status == 1) {
    // is the tool running
    if (acsettings.status || cu->maintainer == 1) {
      // this card is authorised to switch the tool on, so switch it on.
      // check tool status against the acserver incase someone has set the tool out of service
      int status = acsettings.status;

      if (network) {
        // only check every 2 seconds
        if (tool_status_check.check()) {
          status = networkCheckToolStatus();
        }
      }

      if (status == 1 || cu->maintainer == 1) {
        tool.on(*cu);
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

      if (cu->status == 1 && cu->maintainer == 0) {
        menu = USER;
        // if we are a user the 1st item on the menu is OFFLINE
        menu_state = OFFLINE;
        // but if we are already offline don't do anything, normal users can't put us offline
        if (acsettings.status == 0) {
          char tmp[128];

          sprintf(tmp, "%s", "User ");
          uid_str(tmp + strlen(tmp), cu);
          sprintf(tmp + strlen(tmp), " tried to use the menu when the tool is offline");
          Serial.println(tmp);
          syslog.syslog(LOG_NOTICE, tmp);

          menu = NOMENU;
        }
      }
      if (cu->maintainer == 1) {
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
  uid_str(tmp + strlen(tmp), cu);
  sprintf(tmp + strlen(tmp), " took the tool out of service.");
  Serial.println(tmp);
  syslog.syslog(LOG_WARNING, tmp);

  int ret = setToolStatus(0, *cu);
  Serial.print("After setting ToolStatus: ");
  Serial.println(ret);

  // and switch the tool off.
  tool.off(*cu);
}

void online(void) {
  acsettings.status = 1;
  set_settings(acsettings);

  char tmp[128];

  sprintf(tmp, "%s", "User ");
  uid_str(tmp + strlen(tmp), cu);
  sprintf(tmp + strlen(tmp), " put the tool in service.");
  Serial.println(tmp);
  syslog.syslog(LOG_WARNING, tmp);

  int ret = setToolStatus(1, *cu);
  Serial.print("After setting ToolStatus: ");
  Serial.println(ret);

  // this is redundant since users can't put a tool back in service, and the tool will be on for a maintainer.
  // but leave it for completeness
  tool.on(*cu);
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
        memcpy(&maintainer, cu, sizeof(user));
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

void readcard()
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
  
  if (!success)
  {
    cc.invalid = 1;
    return;
  }

/*
  Serial.print("Got card with uid: ");
  dumpHex(uid, uidLength);
  Serial.println("");
*/

  switch (uidLength) {
    case 4:
      memcpy(cc.uid, uid, 4);
      cc.uidlen = 0;
      cc.invalid = 0;
      break;
    case 7:
      memcpy(cc.uid, uid, 7);
      cc.uidlen = 1;
      cc.invalid = 0;
      break;
    default:
      Serial.print("Odd card length?: ");
      cc.invalid = 1;
      Serial.println(uidLength);
      break;
  }
  
  nfc.powerDown();
}

boolean doorbot() {
  return acsettings.role == 1;
}
