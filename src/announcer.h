#ifndef _ANNOUNCER_H_
#define _ANNOUNCER_H_

#include <EthernetUdp.h>

// Only one instance is permitted.

class Announcer {
  public:

  static EthernetUDP _udp;
  static IPAddress _host;
  static int _port;

  Announcer(int);
  static void RFID(char *);
  static void START();
  static void BELL();
};

#endif
