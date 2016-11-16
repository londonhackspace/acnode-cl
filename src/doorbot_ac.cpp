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
    status = (cached.is_user() || cached.is_maintainer()) ? 1 : 0;
  } else {
    status = networking::querycard(c);
  }

  announceCard(c);

  switch (status) {
    case -1:
    case 0:
      denyAccess();
      break;
    default:
      cache->set(c);
      grantAccess();
  }
}
