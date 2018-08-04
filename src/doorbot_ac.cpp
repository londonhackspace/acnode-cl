#include "doorbot_ac.h"

DoorbotWithAccessControl::DoorbotWithAccessControl(Door &d, Watchdog &w, PN532 &n, RGB &l) : Doorbot(d, w, n, l) { }

void DoorbotWithAccessControl::run() {
  wdog.feed();
  door.maybeClose();
  led.run();
  cardPresent((void (Role::*)(Card c))&DoorbotWithAccessControl::handleCardPresent);
  if (door.isOpen()) {
    led.solid(GREEN);
  } else {
    led.solid(BLUE);
  }
}

void DoorbotWithAccessControl::handleCardPresent(Card c) {
  int status;
  Card cached = cache->get(c);
  if (cached.compare_uid(c) && cached.is_valid()) {
    status = 1;
  } else {
    status = networking::querycard(c);
  }

  announceCard(c);

  if(status > 0) {
    cache->set(c);
    grantAccess();
  } else if(status == -1) {
    denyAccess();
  } else {
    networkingError();
  }
}
