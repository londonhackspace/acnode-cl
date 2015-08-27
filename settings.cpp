
#include <Energia.h>
#include "settings.h"
#include "acnode.h"
#include "utils.h"


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
  
  Serial.print("role: ");
  switch(acsettings.role) {
    case 0:
      Serial.println("regular acnode (0)");
      break;
    case 1:
      Serial.println("doorbot (1)");
      break;
    default:
      Serial.println("unknown");
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
  acsettings.role = 0;

  // save the settings since it's a new board.
  acsettings.valid = 42;
  set_settings(acsettings);

  return acsettings;
}

int set_settings(settings acsettings) {
  int ret;

  // hope they are actually valid!
  if (acsettings.valid != 42) {
    acsettings.valid = 42;
  }

  ret = EEPROMProgram((uint32_t *)&acsettings, 0, sizeof(acsettings));

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
    Serial.print("Writeing settings problem: ");
    Serial.println(ret, HEX);
  }
  return ret;  
}

