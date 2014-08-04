
/*
ACNodeClient
 v0.1
 tgreer@tsone.net.uk
 
 */
#include <Ethernet.h>
#include <Mfrc522.h>
#include <SPI.h>

byte mac[] = { 
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
EthernetClient client;

char serverName[] = "babbage.lan.london.hackspace.org.uk";
int chipSelectPin = 42;
int NRSTPD = 13;
Mfrc522 Mfrc522(chipSelectPin, NRSTPD);
unsigned char serNum[5];
unsigned char serNum7[8];
enum cardTypes { CARDUID4, CARDUID7, NOCARD };
cardTypes cardType = NOCARD;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\nACNode Client Startup");
  // start the Ethernet connection:

  if (Ethernet.begin(mac) == 0) {
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

  Serial.println("Initialising Mfrc522");

  SPI.setModule(2);

  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, LOW);
  pinMode(NRSTPD, OUTPUT);   
  digitalWrite(NRSTPD, HIGH);

  Mfrc522.Init();
}

void readcard()
{
  unsigned char i,tmp;
  unsigned char status;
  unsigned char temp[MAX_LEN];
  unsigned char RC_size;
  unsigned char blockAddr;
  String mynum = "";

  temp[1] = 0;
  temp[2] = 0;

  status = Mfrc522.Request(PICC_REQIDL, temp);

  if (status != MI_OK)
  {
    cardType = NOCARD;
    return;
  }
  Serial.print("Card detected. ATQA:");
  dumpHex((char*)temp, 2);
  Serial.println("");

  status = Mfrc522.Anticoll(temp);
  if (status == MI_OK)
  {
    memcpy(serNum, temp, 5);
    unsigned char sak = 0;

    // status here is actually the size of something.
    status = Mfrc522.SelectTag(serNum, &sak);

    if ((sak & 0x04) == 0x00)
    {
      //      Serial.print(" UID: ");
      dumpHex((char*)serNum, 4);
      Serial.println("");
      cardType = CARDUID4;
      
      if ((sak & 0x20) == 0x20)
      {
        //ISO/IEC FCD 14443-3: Table 9 — Coding of SAK
        Serial.println(" UID complete, PICC compliant with ISO/IEC 14443-4");
        //send RATS (Request for Answer To Select)

        unsigned char ats[MAX_LEN];
        unsigned int unLen = 0;
        status = Mfrc522.RATS(ats, &unLen);

        if (status == MI_OK)
        {
          Serial.println(" ATS: ");
          dumpHex((char*)ats, ats[0]);
          Serial.println("");
        }
      }
      /*      Serial.print(" SAK: ");
       Serial.print(sak, HEX);
       Serial.println("");*/
      return;
    }
    else
    {
      //cascading level 2
      memcpy(serNum7, &serNum[1], 3);//cascading L1
      status = Mfrc522.Anticoll2(temp);
      if (status == MI_OK)
      {
        memcpy(&serNum7[3], temp, 4);
        status = Mfrc522.SelectTag2(&serNum7[3], &sak);
        //        Serial.print(" UID: ");
        dumpHex((char*)serNum7, 7);
        Serial.println("");

        cardType = CARDUID7;

        /*        Serial.print(" SAK: ");
         Serial.print(sak, HEX);
         Serial.println("");*/
        return;
      }
      else
      {
        Serial.println("ANTICOLL error: cascading level 2");
      }
    }
  }

  Mfrc522.Halt();       
}


void querycard()
{
  Serial.print("Connecting to http://");
  Serial.println(serverName);

  if (client.connect(serverName, 1234)) {
    Serial.println("Connected");
    Serial.println("Querying");
    client.println("GET /1/card/0477861A402480");
    client.println();

    delay(50);

    if (client.available()) {
      Serial.println("Got Response:\n");
      char c = client.read();
      Serial.print(c);
      Serial.println();
      Serial.println("Disconnecting");
      client.stop();
    }
    Serial.println();

  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop(){
  digitalWrite(RED_LED, HIGH);
  readcard();
  if (cardType != NOCARD)
  {
    digitalWrite(GREEN_LED, HIGH);
    querycard();
  }
  delay(500);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  delay(500);
}



void dumpHex(char* buffer, int len)
{
  for(byte i=0; i < len; i++) {
    char text[4];
    sprintf(text, "%02X\x00", (byte)(*(buffer + i)));
    Serial.print(text);

  }
  //Serial.println(" ");
}





