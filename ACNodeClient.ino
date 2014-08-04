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
unsigned char serNum[7];


void setup() {
  Serial.begin(9600);
  Serial.println("\n\nACNode Client Startup");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
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
  SPI.setModule(2);
  pinMode(chipSelectPin, OUTPUT); //moved this here from Mfrc522.cpp
  digitalWrite(chipSelectPin, LOW);
  pinMode(NRSTPD, OUTPUT); //moved this here from Mfrc522.cpp
  digitalWrite(NRSTPD, HIGH); //moved this here from Mfrc522.cpp
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  Serial.println("Initialising Mfrc522");
  Mfrc522.Init();
}

void readcard()
{
  unsigned char i,tmp;
  unsigned char status;
  unsigned char str[MAX_LEN];
  unsigned char RC_size;
  unsigned char blockAddr;
  String mynum = "";


  status = Mfrc522.Request(PICC_REQIDL, str);
  status = Mfrc522.Anticoll(str);
  memcpy(serNum, str, 7);
  if (status == MI_OK)
  {
    digitalWrite(GREEN_LED, HIGH);
    delay(1000);
    digitalWrite(GREEN_LED, LOW);
    Serial.println("The card's number is  : ");
    Serial.print(serNum[0], HEX);
    Serial.print(" , ");
    Serial.print(serNum[1], HEX);
    Serial.print(" , ");
    Serial.print(serNum[2], HEX);
    Serial.print(" , ");
    Serial.print(serNum[3], HEX);
    Serial.print(" , ");
    Serial.print(serNum[4], HEX);
    Serial.print(" , ");
    Serial.print(serNum[5], HEX);
    Serial.print(" , ");
    Serial.print(serNum[6], HEX);
    Serial.print(" , ");
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
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }

}

void loop(){
  readcard();
  delay(2000);
}




