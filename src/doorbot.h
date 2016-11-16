#ifndef _DOORBOT_H_
#define _DOORBOT_H_

#include "role.h"
#include "door.h"
#include "announcer.h"
#include "card.h"
#include "network.h"
#include "watchdog.h"
#include "rgb.h"
#include "every.h"
#include "PN532.h"
#include "cache.h"

extern Cache *cache;

#define ACCESS_DENIED_LED_ON_TIME 2000

class Doorbot : public Role {
  public:
    Doorbot(Door &, Watchdog &, PN532 &, RGB &l);
    void enableAnnouncer(uint16_t port);
    void run();
  protected:
    void handleCardPresent(Card c);
    void denyAccess();
    void grantAccess();
    void announceCard(Card c);
    Door& door;
    Watchdog &wdog;
    RGB &led;
    Announcer *announcer;
    Card lastScanned;
    unsigned long lastScannedTime;
};

#endif
