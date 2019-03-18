#ifndef _ACNODE_H
#define _ACNODE_H

#include <Ethernet.h>
#include <syslog.h>
#include "card.h"
#include "watchdog.h"
#include "cache.h"
#include "every.h"
#include "rgb.h"
#include "role.h"
#include "tool.h"
#include "microrl.h"
#include "network.h"
#include "menu.h"
#include "announcer.h"

// put these things here so more things can use it.
extern EthernetClient client;
extern Syslog syslog;
extern boolean network;
extern Watchdog wdog;
extern Cache *cache;

// XXX should be elsewhere
// the base address (begining of 7th block)
#define USERBASE (384)


/*
      Serial.print(__FILE__);
      Serial.print(": ");
      Serial.println( __LINE__);

*/

#define TRACE Serial.print(__FILE__); Serial.print(": "); Serial.println( __LINE__);

class ACNode : public Role {
public:
  ACNode(PN532 &, RGB &, Tool &, int button_pin);
  void enableAnnouncer(Announcer* announcer);
  void run();
  void announceCard(Card c, int granted);
protected:
  void feed_incoming_character();
  bool card_present();
  bool card_has_access();
  void activate();
  void deactivate();
  void housekeeping();
  Card readcard();
  bool is_enabled();
private:
  RGB &rgb;
  Tool &tool;
  Menu menu;

  Card card_on_reader;
  unsigned long last_status_checked_at;
  bool enabled;
  Announcer *announcer;
  Card lastScanned;
  unsigned long lastScannedTime;
  unsigned long cardLastSeenTime;
  bool deactivationAnnounced;
};

#endif
