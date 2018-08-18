#ifndef _MQTTANNOUNCER_H_
#define _MQTTANNOUNCER_H_

#include "announcer.h"

#include <cstring> // MQTTClient.h should include this but doesn't
#include <MQTTClient.h>
#include <Ethernet.h>
#include <Countdown.h>
#include <EthernetStack.h>
#include <aJSON.h>

// Only one instance is permitted.

class MQTTAnnouncer : public Announcer{
public:
  MQTTAnnouncer(char* server, uint16_t port, const char* topic_base);
  void RFID(char * card) override;
  void START() override;
  void BELL() override;
private:
  void sendMessage(aJsonObject*, int);

  char* server;
  uint16_t port;
  const char* topic_base;

  EthernetStack network;
  MQTT::Client<EthernetStack, Countdown> mqttClient;
};

#endif
