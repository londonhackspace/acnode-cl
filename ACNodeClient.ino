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

extern settings acsettings;

// create microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

PN532_HSU pnhsu(Serial6);
PN532 nfc(pnhsu);

EthernetClient client;
boolean network = false;
Syslog syslog;
Tool tool(PG_1);

unsigned char serNum[8];

enum cardTypes { CARDUID4, CARDUID7, NOCARD };

cardTypes cardType = NOCARD;

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

  // start the Ethernet connection:
  if (Ethernet.begin(acsettings.mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  } else {
    network = true;
    Ethernet.enableLinkLed();
    Ethernet.enableActivityLed();
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
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();

  if (network) {
    Serial.println("Checking tool status");
    int status = networkCheckToolStatus();
    Serial.println(status);
    acsettings.status = status;
    char tmp[42];
    snprintf(tmp, 42, "tool status: %d", status);
    syslog.syslog(LOG_INFO, tmp);
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
int grace_period = 0;

void loop() {
  user *tu;
  boolean check = true;

  if (!interactive) {
    if (cu != NULL) {
      Serial.print("user active: ");
      dump_user(cu);
      // tool enable etc here.
      if (cu->status == 1) {
        // this card is authorised to switch the tool on, so switch it on.
        tool.on();
      }
    }
  }

  if (!interactive) {
    digitalWrite(D1_LED, HIGH);
  }
  check_ser();

  if (!interactive) {
    // the 1 second delay looking for cards is annoying when trying to use the cli...
    readcard();
    if (cardType != NOCARD)
    {
      int status;
      digitalWrite(D2_LED, HIGH);
      user *nu;
      nu = new user;
      memset(nu, 0, sizeof(user));

      if (cardType == CARDUID7) {
        nu->uidlen = 1;
        memcpy(nu->uid, serNum, 7);
      } else {
        memcpy(nu->uid, serNum, 4);
      }

      // we have a possibly new card, check it against the cache.
      tu = get_user(nu);

      // was it in the cache?
      if (tu != NULL && cu != NULL) {
        // same as current card?
        if (compare_user(cu, tu)) {
          Serial.println("card not changed");
          // no need to check against the server.
          check = false;
          grace_period = 3;
        }
        delete tu;
      } else if (cu != NULL and tu == NULL) {
        // not in the cache.
        dump_user(cu);
        dump_user(nu);
        if (compare_uid(cu, nu)) {
          // no need to check against the server, we've already got the card
          check = false;
          grace_period = 3;
        }
      }

      if (network && check) {
        status = querycard();
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
        Serial.println("trying to find cached card");
        tu = get_user(nu);
        if (tu != NULL) {
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
      grace_period = 3;
      delete nu;
    } else {
      if (cu != NULL) {
        // the "TEMP" card (uid 040957827B3280) is very wierd
        // and only reads on alternate cycles, so allow a card to disappear for a bit.
        if (grace_period > 0) {
          grace_period--;
        } else {
          Serial.println("Card removed");
          tool.off();
          delete cu;
          cu = NULL;
          grace_period = 0;
        }
      }
    }
    
    check_ser();
    delay(500);
    check_ser();
    digitalWrite(D1_LED, LOW);
    digitalWrite(D2_LED, LOW);
    check_ser();
    delay(500);
    check_ser();
  }
  // 10 second timeout
  if (interactive && intstart + (1000 * 10) < millis()) {
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

  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (!success)
  {
    cardType = NOCARD;
    return;
  }

  Serial.print("Got card with uid: ");
  dumpHex(uid, uidLength);
  Serial.println("");

  switch (uidLength) {
    case 4:
      memcpy(serNum, uid, 4);
      cardType = CARDUID4;
      break;
    case 7:
      memcpy(serNum, uid, 7);
      cardType = CARDUID7;
      break;
    default:
      Serial.print("Odd card length?: ");
      cardType = NOCARD;
      Serial.println(uidLength);
      break;
  }
}

int querycard()
{
  char path[13 + 14 + 1];
  int result = -1;
  boolean first = true;

  if (cardType == NOCARD) {
    return -1;
  }
  
  Serial.print("Connecting to http://");
  Serial.println(acsettings.servername);

  if (client.connect(acsettings.servername, acsettings.port)) {
    Serial.println("Connected");
    Serial.println("Querying");
    sprintf(path, "GET /%ld/card/", acsettings.nodeid);

    int uidlen = 0;
    
    if (cardType == CARDUID4) {
      uidlen = 4;
    } else if (cardType == CARDUID7) {
      uidlen = 7;
    }
    for(byte i=0; i < uidlen; i++) {
      sprintf(path + strlen(path), "%02X", serNum[i]);
    }
    Serial.println(path);
    client.println(path);
    client.print("Host: ");
    client.println(acsettings.servername);
    client.println();

    int timeout = 0;

    while (!client.available()) {
       delay(25);
       timeout++;
       if (timeout > 400) {
         Serial.println("Timeout :(");
         break;
       }
    }

    if (client.available()) {
      char c;
      Serial.print("Got Response: >");

      while (client.available()) {
        c = client.read();
        Serial.print(c);
        if (c == '\n') {
          Serial.print('\r');
        }
        // we only want the 1st char returned
        if (first) {
          if (isdigit(c)) {
            result = c - '0';
          }
          first = false;
        }
      }

      Serial.println("<");
      Serial.println("Disconnecting");
      client.flush();
      client.stop();
    }
    Serial.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  // if it's 2 it's a maintainer.
  if (result == 1 || result == 2) {
    Serial.println("access granted");
  } else {
    Serial.println("access denied");
  }
  return result;
}

bool networkCheckToolStatus()
{
  char path[13 + 2];
  int result = -1;
  boolean first = true;
  
  Serial.print("Connecting to http://");
  Serial.println(acsettings.servername);

  if (client.connect(acsettings.servername, acsettings.port)) {
    Serial.println("Querying");
    sprintf(path, "GET /%ld/status/", acsettings.nodeid);

    Serial.println(path);
    client.println(path);
    client.print("Host: ");
    client.println(acsettings.servername);
    client.println();

    int timeout = 0;

    while (!client.available()) {
       delay(25);
       timeout++;
       if (timeout > 400) {
         Serial.println("Timeout :(");
         break;
       }
    }

    if (client.available()) {
      char c;
      Serial.print("Got Response: >");

      while (client.available()) {
        c = client.read();
        Serial.print(c);
        if (c == '\n') {
          Serial.print('\r');
        }
        if (first) {
          if (isdigit(c)) {
            result = c - '0';
          }
          first = false;
        }
      }

      Serial.println("<");
      Serial.println("Disconnecting");
      client.flush();
      client.stop();
    }
    Serial.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  // if it's 2 it's a maintainer.
  if (result == 1) {
    Serial.println("tool is in service");
    return true;
  } else {
    Serial.println("tool out of service");
  }
  
  return false;
}


