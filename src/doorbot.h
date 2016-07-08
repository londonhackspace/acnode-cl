#ifndef _DOORBOT_H_
#define _DOORBOT_H_

#include "door.h"
#include "card.h"
#include "network.h"
#include "watchdog.h"
#include "rgb.h"
#include "PN532.h"

#define ACCESS_DENIED_LED_ON_TIME 2000

class Doorbot {
  public:
    Doorbot(Door &, Watchdog &, PN532 &, RGB &l);
    void run();
  private:
    void cardPresent(void (Doorbot::*handler) (Card c));
    void handleCardPresent(Card c);
    void denyAccess();
    void grantAccess();
    Door& door;
    Watchdog &wdog;
    PN532 &nfc;
    RGB &led;
};

#endif
