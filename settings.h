#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>
#include <SD.h>

// enough length for acserver.lan.london.hackspace.org.uk + 1 + 4 extra for luck.
#define SERVERNAMELEN (36 + 1 + 4)

#define TOOLNAMELEN (16)

// when we read our settings we check this to see if we have valid settings.
#define ACSETTINGS42 (42)
#define ACSETTINGSVALID (43)

struct settings42 {
  uint8_t valid;
  uint8_t mac[6];
  char servername[SERVERNAMELEN];
  uint16_t port;
  int32_t nodeid; // are we calling this "node id" or "tool id"?
  uint8_t status; // 0 = out of service, 1 = in service
  char syslogserver[SERVERNAMELEN];
  char toolname[TOOLNAMELEN];
  uint32_t runtime; // total seconds the tool has run
  uint8_t minontime; // the minimum time the tool should be on for.
  // 4 more bytes space
};

struct settings {
  uint8_t valid;
  uint8_t mac[6];
  char servername[SERVERNAMELEN];
  uint16_t port;
  uint16_t nodeid; // are we calling this "node id" or "tool id"?
  unsigned int status  :1; // 0 = out of service, 1 = in service
  unsigned int sdcache :1; // 1 = use sdcard for the cache, 0 = use the eeprom
  char syslogserver[SERVERNAMELEN];
  char toolname[16];
  uint32_t runtime; // total seconds the tool has run
  uint8_t minontime; // the minimum time the tool should be on for.
  uint8_t padding; // to make this struct a multiple of 4
} __attribute__ ((packed));

settings get_settings(void);
int set_settings(settings acsettings);
void init_settings(void);
void dump_settings(settings acsettings);
int clear_settings(void);

#endif

