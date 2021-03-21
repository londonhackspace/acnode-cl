#ifndef _DOORBOT_AC_H_
#define _DOORBOT_AC_H_

#include "doorbot.h"

class DoorbotWithAccessControl : public Doorbot {
public:
  DoorbotWithAccessControl(Door &, Watchdog &, CardReader*, RGB &l, int button_pin, int door_release_pin);
  void run();
  void handleCardPresent(Card c);
};

#endif
