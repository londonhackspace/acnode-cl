#include <Energia.h>
#include "settings.h"
#include "acnode.h"
#include "utils.h"

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

  Serial.print("setting left: ");
  Serial.println(384 - sizeof(acsettings));

  if (sizeof(acsettings) > 384) {
    Serial.print("acsettings wrong size, must be smaller than 384: ");
    Serial.println(sizeof(acsettings));
    while (true) {
      ;
    }
  }

  uint32_t ret;

  ret = EEPROMInit();

  if (ret != EEPROM_INIT_OK) {
    Serial.print("EEprom init error: ");
    Serial.println(ret);
  }
}

void dump_settings(settings acsettings) {
  print_buildinfo();

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

  Serial.print("Server API key: ");
  if (isprint(acsettings.secret[0])) {
    Serial.println(acsettings.secret);
  } else {
     Serial.println("Not Set!");
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

  Serial.print("Minimum on time: ");
  Serial.println(acsettings.minontime);

  Serial.print("Using ");
  if (acsettings.sdcache) {
    Serial.print("a SD Card");
  } else {
    Serial.print("the internal eeprom");
  }
  Serial.println(" to cache cards.");

  if(acsettings.maintainer_cache) {
    Serial.println("Maintainer cache enabled.");
  } else {
    Serial.println("Maintainer cache disabled.");
  }

  if (acsettings.netverbose) {
    Serial.println("Verbose network messages.");
  } else {
    Serial.println("Being quiet about the network.");
  }

  Serial.print("The tool will be powered on by switching the toolonpin ");
  if (acsettings.toolonpin_activehigh) {
    Serial.println("HIGH.");
  } else {
    Serial.println("LOW.");
  }

  Serial.print("The acnode will detect that the tool is running when the toolrunpin is ");
  if (acsettings.toolrunpin_activehigh) {
    Serial.println("HIGH.");
  } else {
    Serial.println("LOW.");
  }

  Serial.print("This node behaves like a: ");
  switch(acsettings.role) {
    case 0:
      Serial.println("regular acnode");
      break;
    case 1:
      Serial.println("doorbot");
      break;
    case 2:
      Serial.println("doorbot with access control");
      break;
    case 3:
      Serial.println("auditing only");
  }

  if (acsettings.role > 0) {
    if(acsettings.announce_mode == 1) {
        Serial.print("Announcer port: ");
        Serial.println(acsettings.announce_port);
    }
    Serial.print("Door Keep Open Milliseconds: ");
    Serial.println(acsettings.door_keep_open_ms);
  }

  Serial.print("Announcer in ");
  switch(acsettings.announce_mode) {
      case 0:
        Serial.print("Disabled");
        break;
      case 1:
        Serial.print("Broadcast");
        break;
      case 2:
        Serial.print("MQTT");
  }
  Serial.println(" mode");

  if(acsettings.announce_mode == 2) {
    Serial.print("MQTT Server: ");
    if(isprint(acsettings.mqtt_server[0])) {
      Serial.println(acsettings.mqtt_server);
    } else {
      Serial.println("!Invalid!");
    }

    Serial.print("MQTT Port: ");
    Serial.println(acsettings.mqtt_port);

    Serial.print("MQTT Topic Base: ");
    if(isprint(acsettings.mqtt_topic_base[0])) {
      Serial.println(acsettings.mqtt_topic_base);
    } else {
      Serial.println("!Invalid!");
    }
  }

  Serial.print("Invert Colours: ");
  Serial.println((acsettings.invert_colours == 1) ? "on" : "off");

}

uint8_t get_settings_version(void) {
  uint32_t scratch;
  EEPROMRead((uint32_t *)&scratch, 0, sizeof(scratch));
  return scratch & 0xff;
}

settings get_settings(void) {
  memset(&acsettings, 0, sizeof(acsettings));

  EEPROMRead((uint32_t *)&acsettings, 0, sizeof(acsettings));

  switch (acsettings.valid) {
    case ACSETTINGSVALID:
      // nothing to do
      break;
    case ACSETTINGS42:
      // upgrade the settings and add defaults for new settings.
      Serial.println("Old settings (42) found, upgrading");
      {
        settings42 osettings;
        memset(&osettings, 0, sizeof(osettings));
        EEPROMRead((uint32_t *)&osettings, 0, sizeof(osettings));

        memcpy(acsettings.mac, osettings.mac, 6);
        memcpy(acsettings.servername, osettings.servername, SERVERNAMELEN);
        memcpy(acsettings.syslogserver, osettings.syslogserver, SERVERNAMELEN);
        memcpy(acsettings.toolname, osettings.toolname, TOOLNAMELEN);
        acsettings.port = osettings.port;
        acsettings.nodeid = osettings.nodeid;
        acsettings.status = osettings.status;
        acsettings.runtime = osettings.runtime;
        acsettings.minontime = 5;
        acsettings.sdcache = 0;
        acsettings.netverbose = 1;
        acsettings.toolonpin_activehigh = 1;
        // the laser signal is inverted
        acsettings.toolrunpin_activehigh = 0;
        memset(acsettings.secret, 0, KEYLEN+1);
      }

      acsettings.valid = ACSETTINGSVALID;
      set_settings(acsettings);
      break;
    case ACSETTINGS43:
      // upgrade the settings and add defaults for new settings.
      Serial.println("Old settings (43) found, upgrading");
      {
        settings43 osettings;
        memset(&osettings, 0, sizeof(osettings));
        EEPROMRead((uint32_t *)&osettings, 0, sizeof(osettings));

        memcpy(acsettings.mac, osettings.mac, 6);
        memcpy(acsettings.servername, osettings.servername, SERVERNAMELEN);
        memcpy(acsettings.syslogserver, osettings.syslogserver, SERVERNAMELEN);
        memcpy(acsettings.toolname, osettings.toolname, TOOLNAMELEN);
        acsettings.port = osettings.port;
        acsettings.nodeid = osettings.nodeid;
        acsettings.status = osettings.status;
        acsettings.runtime = osettings.runtime;
        acsettings.minontime = osettings.minontime;
        acsettings.sdcache = osettings.sdcache;
        acsettings.netverbose = osettings.netverbose;
        acsettings.toolonpin_activehigh = osettings.toolonpin_activehigh;
        acsettings.toolrunpin_activehigh = osettings.toolrunpin_activehigh;

        memset(acsettings.secret, 0, KEYLEN+1);

        acsettings.valid = ACSETTINGSVALID;
        set_settings(acsettings);
      }
      break;
      case ACSETTINGS44:
          // upgrade the settings and add defaults for new settings.
          Serial.println("Old settings (44) found, upgrading");
          {
            settings44 osettings;
            memset(&osettings, 0, sizeof(osettings));
            EEPROMRead((uint32_t *)&osettings, 0, sizeof(osettings));

            memcpy(acsettings.mac, osettings.mac, 6);
            memcpy(acsettings.servername, osettings.servername, SERVERNAMELEN);
            memcpy(acsettings.syslogserver, osettings.syslogserver, SERVERNAMELEN);
            memcpy(acsettings.toolname, osettings.toolname, TOOLNAMELEN);
            acsettings.port = osettings.port;
            acsettings.nodeid = osettings.nodeid;
            acsettings.status = osettings.status;
            acsettings.runtime = osettings.runtime;
            acsettings.minontime = osettings.minontime;
            acsettings.sdcache = osettings.sdcache;
            acsettings.netverbose = osettings.netverbose;
            acsettings.toolonpin_activehigh = osettings.toolonpin_activehigh;
            acsettings.toolrunpin_activehigh = osettings.toolrunpin_activehigh;
            memcpy(acsettings.secret, osettings.secret, KEYLEN+1);

            strncpy(acsettings.mqtt_server, "mqtt.lan.london.hackspace.org.uk", SERVERNAMELEN);
            acsettings.mqtt_port = 1883;
            strncpy(acsettings.mqtt_topic_base, "/door/999", MQTT_TOPIC_LEN);
            acsettings.door_keep_open_ms = 1500;

            // London Hackspace has now migrated all listeners to MQTT. Default changed
            // Keeping old broadcast announcer code for now in case any other
            // hackspaces are using our code and still use UDP broadcast listeners
            
            acsettings.announce_mode = 2;

            // for future use
            acsettings.doorbell_mode = 0;

            // use persistent maintainer cache?
            acsettings.maintainer_cache = 0;

            acsettings.valid = ACSETTINGSVALID;
            set_settings(acsettings);
          }
          break;
      case ACSETTINGS45:
          // upgrade the settings and add defaults for new settings.
          Serial.println("Old settings (45) found, upgrading");
          {
            settings45 osettings;
            memset(&osettings, 0, sizeof(osettings));
            EEPROMRead((uint32_t *)&osettings, 0, sizeof(osettings));

            memcpy(acsettings.mac, osettings.mac, 6);
            memcpy(acsettings.servername, osettings.servername, SERVERNAMELEN);
            memcpy(acsettings.syslogserver, osettings.syslogserver, SERVERNAMELEN);
            memcpy(acsettings.toolname, osettings.toolname, TOOLNAMELEN);
            acsettings.port = osettings.port;
            acsettings.nodeid = osettings.nodeid;
            acsettings.status = osettings.status;
            acsettings.runtime = osettings.runtime;
            acsettings.minontime = osettings.minontime;
            acsettings.sdcache = osettings.sdcache;
            acsettings.netverbose = osettings.netverbose;
            acsettings.toolonpin_activehigh = osettings.toolonpin_activehigh;
            acsettings.toolrunpin_activehigh = osettings.toolrunpin_activehigh;
            memcpy(acsettings.secret, osettings.secret, KEYLEN+1);

            strncpy(acsettings.mqtt_server, osettings.mqtt_server, SERVERNAMELEN);
            acsettings.mqtt_port = osettings.mqtt_port;
            strncpy(acsettings.mqtt_topic_base, osettings.mqtt_topic_base, MQTT_TOPIC_LEN);
            acsettings.door_keep_open_ms = osettings.door_keep_open_ms;
            acsettings.announce_mode = osettings.announce_mode;
            acsettings.doorbell_mode = osettings.doorbell_mode;

            // use persistent maintainer cache?
            acsettings.maintainer_cache = 0;

            acsettings.valid = ACSETTINGSVALID;
            set_settings(acsettings);
          }
          break;
      case ACSETTINGS46:
          // upgrade the settings and add defaults for new settings.
          Serial.println("Old settings (46) found, upgrading");
          {
            settings46 osettings;
            memset(&osettings, 0, sizeof(osettings));
            EEPROMRead((uint32_t *)&osettings, 0, sizeof(osettings));

            memcpy(acsettings.mac, osettings.mac, 6);
            memcpy(acsettings.servername, osettings.servername, SERVERNAMELEN);
            memcpy(acsettings.syslogserver, osettings.syslogserver, SERVERNAMELEN);
            memcpy(acsettings.toolname, osettings.toolname, TOOLNAMELEN);
            acsettings.port = osettings.port;
            acsettings.nodeid = osettings.nodeid;
            acsettings.status = osettings.status;
            acsettings.runtime = osettings.runtime;
            acsettings.minontime = osettings.minontime;
            acsettings.sdcache = osettings.sdcache;
            acsettings.netverbose = osettings.netverbose;
            acsettings.toolonpin_activehigh = osettings.toolonpin_activehigh;
            acsettings.maintainer_cache = osettings.maintainer_cache;
            acsettings.toolrunpin_activehigh = osettings.toolrunpin_activehigh;
            memcpy(acsettings.secret, osettings.secret, KEYLEN+1);

            strncpy(acsettings.mqtt_server, osettings.mqtt_server, SERVERNAMELEN);
            acsettings.mqtt_port = osettings.mqtt_port;
            strncpy(acsettings.mqtt_topic_base, osettings.mqtt_topic_base, MQTT_TOPIC_LEN);
            acsettings.door_keep_open_ms = osettings.door_keep_open_ms;
            acsettings.announce_mode = osettings.announce_mode;
            acsettings.doorbell_mode = osettings.doorbell_mode;

            // use persistent maintainer cache?
            acsettings.invert_colours = 1;

            acsettings.valid = ACSETTINGSVALID;
            set_settings(acsettings);
          }
          break;
    default:
      Serial.println("Settings not valid, using defaults");
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
      strncpy(acsettings.toolname, "ACNode", TOOLNAMELEN);
      acsettings.nodeid = -1;
      acsettings.port = 1234;
      acsettings.status = 0;
      acsettings.runtime = 0;
      acsettings.sdcache = 1;
      acsettings.minontime = 5;
      acsettings.netverbose = 1;
      acsettings.toolonpin_activehigh = 1;
      // the laser signal is inverted
      acsettings.toolrunpin_activehigh = 0;

      // default to on for historic reasons
      acsettings.invert_colours = 1;

      memset(acsettings.secret, 0, KEYLEN+1);
      acsettings.role = 0;
      acsettings.announce_port = 0;

      strncpy(acsettings.mqtt_server, "mqtt.lan.london.hackspace.org.uk", SERVERNAMELEN);
      acsettings.mqtt_port = 1883;
      strncpy(acsettings.mqtt_topic_base, "/door/999", MQTT_TOPIC_LEN);
      acsettings.door_keep_open_ms = 1500;
      // For now, default to broadcast
      acsettings.announce_mode = 1;

      // for future use
      acsettings.doorbell_mode = 0;

      // use persistent maintainer cache?
      acsettings.maintainer_cache = 0;

      // save the settings since it's a new board.
      acsettings.valid = ACSETTINGSVALID;
      set_settings(acsettings);
  }

  return acsettings;
}

int set_settings(settings acsettings) {
  int ret;

  // hope they are actually valid!
  if (acsettings.valid != ACSETTINGSVALID) {
    acsettings.valid = ACSETTINGSVALID;
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

void print_buildinfo() {
  Serial.print("Revision: ");
  Serial.println(STRINGIFY(GIT_REVISION));
  Serial.print("Built on ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);
}
