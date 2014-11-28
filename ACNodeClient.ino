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
Tool tool(PG_1);

RGB rgb(PM_0, PM_1, PM_2);

Every one_sec(1000);
Every five_sec(5000);

Every card_every(500);

Button button(PF_1);

void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("\n\nACNode Client Startup");
  // lets use all the LED's
  pinMode(D1_LED, OUTPUT);
  pinMode(D2_LED, OUTPUT);

  digitalWrite(D1_LED, HIGH); // heartbeat
  digitalWrite(D2_LED, LOW);  // lit when we detect a card

  // used for ethernet link and activity
  pinMode(D3_LED, OUTPUT);
  pinMode(D4_LED, OUTPUT);

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


  syslog.begin(acsettings.syslogserver, acsettings.toolname, LOG_LOCAL0);
  syslog.syslog(LOG_NOTICE, "Starting up");

  tool.begin();
  button.begin();
  one_sec.begin();
  five_sec.begin();
  card_every.begin();
  rgb.begin();

  Serial.println("Initialising PN532");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();

  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
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
  // set it even lower to just 16 retries.
  //
  // 0xbd works
  // 0x9e works
  // 0x7f dosn't
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
    if (acsettings.status != status) {
      // update them if they have changed.
      acsettings.status = status;
      set_settings(acsettings);
    }

    char tmp[42];
    snprintf(tmp, 42, "tool status: %s", status ? "in service" : "out of service");
    syslog.syslog(LOG_INFO, tmp);
    snprintf(tmp, 42, "total runtime: ");
    duration_str(tmp + strlen(tmp), acsettings.runtime);
    syslog.syslog(LOG_INFO, tmp);
    rgb.blue();
  } else {
    rgb.orange();
  }

  Serial.println("press enter for a prompt");
}

boolean interactive = false;
unsigned long intstart = 0;

//
// can we do a delay() thats interrupted by the arrival of serial data?
// oh for an rtos and threads/callbacks etc... :(
//
// using the irq from the card reader would help.
//
void check_ser(void) {
  if (Serial.available()) {
    intstart = millis();
    interactive = true;
  }
}

// current user
user *cu = NULL;


// true if we are adding a card
boolean adding = false;

// after the card is removed we have don't shut off the tool immediatly
// some cards only read on alternate cycles, this means the tool stays on
// on the missed reads
int grace_period = 0;
#define GRACE (3)

void loop() {
  user *tu;
  boolean check = true;
  int button_state = NO_PRESS;

  button_state = button.poll();
  if (button_state != NO_PRESS) {
    Serial.print("Button: ");
    Serial.println(button_state);
  }

  if (button_state == SHORT_PRESS) {
    Serial.println("Button down");
  } else if (button_state == LONG_PRESS) {
    Serial.println("Long button press");
  }

  if (!interactive) {
    if (cu != NULL) {
      if (one_sec.check()) {
        Serial.print("user active: ");
        dump_user(cu);
      }
      // tool enable etc here.
      if (cu->status == 1) {
        // this card is authorised to switch the tool on, so switch it on.
        tool.on(*cu);
        if (cu->maintainer == 0) {
          rgb.green();
        }
      } else {
        rgb.orange();
      }
      if (cu->status == 1 && cu->maintainer == 1) {
        rgb.yellow();
        // This user is a maintainer, check the button
        if (button_state == SHORT_PRESS) {
          // the button is pressed, copy our current card details to the maintainer block
           memcpy(&maintainer, cu, sizeof(user));
           // we are adding
           adding = true;
           Serial.println("*Adding*");
        }
      }
    }
  }

  if (!interactive) {
    digitalWrite(D1_LED, HIGH);
  }
  check_ser();

  if (!interactive) {
    // the 1 second delay looking for cards is annoying when trying to use the cli...
    if (card_every.check()) {
      readcard();
    }
    /*
     * we have to:
     *
     * use the uid on the card to fill in the other info about the user
     * if the card is in the cache start with the data there
     * if we can get to the acserver update the data from there,
     * and save it to the cache if needed
     * if the user is allowed to run the tool switch it on.
     */
    if (!cc.invalid) // we have a valid card
    {
      int status;
      digitalWrite(D2_LED, HIGH);
      user *nu;
      nu = new user;
      memset(nu, 0, sizeof(user));
      memcpy(nu, &cc, sizeof(user));

      if (one_sec.check()) {
        Serial.println("got a card");
        dump_user(nu);
      }

      if (!compare_uid(nu, &maintainer)) {
        // are we adding the user?
        if (adding) {
          // check that the button is still being pressed
          if (button_state == SHORT_PRESS) {
            char tmp[128];

            adding = false;

            sprintf(tmp, "%s", "Maintainer ");
            uid_str(tmp + strlen(tmp), &maintainer);
            sprintf(tmp + strlen(tmp), " adding a card ");
            uid_str(tmp + strlen(tmp), nu);
            Serial.println(tmp);
            syslog.syslog(LOG_NOTICE, tmp);

            addNewUser(*nu, maintainer);
          }
        } else {
          // not adding so nuke it.
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
          grace_period = GRACE;
        } else {
          Serial.println("card changed");
          dump_user(tu);
        }
        delete tu;
      } else if (cu != NULL and tu == NULL) {
        // not in the cache.
//        TRACE
//        dump_user(cu);
//        dump_user(nu);
        if (compare_uid(cu, nu)) {
          // no need to check against the server, we've already got the card
          check = false;
          grace_period = GRACE;
        }
      }

      if (network && check) {
        status = querycard(cc);
        Serial.println(status);

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
            dump_user(tu);
          }
        }
      } else {
        // no network or no need to check, cached users only.
        if (one_sec.check()) {
          Serial.println("trying to find cached card");
        }
        tu = get_user(nu);
        if (tu != NULL) {
          TRACE
          dump_user(tu);
          delete nu;
          nu = tu;
        }
      }
      
      if (cu != NULL) {
        delete cu;
      }
      cu = new user;
      memcpy(cu, nu, sizeof(user));
      grace_period = GRACE;
      delete nu;
    } else {
      // no card on the reader
      if (cu != NULL) {
        // the "TEMP" card (uid 040957827B3280) is very wierd
        // and only reads on alternate cycles, so allow a card to disappear for a bit.
        if (grace_period > 0) {
          grace_period--;
        } else {
          Serial.println("Card removed");
          tool.off(*cu);
          delete cu;
          cu = NULL;
          // nuke the maintainer just in case.
          memset(&maintainer, 0, sizeof(user));
          grace_period = 0;
        }
      }
    }
    
    check_ser();
    digitalWrite(D1_LED, LOW);
    digitalWrite(D2_LED, LOW);
    rgb.blue();
  }
  // 10 second timeout
  if (interactive && intstart + (1000 * 1) < millis()) {
    interactive = false;
    intstart = 0;
  }
  // put received char from stdin to microrl lib
  if (Serial.available()) {
    char c;
    c = Serial.read();
    microrl_insert_char (prl, c);
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

  Serial.print("Got card with uid: ");
  dumpHex(uid, uidLength);
  Serial.println("");

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


