#ifndef _ANNOUNCER_H_
#define _ANNOUNCER_H_

#include <EthernetUdp.h>
#include "user.h"

// Only one instance is permitted.

class Announcer {
  public:
  
  static EthernetUDP _udp; 
  static IPAddress _host;
  static int _port;

  Announcer();
  static void RFID(user *);
  static void START();
  static void BELL(); 
};

#endif

