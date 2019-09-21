#include "announcer.h"
#include "acnode.h"
#include "mqttannouncer.h"
#include "version.h"
#include "utils.h"
#include "settings.h"

enum msgType {
  MSG_ANNOUNCE,
  MSG_STATUS,
  MSG_DOORBELL
};

MQTTAnnouncer::MQTTAnnouncer(char* server, uint16_t port, const char* topic_base) :
  server(server),
  port(port),
  mqttClient(network,2000), // keep the timeout short - it'll either work or fail in this time
  topic_base(topic_base),
  lastYield(0)
{

}

void MQTTAnnouncer::RFID(char *cardId, int granted) {
  aJsonObject* root = aJson.createObject();
  aJson.addStringToObject(root, "Type", "RFID");
  aJson.addStringToObject(root, "Card", cardId);
  aJson.addNumberToObject(root, "Granted", granted);
  sendMessage(root, MSG_ANNOUNCE);
  aJson.deleteItem(root);
}

void MQTTAnnouncer::START() {
  connect();

  if(mqttClient.isConnected()) {
    aJsonObject* root = aJson.createObject();
    aJson.addStringToObject(root, "Type", "START");
    aJson.addStringToObject(root, "Version", ACVERSION);
    aJson.addStringToObject(root, "Git", STRINGIFY(GIT_REVISION));
    aJson.addNumberToObject(root, "SettingsVersion", acsettings.valid);
    aJson.addNumberToObject(root, "EEPROMSettingsVersion", get_settings_version());
    if(wdog.was_reset())
    {
      aJson.addStringToObject(root, "Cause", "Watchdog");
    }
    sendMessage(root, MSG_STATUS);
  }
}

void MQTTAnnouncer::run() {
  if((millis() - lastYield) > 2000) {
    if(lwIPLinkActive()) {
      mqttClient.yield(50);
    } else {
      if(mqttClient.isConnected()) {
        Serial.println("Disconnecting from MQTT server - no link");
        mqttClient.disconnect();
      }
    }
    lastYield = millis();
  }
}

void MQTTAnnouncer::connect() {
  if(!lwIPLinkActive()) {
    Serial.println("Not connecting to MQTT server - no link");
    return;
  }
  // this connection can take a while if the server doesn't exist
  Serial.println("Connecting to MQTT server...");
  wdog.feed();
  if(network.connect(server, port) <= 0) {
    Serial.print("Failed to connect to MQTT server ");
    Serial.print(server);
    Serial.print(" on port ");
    Serial.println(port);
  } else {
    wdog.feed();
    Serial.println("Starting MQTT session");
    mqttClient.connect();
  }
}

void MQTTAnnouncer::sendMessage(aJsonObject* object, int type)
{
  if(!lwIPLinkActive()) {
    Serial.println("Not sending to MQTT server - no link");
    mqttClient.disconnect();
    return;
  }
  // Double the topic base length since we're adding some long strings
  char topic[2*MQTT_TOPIC_LEN] = { 0 };
  switch(type) {
    case MSG_ANNOUNCE: {
      sprintf(topic, "%s/announcements", topic_base);
    } break;
    case MSG_STATUS: {
      sprintf(topic, "%s/status", topic_base);
    } break;
    case MSG_DOORBELL: {
      sprintf(topic, "%s/bell", topic_base);
    } break;
    default: {
      Serial.print("Unknown message type ");
      Serial.println(type);
    }
  }

  if(!mqttClient.isConnected()) {
    connect();
  }
  if(!mqttClient.isConnected()) {
    Serial.println("MQTT Disconnected - Not publishing");
  }
  Serial.print("Publishing MQTT message to ");
  Serial.println(topic);
  aJson.addStringToObject(object, "Source", "ACNode");
  char* message = aJson.print(object);
  wdog.feed();
  if(mqttClient.publish(topic, (void*)message, strlen(message)) != 0)
  {
    // try one more time since sometimes this triggers it to notice
    // the server went missing
    Serial.println("Failed to send MQTT message - trying once more");
    if(!mqttClient.isConnected()) {
      connect();
    }
    if(!mqttClient.isConnected()) {
      Serial.println("MQTT Disconnected - Not publishing");
    }
    else
    {
      if(mqttClient.publish(topic, (void*)message, strlen(message)) != 0)
      {
        Serial.println("MQTT Publish failed - Giving up.");
      }
    }
  }
  free(message);
}

void MQTTAnnouncer::BELL() {
  aJsonObject* root = aJson.createObject();
  aJson.addStringToObject(root, "Type", "BELL");
  aJson.addStringToObject(root, "Message", "RINGGGGGG!!!!!!");
  sendMessage(root, MSG_DOORBELL);
  aJson.deleteItem(root);
}

void MQTTAnnouncer::EXIT(int doorbellack) {
  aJsonObject* root = aJson.createObject();
  aJson.addStringToObject(root, "Type", "EXIT");
  if (doorbellack == 1) {
    aJson.addStringToObject(root, "Message", "Doorbell acknowledged");
  } else {
    aJson.addStringToObject(root, "Message", "Door opened by exit button");
  }
  sendMessage(root, MSG_ANNOUNCE);
  aJson.deleteItem(root);
}

void MQTTAnnouncer::WEDGED() {
  aJsonObject* root = aJson.createObject();
  aJson.addStringToObject(root, "Type", "WEDGED");
  aJson.addStringToObject(root, "Message", "Door detected as still open");
  sendMessage(root, MSG_ANNOUNCE);
  aJson.deleteItem(root);
}

void MQTTAnnouncer::ALIVE() {
  aJsonObject* root = aJson.createObject();
  aJson.addStringToObject(root, "Type", "ALIVE");
  aJson.addStringToObject(root, "Message", "I'm not dead yet");
  
  aJsonObject* mem = aJson.createObject();
  aJson.addNumberToObject(mem, "heap_free", (int)get_remaining_heap_mem());
  aJson.addNumberToObject(mem, "heap_used", (int)get_used_heap_mem());

  aJson.addItemToObject(root, "mem", mem);
  sendMessage(root, MSG_STATUS);
  aJson.deleteItem(root);
}

void MQTTAnnouncer::TOOL_DEACTIVATE() {
  aJsonObject* root = aJson.createObject();
  aJson.addStringToObject(root, "Type", "DEACTIVATED");
  aJson.addStringToObject(root, "Message", "Tool deactivated");
  sendMessage(root, MSG_STATUS);
  aJson.deleteItem(root);
}