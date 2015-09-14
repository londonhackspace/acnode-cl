#include "announcer.h"

// hints for the linker

EthernetUDP Announcer::_udp;
IPAddress Announcer::_host;
int Announcer::_port;

Announcer::Announcer(int port) {
  _host = IPAddress(255, 255, 255, 255);
  _port = port;
  _udp.begin(8888);
}

void Announcer::RFID(user *user) {
  char uid[14];
  memset(uid, 0x0, 14);
  uid_str(uid, user);
  
  char message[6 + 14];
  memset(message, 0x0, 6 + 14);
  sprintf(message, "RFID\n%s\n", uid);

  _udp.beginPacket(_host, _port);
  _udp.write(message);
  _udp.endPacket();
}

void Announcer::START() {
  _udp.beginPacket(_host, _port);
  _udp.write("START\n\n");
  _udp.endPacket();
}

void Announcer::BELL() {
  _udp.beginPacket(_host, _port);
  _udp.write("BELL\n\n");
  _udp.endPacket();
}
