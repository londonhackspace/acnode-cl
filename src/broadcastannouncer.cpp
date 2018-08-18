#include "announcer.h"
#include "broadcastannouncer.h"

// hints for the linker

BroadcastAnnouncer::BroadcastAnnouncer(int port) {
  _host = IPAddress(255, 255, 255, 255);
  _port = port;
  _udp.begin(8888);
}

void BroadcastAnnouncer::RFID(char *cardId) {
  char message[6 + 14];
  memset(message, 0x0, 6 + 14);
  sprintf(message, "RFID\n%s\n", cardId);

  _udp.beginPacket(_host, _port);
  _udp.write(message);
  _udp.endPacket();
}

void BroadcastAnnouncer::START() {
  _udp.beginPacket(_host, _port);
  _udp.write("START\n\n");
  _udp.endPacket();
}

void BroadcastAnnouncer::BELL() {
  _udp.beginPacket(_host, _port);
  _udp.write("BELL\n\n");
  _udp.endPacket();
}
