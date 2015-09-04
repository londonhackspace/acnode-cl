#include <Energia.h>
#include "settings.h"
#include "acnode.h"
#include "utils.h"

#define SD_CS_PIN PC_7
#define ACNODE_DIR "ACNODE"
#define ACNODE_CFG "ACNODE/CONFIG"

void init_settings(void) {
  if (SD.begin(SD_CS_PIN, SPI_HALF_SPEED, 2)) {
    Serial.println("SD card is accessible");
  } else {
    Serial.println("SD card could not be accessed");
  }
  
  SD.mkdir(ACNODE_DIR);
}

void dump_settings(settings acsettings) {
  if (acsettings.valid) {
    Serial.println("Using settings from SD card");
  } else {
    Serial.println("Using default settings, please change!");
  }

  int i;
  char tmp[10];

  Serial.print("Tool: ");
  Serial.println(acsettings.toolname);

  Serial.print("Mac address: ");

  for (i = 0; i < 6; i++) {
    sprintf(tmp, "%02X", acsettings.mac[i]);
    Serial.print(tmp);
    if (i < 5)
      Serial.print(":");
  }
  Serial.println("");

  Serial.print("Server hostname: ");
  if (isprint(acsettings.servername[0])) {
    Serial.println(acsettings.servername);
  } else {
     Serial.println("!Invalid!");
  }
  
  Serial.print("Port: ");
  Serial.println(acsettings.port);
  
  Serial.print("nodeid: ");
  Serial.println(acsettings.nodeid);

  Serial.print("status: ");
  if (acsettings.status == 0) {
    Serial.println("out of service");
  } else if (acsettings.status == 1) {
    Serial.println("in service");
  } else {
    Serial.println("invalid status");
  }

  Serial.print("Syslog Server: ");
  if (isprint(acsettings.syslogserver[0])) {
    Serial.println(acsettings.syslogserver);
  } else {
     Serial.println("!Invalid!");
  }

  Serial.print("Total runtime: ");
  duration_str(tmp, acsettings.runtime);
  Serial.println(tmp);
}


settings get_settings(void) {
  memset(&acsettings, 0, sizeof(acsettings));
  
  if (SD.exists(ACNODE_CFG)) {
    File f = SD.open(ACNODE_CFG, FILE_READ);
    f.read(&acsettings, sizeof(acsettings));
    f.close();
  }

  if (acsettings.valid != 42) {
    Serial.println("Settings not valid, using defaults");
  } else {
    return acsettings;
  }

  acsettings.valid = 0;

  uint32_t ui32User0, ui32User1;

  ROM_FlashUserGet(&ui32User0, &ui32User1);

  acsettings.mac[0] = ((ui32User0 >> 0) & 0xff);
  acsettings.mac[1] = ((ui32User0 >> 8) & 0xff);
  acsettings.mac[2] = ((ui32User0 >> 16) & 0xff);
  acsettings.mac[3] = ((ui32User1 >> 0) & 0xff);
  acsettings.mac[4] = ((ui32User1 >> 8) & 0xff);
  acsettings.mac[5] = ((ui32User1 >> 16) & 0xff);

  strncpy(acsettings.servername, "acserver.lan.london.hackspace.org.uk", SERVERNAMELEN);
  strncpy(acsettings.syslogserver, "syslog.lan.london.hackspace.org.uk", SERVERNAMELEN);
  strncpy(acsettings.toolname, "ACNode", SERVERNAMELEN);
  acsettings.nodeid = -1;
  acsettings.port = 1234;
  acsettings.status = 0;
  acsettings.runtime = 0;

  // save the settings since it's a new board.
  acsettings.valid = 42;
  set_settings(acsettings);

  return acsettings;
}

int set_settings(settings acsettings) {
  // hope they are actually valid!
  if (acsettings.valid != 42) {
    acsettings.valid = 42;
  }
  
  SD.remove(ACNODE_CFG);
  File f = SD.open(ACNODE_CFG, FILE_WRITE);
  f.write((uint8_t *)&acsettings, sizeof(acsettings));
  f.close();

  if (SD.getErrorCode() != 0) {
    Serial.print("SD Card Error: ");
    Serial.println(SD.getErrorCode());
  }
  return 0;
}

int clear_settings(void) {
  SD.remove(ACNODE_CFG);
}

