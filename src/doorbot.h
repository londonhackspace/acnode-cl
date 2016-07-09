#ifndef _DOORBOT_H_
#define _DOORBOT_H_

#include "door.h"
#include "announcer.h"
#include "card.h"
#include "network.h"
#include "watchdog.h"
#include "rgb.h"
#include "every.h"
#include "PN532.h"

#define ACCESS_DENIED_LED_ON_TIME 2000

class Doorbot {
  public:
    Doorbot(Door &, Watchdog &, PN532 &, RGB &l);
    void enableAnnouncer(uint16_t port);
    void run();
  private:
    void cardPresent(void (Doorbot::*handler) (Card c));
    void handleCardPresent(Card c);
    void denyAccess();
    void grantAccess();
    void announceCard(Card c);
    Door& door;
    Watchdog &wdog;
    PN532 &nfc;
    RGB &led;
    Announcer *announcer;
    Every *every_card;
    Card lastScanned;
    unsigned long lastScannedTime;
};

#endif
