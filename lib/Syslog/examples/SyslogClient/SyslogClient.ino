#include <Ethernet.h>
#include <EthernetUdp.h>
#include <EthernetClient.h>
#include <EthernetServer.h>

#include "syslog.h"

byte mac[] = { 0x00, 0x42, 0x42, 0x42, 0x42, 0x42 };

Syslog syslog;

void setup()
{
  Serial.begin(9600);
  Serial.println("");
  Serial.println("\n\nSyslog test Startup");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    while (1); // loop forever. 
  }

  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();

  // Change the log server below
  syslog.begin("syslog.lan.london.hackspace.org.uk", "SyslogTest", LOG_LOCAL0);
  syslog.syslog(LOG_INFO, "Hello world");
  syslog.syslog(LOG_EMERG, "Emergency!");
  syslog.syslog(LOG_DEBUG, "debugging");  
}

void loop()
{

  while(1);
  
}
