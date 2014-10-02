/*
ACNodeClient
 v0.2+
 
 */
#include <Ethernet.h>

#define DEBUG 1
#include <PN532_debug.h>
#include <PN532_HSU.h>
#include <PN532.h>

#include "settings.h"
#include "microrl.h"
#include "cli.h"
#include "user.h"

extern settings acsettings;

// create microrl object and pointer on it
microrl_t rl;
microrl_t * prl = &rl;

PN532_HSU pnhsu(Serial6);
PN532 nfc(pnhsu);

EthernetClient client;

unsigned char serNum[5];
unsigned char serNum7[8];

enum cardTypes { CARDUID4, CARDUID7, NOCARD };

cardTypes cardType = NOCARD;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nACNode Client Startup");
  // lets use all the LED's
  pinMode(D1_LED, OUTPUT);
  pinMode(D2_LED, OUTPUT);
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
    // no point in carrying on, so do nothing forevermore:
    // ^-- err, lol?
    while(true);
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);

  Serial.println("Initialising PN532");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();

  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
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
  
  Serial.println("Checking tool status");
  int status = networkCheckToolStatus();
  Serial.println(status);
  acsettings.status = status;

  Serial.println("press enter for a prompt");
}

boolean interactive = false;
unsigned long intstart = 0;

//
// can we do a delay() thats interrupted by the arrival of serial data?
// oh for an rtos and threads/callbacks etc... :(
//
void check_ser(void) {
  if (Serial.available()) {
    intstart = millis();
    interactive = true;
  }
}

void loop(){
  if (!interactive) {
    digitalWrite(RED_LED, HIGH);
  }
  check_ser();


  if (!interactive) {
    // the 1 second delay looking for cards is annoying when trying to use the cli...
    readcard();
    if (cardType != NOCARD)
    {
      digitalWrite(GREEN_LED, HIGH);
      querycard();
    }
    check_ser();
    delay(500);
    check_ser();
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    check_ser();
    delay(500);
    check_ser();
  }
  // 30 second timeout
  if (interactive && intstart + (1000 * 30) < millis()) {
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
  unsigned char i,tmp;
  unsigned char status;
  unsigned char RC_size;
  unsigned char blockAddr;
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

  dumpHex(uid, uidLength);

  switch (uidLength) {
    case 4:
      memcpy(serNum, uid, 4);
      serNum[4] = 0;
      cardType = CARDUID4;
      break;
    case 7:
      memcpy(serNum7, uid, 7);
      serNum[7] = 0;
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
  int i;
  int result = -1;

  if (cardType == NOCARD) {
    return -1;
  }
  
  Serial.print("Connecting to http://");
  Serial.println(acsettings.servername);

  if (client.connect(acsettings.servername, acsettings.port)) {
    Serial.println("Connected");
    Serial.println("Querying");
    sprintf(path, "GET /%d/card/", acsettings.nodeid);

    if (cardType == CARDUID4) {
      for(byte i=0; i < 4; i++) {
        sprintf(path + strlen(path), "%02X\00", serNum[i]);
      }
    }
    else if (cardType == CARDUID7) {
      for(byte i=0; i < 7; i++) {
        sprintf(path + strlen(path), "%02X\00", serNum7[i]);
      }
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
        if (result == -1 && isdigit(c)) {
          result = c - '0';
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
  int i;
  int result = -1;
  
  Serial.print("Connecting to http://");
  Serial.println(acsettings.servername);

  if (client.connect(acsettings.servername, acsettings.port)) {
    Serial.println("Querying");
    sprintf(path, "GET /%d/status/", acsettings.nodeid);

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
        if (result == -1) {
          result = c - '0';
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

void dumpHex(const uint8_t* buffer, int len)
{
  for(byte i=0; i < len; i++) {
    char text[4];
    sprintf(text, "%02X\x00", (byte)(*(buffer + i)));
    Serial.print(text);

  }
  Serial.println(" ");
}

