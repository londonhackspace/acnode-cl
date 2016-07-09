#include "doorbot_ac.h"

DoorbotWithAccessControl::DoorbotWithAccessControl(Door &d, Watchdog &w, PN532 &n, RGB &l) : Doorbot(d, w, n, l) { }

void DoorbotWithAccessControl::run() {
  wdog.feed();
  door.maybeClose();
  cardPresent((void (Role::*)(Card c))&DoorbotWithAccessControl::handleCardPresent);
  if (door.isOpen()) {
    led.green();
  } else {
    led.blue();
  }
}

void DoorbotWithAccessControl::handleCardPresent(Card c) {
  int status = querycard(c);
  announceCard(c);

  switch (status) {
    case -1:
    case 0:
      denyAccess();
      break;
    default:
      grantAccess();
  }
}
