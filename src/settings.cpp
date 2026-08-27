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

  Serial.print("Turn off delay: ");
  Serial.println(acsettings.off_delay);

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
  settings42 acsettings42;
  settings43 acsettings43;
  settings44 acsettings44;
  settings45 acsettings45;
  settings46 acsettings46;  
  settings47 acsettings47;
  bool upgraded = false;

  switch (acsettings.valid) {
    case ACSETTINGSVALID:
      // nothing to do
      break;
    case ACSETTINGS42:
      // upgrade the settings and add defaults for new settings.
      Serial.println("Old settings (42) found, upgrading");
      {
        memset(&acsettings42, 0, sizeof(settings42));
        upgraded = true;
        EEPROMRead((uint32_t *)&acsettings42, 0, sizeof(acsettings42));

        memcpy(acsettings43.mac, acsettings42.mac, 6);
        memcpy(acsettings43.servername, acsettings42.servername, SERVERNAMELEN);
        memcpy(acsettings43.syslogserver, acsettings42.syslogserver, SERVERNAMELEN);
        memcpy(acsettings43.toolname, acsettings42.toolname, TOOLNAMELEN);
        acsettings43.port = acsettings42.port;
        acsettings43.nodeid = acsettings42.nodeid;
        acsettings43.status = acsettings42.status;
        acsettings43.runtime = acsettings42.runtime;
        acsettings43.minontime = 5;
        acsettings43.sdcache = 0;
        acsettings43.netverbose = 1;
        acsettings43.toolonpin_activehigh = 1;
        // the laser signal is inverted
        acsettings43.toolrunpin_activehigh = 0;
      }
    case ACSETTINGS43:
      // upgrade the settings and add defaults for new settings.
      Serial.println("Old settings (43) found, upgrading");
      {
        if (!upgraded) {
          memset(&acsettings43, 0, sizeof(settings43));
          EEPROMRead((uint32_t *)&acsettings43, 0, sizeof(acsettings43));
          upgraded = true;
        }

        memcpy(acsettings44.mac, acsettings43.mac, 6);
        memcpy(acsettings44.servername, acsettings43.servername, SERVERNAMELEN);
        memcpy(acsettings44.syslogserver, acsettings43.syslogserver, SERVERNAMELEN);
        memcpy(acsettings44.toolname, acsettings43.toolname, TOOLNAMELEN);
        acsettings44.port = acsettings43.port;
        acsettings44.nodeid = acsettings43.nodeid;
        acsettings44.status = acsettings43.status;
        acsettings44.runtime = acsettings43.runtime;
        acsettings44.minontime = acsettings43.minontime;
        acsettings44.sdcache = acsettings43.sdcache;
        acsettings44.netverbose = acsettings43.netverbose;
        acsettings44.toolonpin_activehigh = acsettings43.toolonpin_activehigh;
        acsettings44.toolrunpin_activehigh = acsettings43.toolrunpin_activehigh;

        memset(acsettings44.secret, 0, KEYLEN+1);
      }
      case ACSETTINGS44:
          // upgrade the settings and add defaults for new settings.
          Serial.println("Old settings (44) found, upgrading");
          {
            if (!upgraded) {
              memset(&acsettings44, 0, sizeof(settings44));
              EEPROMRead((uint32_t *)&acsettings44, 0, sizeof(acsettings44));
              upgraded = true;
            }
            memcpy(acsettings45.mac, acsettings44.mac, 6);
            memcpy(acsettings45.servername, acsettings44.servername, SERVERNAMELEN);
            memcpy(acsettings45.syslogserver, acsettings44.syslogserver, SERVERNAMELEN);
            memcpy(acsettings45.toolname, acsettings44.toolname, TOOLNAMELEN);
            acsettings45.port = acsettings44.port;
            acsettings45.nodeid = acsettings44.nodeid;
            acsettings45.status = acsettings44.status;
            acsettings45.runtime = acsettings44.runtime;
            acsettings45.minontime = acsettings44.minontime;
            acsettings45.sdcache = acsettings44.sdcache;
            acsettings45.netverbose = acsettings44.netverbose;
            acsettings45.toolonpin_activehigh = acsettings44.toolonpin_activehigh;
            acsettings45.toolrunpin_activehigh = acsettings44.toolrunpin_activehigh;
            memcpy(acsettings45.secret, acsettings44.secret, KEYLEN+1);

            strncpy(acsettings45.mqtt_server, "mqtt.lan.london.hackspace.org.uk", SERVERNAMELEN);
            acsettings45.mqtt_port = 1883;
            strncpy(acsettings45.mqtt_topic_base, "/door/999", MQTT_TOPIC_LEN);
            acsettings45.door_keep_open_ms = 1500;

            // London Hackspace has now migrated all listeners to MQTT. Default changed
            // Keeping old broadcast announcer code for now in case any other
            // hackspaces are using our code and still use UDP broadcast listeners
            
            acsettings45.announce_mode = 2;

            // for future use
            acsettings45.doorbell_mode = 0;
          }
      case ACSETTINGS45:
          // upgrade the settings and add defaults for new settings.
          Serial.println("Old settings (45) found, upgrading");
          {
            if (!upgraded) {
              memset(&acsettings45, 0, sizeof(settings45));
              EEPROMRead((uint32_t *)&acsettings45, 0, sizeof(acsettings45));
              upgraded = true;
            }

            memcpy(acsettings46.mac, acsettings45.mac, 6);
            memcpy(acsettings46.servername, acsettings45.servername, SERVERNAMELEN);
            memcpy(acsettings46.syslogserver, acsettings45.syslogserver, SERVERNAMELEN);
            memcpy(acsettings46.toolname, acsettings45.toolname, TOOLNAMELEN);
            acsettings46.port = acsettings45.port;
            acsettings46.nodeid = acsettings45.nodeid;
            acsettings46.status = acsettings45.status;
            acsettings46.runtime = acsettings45.runtime;
            acsettings46.minontime = acsettings45.minontime;
            acsettings46.sdcache = acsettings45.sdcache;
            acsettings46.netverbose = acsettings45.netverbose;
            acsettings46.toolonpin_activehigh = acsettings45.toolonpin_activehigh;
            acsettings46.toolrunpin_activehigh = acsettings45.toolrunpin_activehigh;
            memcpy(acsettings46.secret, acsettings45.secret, KEYLEN+1);

            strncpy(acsettings46.mqtt_server, acsettings45.mqtt_server, SERVERNAMELEN);
            acsettings46.mqtt_port = acsettings45.mqtt_port;
            strncpy(acsettings46.mqtt_topic_base, acsettings45.mqtt_topic_base, MQTT_TOPIC_LEN);
            acsettings46.door_keep_open_ms = acsettings45.door_keep_open_ms;
            acsettings46.announce_mode = acsettings45.announce_mode;
            acsettings46.doorbell_mode = acsettings45.doorbell_mode;

            // use persistent maintainer cache?
            acsettings46.maintainer_cache = 0;
          }
      case ACSETTINGS46:
          // upgrade the settings and add defaults for new settings.
          Serial.println("Old settings (46) found, upgrading");
          {
            if (!upgraded) {
              memset(&acsettings46, 0, sizeof(settings46));
              EEPROMRead((uint32_t *)&acsettings46, 0, sizeof(acsettings46));
              upgraded = true;
            }

            memcpy(acsettings47.mac, acsettings46.mac, 6);
            memcpy(acsettings47.servername, acsettings46.servername, SERVERNAMELEN);
            memcpy(acsettings47.syslogserver, acsettings46.syslogserver, SERVERNAMELEN);
            memcpy(acsettings47.toolname, acsettings46.toolname, TOOLNAMELEN);
            acsettings47.port = acsettings46.port;
            acsettings47.nodeid = acsettings46.nodeid;
            acsettings47.status = acsettings46.status;
            acsettings47.runtime = acsettings46.runtime;
            acsettings47.minontime = acsettings46.minontime;
            acsettings47.sdcache = acsettings46.sdcache;
            acsettings47.netverbose = acsettings46.netverbose;
            acsettings47.toolonpin_activehigh = acsettings46.toolonpin_activehigh;
            acsettings47.maintainer_cache = acsettings46.maintainer_cache;
            acsettings47.toolrunpin_activehigh = acsettings46.toolrunpin_activehigh;
            memcpy(acsettings47.secret, acsettings46.secret, KEYLEN+1);

            strncpy(acsettings47.mqtt_server, acsettings46.mqtt_server, SERVERNAMELEN);
            acsettings47.mqtt_port = acsettings46.mqtt_port;
            strncpy(acsettings47.mqtt_topic_base, acsettings46.mqtt_topic_base, MQTT_TOPIC_LEN);
            acsettings47.door_keep_open_ms = acsettings46.door_keep_open_ms;
            acsettings47.announce_mode = acsettings46.announce_mode;
            acsettings47.doorbell_mode = acsettings46.doorbell_mode;

            acsettings47.invert_colours = 1;
          } 
      case ACSETTINGS47:
          // upgrade the settings and add defaults for new settings.
          Serial.println("Old settings (46) found, upgrading");
          {
            if (!upgraded) {
              memset(&acsettings47, 0, sizeof(settings47));
              EEPROMRead((uint32_t *)&acsettings47, 0, sizeof(acsettings47));
              upgraded = true;
            }

            memcpy(acsettings.mac, acsettings47.mac, 6);
            memcpy(acsettings.servername, acsettings47.servername, SERVERNAMELEN);
            memcpy(acsettings.syslogserver, acsettings47.syslogserver, SERVERNAMELEN);
            memcpy(acsettings.toolname, acsettings47.toolname, TOOLNAMELEN);
            acsettings.port = acsettings47.port;
            acsettings.nodeid = acsettings47.nodeid;
            acsettings.status = acsettings47.status;
            acsettings.runtime = acsettings47.runtime;
            acsettings.minontime = acsettings47.minontime;
            acsettings.off_delay = 1;
            acsettings.sdcache = acsettings47.sdcache;
            acsettings.netverbose = acsettings47.netverbose;
            acsettings.toolonpin_activehigh = acsettings47.toolonpin_activehigh;
            acsettings.maintainer_cache = acsettings47.maintainer_cache;
            acsettings.toolrunpin_activehigh = acsettings47.toolrunpin_activehigh;
            memcpy(acsettings.secret, acsettings47.secret, KEYLEN+1);

            strncpy(acsettings.mqtt_server, acsettings47.mqtt_server, SERVERNAMELEN);
            acsettings.mqtt_port = acsettings47.mqtt_port;
            strncpy(acsettings.mqtt_topic_base, acsettings47.mqtt_topic_base, MQTT_TOPIC_LEN);
            acsettings.door_keep_open_ms = acsettings47.door_keep_open_ms;
            acsettings.announce_mode = acsettings47.announce_mode;
            acsettings.doorbell_mode = acsettings47.doorbell_mode;

            acsettings.invert_colours = acsettings47.invert_colours;



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
      acsettings.off_delay = 1;
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
    Serial.print("Writing settings problem: ");
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
