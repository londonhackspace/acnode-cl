#ifndef _ACNODE_H
#define _ACNODE_H

#include <Ethernet.h>
#include <syslog.h>
#include "settings.h"
#include "user.h"

// put these things here so more things can use it.
extern settings acsettings;
extern EthernetClient client;
extern Syslog syslog;
extern boolean network;

/*
      Serial.print(__FILE__);
      Serial.print(": ");
      Serial.println( __LINE__);

*/

#define TRACE Serial.print(__FILE__); Serial.print(": "); Serial.println( __LINE__);

#endif

