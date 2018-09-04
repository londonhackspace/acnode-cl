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
  void EXIT() override;

  void run() override;
private:
  void sendMessage(aJsonObject*, int);
  void connect();

  char* server;
  uint16_t port;
  const char* topic_base;
  int lastYield;

  EthernetStack network;
  MQTT::Client<EthernetStack, Countdown> mqttClient;
};

#endif