#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>
#include <driverlib/eeprom.h>

// enough length for acserver.lan.london.hackspace.org.uk + 1 + 4 extra for luck.
#define SERVERNAMELEN (36 + 1 + 4)
// when we read our settings we check this to see if we have valid settings.
#define ACSETTINGSVALID (42)

struct settings {
  uint8_t valid;
  uint8_t mac[6];
  char servername[SERVERNAMELEN];
  int16_t port;
  int32_t nodeid; // are we calling this "node id" or "tool id"?
  uint8_t status; // 0 = out of service, 1 = in service
  char syslogserver[SERVERNAMELEN];
  char toolname[16];
  int8_t padding[1];
};


settings get_settings(void);
int set_settings(settings acsettings);
void init_settings(void);
void dump_settings(settings acsettings);
int clear_settings(void);

#endif

