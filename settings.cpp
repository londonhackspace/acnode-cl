
#include <Energia.h>
#include "settings.h"

settings acsettings;

void init_settings(void) {
    // must be a multiple of 4 cos our eeprom is
  // read and written in 32 bit chunks.
  if (sizeof(acsettings) % 4 != 0) {
    Serial.print("acsettings wrong size, needs to be a multiple of 4: ");
    Serial.println(sizeof(acsettings));
    while (true) {
      ;
    }
  }

  if (sizeof(acsettings) > 128) {
    Serial.print("acsettings wrong size, must be smaller than 128: ");
    Serial.println(sizeof(acsettings));
    while (true) {
      ;
    }
  }

  uint32_t ret, eesize, blocks;

  ret = EEPROMInit();

  if (ret != EEPROM_INIT_OK) {
    Serial.print("EEprom init error: ");
    Serial.println(ret);
  }

  eesize = EEPROMSizeGet();

  Serial.print("EEPROM Size: ");
  Serial.println(eesize);

  blocks = EEPROMBlockCountGet();

  Serial.print("EEPROM no. of blocks: ");
  Serial.println(blocks);

  Serial.print("EEPROM size of each block: ");
  Serial.println(eesize / blocks);
}

void dump_settings(settings acsettings) {
  if (acsettings.valid) {
    Serial.println("Using settings from eeprom");
  } else {
    Serial.println("Using default settings, please change!");
  }

  int i;
  char tmp[3];

  Serial.print("Tool: ");
  Serial.println(acsettings.toolname);

  Serial.print("Mac address: ");

  for (i = 0; i < 6; i++) {
    sprintf(tmp, "%02X\00", acsettings.mac[i]);
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
}


settings get_settings(void) {
  memset(&acsettings, 0, sizeof(acsettings));

  EEPROMRead((uint32_t *)&acsettings, 0, sizeof(acsettings));

  if (acsettings.valid != 42) {
    Serial.println("Settings not valid, using defaults");
  } else {
    return acsettings;
  }

  acsettings.valid = 0;
  acsettings.mac[0] = 0x00;
  acsettings.mac[1] = 0xAA;
  acsettings.mac[2] = 0xBB;
  acsettings.mac[3] = 0xCC;
  acsettings.mac[4] = 0xDE;
  acsettings.mac[5] = 0x02;

  strncpy(acsettings.servername, "acserver.lan.london.hackspace.org.uk", SERVERNAMELEN);
  strncpy(acsettings.syslogserver, "syslog.lan.london.hackspace.org.uk", SERVERNAMELEN);
  strncpy(acsettings.toolname, "ACNode", SERVERNAMELEN);
  acsettings.nodeid = -1;
  acsettings.port = 1234;
  acsettings.status = 0;

  return acsettings;
}

int set_settings(settings acsettings) {
  int ret;

  // hope they are actually valid!
  if (acsettings.valid != 42) {
    acsettings.valid = 42;
  }

  Serial.println("About to write");
  ret = EEPROMProgram((uint32_t *)&acsettings, 0, sizeof(acsettings));
  Serial.println("Done write");

  if (ret != 0) {
    Serial.print("Writeing problem: ");
    Serial.println(ret, HEX);
  }
  return ret;
}

int clear_settings(void) {
  int ret;
  uint32_t no = 0;
  Serial.println("About to clear");
  ret = EEPROMProgram(&no, 0, sizeof(no));
  Serial.println("Done clear");

  if (ret != 0) {
    Serial.print("Writeing problem: ");
    Serial.println(ret, HEX);
  }
  return ret;  
}

