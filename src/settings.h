#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>
#include <SD.h>

// enough length for acserver.lan.london.hackspace.org.uk + 1 + 4 extra for luck.
#define SERVERNAMELEN (36 + 1 + 4)

#define TOOLNAMELEN (16)

#define MQTT_TOPIC_LEN (32)

// when we read our settings we check this to see if we have valid settings.
#define ACSETTINGS42 (42)
#define ACSETTINGS43 (43)
#define ACSETTINGS44 (44)
#define ACSETTINGS45 (45)
#define ACSETTINGS46 (46)
#define ACSETTINGSVALID (47)

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

struct settings43 {
  uint8_t valid;
  uint8_t mac[6];
  char servername[SERVERNAMELEN];
  uint16_t port;
  uint16_t nodeid; // are we calling this "node id" or "tool id"?
  unsigned int status  :1; // 0 = out of service, 1 = in service
  unsigned int sdcache :1; // 1 = use sdcard for the cache, 0 = use the eeprom
  unsigned int netverbose :1; // 1 = verbose network replies, 0 = be quiet
  unsigned int toolonpin_activehigh :1; // 1 = if active high (default), 0 = active low
  unsigned int toolrunpin_activehigh :1; // 1 = if active high, 0 = active low (default cos of the lasercutter)
  char syslogserver[SERVERNAMELEN];
  char toolname[16];
  uint32_t runtime; // total seconds the tool has run
  uint8_t minontime; // the minimum time the tool should be on for.
  uint8_t padding; // to make this struct a multiple of 4
} __attribute__ ((packed));

#define KEYLEN 8

struct settings44 {
  uint8_t valid;
  uint8_t mac[6];
  char servername[SERVERNAMELEN];
  uint16_t port;
  uint16_t nodeid; // are we calling this "node id" or "tool id"?
  unsigned int status  :1; // 0 = out of service, 1 = in service
  unsigned int sdcache :1; // 1 = use sdcard for the cache, 0 = use the eeprom
  unsigned int netverbose :1; // 1 = verbose network replies, 0 = be quiet
  unsigned int toolonpin_activehigh :1; // 1 = if active high (default), 0 = active low
  unsigned int toolrunpin_activehigh :1; // 1 = if active high, 0 = active low (default cos of the lasercutter)
  char syslogserver[SERVERNAMELEN];
  char toolname[16];
  uint32_t runtime; // total seconds the tool has run
  uint8_t minontime; // the minimum time the tool should be on for.
  char secret[KEYLEN + 1]; // API key to talk to the acserver
  uint8_t role; // 0 - regular acnode, 1 - doorbot, 2 - auditor
  uint16_t announce_port;
  char __padding[1];
} __attribute__ ((packed));

struct settings45 {
    uint8_t valid;
    uint8_t mac[6];
    char servername[SERVERNAMELEN];
    uint16_t port;
    uint16_t nodeid; // are we calling this "node id" or "tool id"?
    unsigned int status  :1; // 0 = out of service, 1 = in service
    unsigned int sdcache :1; // 1 = use sdcard for the cache, 0 = use the eeprom
    unsigned int netverbose :1; // 1 = verbose network replies, 0 = be quiet
    unsigned int toolonpin_activehigh :1; // 1 = if active high (default), 0 = active low
    unsigned int toolrunpin_activehigh :1; // 1 = if active high, 0 = active low (default cos of the lasercutter)
    char syslogserver[SERVERNAMELEN];
    char toolname[16];
    uint32_t runtime; // total seconds the tool has run
    uint8_t minontime; // the minimum time the tool should be on for.
    char secret[KEYLEN + 1]; // API key to talk to the acserver
    uint8_t role; // 0 - regular acnode, 1 - doorbot, 2 - auditor
    uint16_t announce_port;
    uint8_t announce_mode; // 0 - off, 1 - broadcast 2 - mqtt
    char mqtt_server[SERVERNAMELEN];
    uint16_t mqtt_port;
    char mqtt_topic_base[MQTT_TOPIC_LEN];
    uint16_t door_keep_open_ms;
    uint8_t doorbell_mode; // 0 - off
    char __padding[2];
} __attribute__ ((packed));

struct settings46 {
    uint8_t valid;
    uint8_t mac[6];
    char servername[SERVERNAMELEN];
    uint16_t port;
    uint16_t nodeid; // are we calling this "node id" or "tool id"?
    unsigned int status  :1; // 0 = out of service, 1 = in service
    unsigned int sdcache :1; // 1 = use sdcard for the cache, 0 = use the eeprom
    unsigned int netverbose :1; // 1 = verbose network replies, 0 = be quiet
    unsigned int toolonpin_activehigh :1; // 1 = if active high (default), 0 = active low
    unsigned int toolrunpin_activehigh :1; // 1 = if active high, 0 = active low (default cos of the lasercutter)
    unsigned int maintainer_cache :1; // 1 = use persistent (eeprom) maintainer cache
    char syslogserver[SERVERNAMELEN];
    char toolname[16];
    uint32_t runtime; // total seconds the tool has run
    uint8_t minontime; // the minimum time the tool should be on for.
    char secret[KEYLEN + 1]; // API key to talk to the acserver
    uint8_t role; // 0 - regular acnode, 1 - doorbot, 2 - auditor
    uint16_t announce_port;
    uint8_t announce_mode; // 0 - off, 1 - broadcast 2 - mqtt
    char mqtt_server[SERVERNAMELEN];
    uint16_t mqtt_port;
    char mqtt_topic_base[MQTT_TOPIC_LEN];
    uint16_t door_keep_open_ms;
    uint8_t doorbell_mode; // 0 - off
    char __padding[2];
} __attribute__ ((packed));

struct settings {
    uint8_t valid;
    uint8_t mac[6];
    char servername[SERVERNAMELEN];
    uint16_t port;
    uint16_t nodeid; // are we calling this "node id" or "tool id"?
    unsigned int status  :1; // 0 = out of service, 1 = in service
    unsigned int sdcache :1; // 1 = use sdcard for the cache, 0 = use the eeprom
    unsigned int netverbose :1; // 1 = verbose network replies, 0 = be quiet
    unsigned int toolonpin_activehigh :1; // 1 = if active high (default), 0 = active low
    unsigned int toolrunpin_activehigh :1; // 1 = if active high, 0 = active low (default cos of the lasercutter)
    unsigned int maintainer_cache :1; // 1 = use persistent (eeprom) maintainer cache
    unsigned int invert_colours : 1; // 1 = invert colours on LED
    char syslogserver[SERVERNAMELEN];
    char toolname[16];
    uint32_t runtime; // total seconds the tool has run
    uint8_t minontime; // the minimum time the tool should be on for.
    char secret[KEYLEN + 1]; // API key to talk to the acserver
    uint8_t role; // 0 - regular acnode, 1 - doorbot, 2 - auditor
    uint16_t announce_port;
    uint8_t announce_mode; // 0 - off, 1 - broadcast 2 - mqtt
    char mqtt_server[SERVERNAMELEN];
    uint16_t mqtt_port;
    char mqtt_topic_base[MQTT_TOPIC_LEN];
    uint16_t door_keep_open_ms;
    uint8_t doorbell_mode; // 0 - off
    char __padding[2];
} __attribute__ ((packed));

extern settings acsettings;

settings get_settings(void);
int set_settings(settings acsettings);
void init_settings(void);
void dump_settings(settings acsettings);
int clear_settings(void);
uint8_t get_settings_version(void);
void print_buildinfo();

#endif
