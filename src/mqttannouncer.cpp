#include "announcer.h"
#include "acnode.h"
#include "mqttannouncer.h"
#include "version.h"

enum msgType {
  MSG_ANNOUNCE,
  MSG_STATUS,
  MSG_DOORBELL,
};

MQTTAnnouncer::MQTTAnnouncer(char* server, uint16_t port, const char* topic_base) :
  server(server),
  port(port),
  mqttClient(network,2000), // keep the timeout short - it'll either work or fail in this time
  topic_base(topic_base)
{

}

void MQTTAnnouncer::RFID(char *cardId) {
  aJsonObject* root = aJson.createObject();
  aJson.addStringToObject(root, "Type", "RFID");
  aJson.addStringToObject(root, "Card", cardId);
  sendMessage(root, MSG_ANNOUNCE);
  aJson.deleteItem(root);
}

void MQTTAnnouncer::START() {
  Serial.println("Connecting to MQTT server...");
  // this connection can take a while if the server doesn't exist
  wdog.feed();
  if(network.connect(server, port) <= 0) {
    Serial.print("Failed to connect to MQTT server ");
    Serial.println(server);
  } else {
    wdog.feed();
    Serial.println("Starting MQTT session");
    mqttClient.connect();
    aJsonObject* root = aJson.createObject();
    aJson.addStringToObject(root, "Type", "START");
    aJson.addStringToObject(root, "Version", ACVERSION);
    if(wdog.was_reset())
    {
      aJson.addStringToObject(root, "Cause", "Watchdog");
    }
    sendMessage(root, MSG_STATUS);
  }
}

void MQTTAnnouncer::sendMessage(aJsonObject* object, int type)
{
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

  Serial.print("Publishing MQTT message to ");
  Serial.println(topic);
  aJson.addStringToObject(object, "Source", "ACNode");
  char* message = aJson.print(object);
  wdog.feed();
  mqttClient.publish(topic, (void*)message, strlen(message));
}

void MQTTAnnouncer::BELL() {
  aJsonObject* root = aJson.createObject();
  aJson.addStringToObject(root, "Type", "BELL");
  aJson.addStringToObject(root, "Message", "RINGGGGGG!!!!!!");
  sendMessage(root, MSG_DOORBELL);
  aJson.deleteItem(root);
}
