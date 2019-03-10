#include "doorbot_ac.h"

DoorbotWithAccessControl::DoorbotWithAccessControl(Door &d, Watchdog &w, PN532 &n, RGB &l, int button_pin, int door_release_pin) : Doorbot(d, w, n, l, button_pin, door_release_pin) { }

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
  int poll = button.poll();
  switch (poll) {
    case SHORT_PRESS:
    case LONG_PRESS:
      announcer->BELL();
      Serial.println("BING BONG ");
        led.solid(ORANGE);
        delay(ACCESS_DENIED_LED_ON_TIME/2);
        led.solid(BLUE);
        delay(ACCESS_DENIED_LED_ON_TIME/2);
        led.solid(ORANGE);
      break;
  }
  int poll_release = door_release_button.poll();
  switch (poll_release) {
    case SHORT_PRESS:
    case LONG_PRESS:
    announcer->EXIT();
    Serial.println("Door release");
    grantAccess();
  }
  announcer->run();
}

void DoorbotWithAccessControl::handleCardPresent(Card c) {
  int status;
  Card cached = cache->get(c);
  if (cached.compare_uid(c) && cached.is_valid()) {
    status = 1;
  } else if(lwIPLinkActive()) {
    led.solid(MAUVE);
    status = networking::querycard(c);
    switch (status) {
        case 2:
          c.set_maintainer(true);
        case 1:
          c.set_user(true);
    }
  } else {
    Serial.println("No network link - not querying server");
    status = -127;
  }

  

  if(status > 0) {
    cache->set(c);
    grantAccess();
    announceCard(c, 1);
  } else if(status == -1) {
    announceCard(c, 0);
    denyAccess();
  } else {
    networkingError();
    announceCard(c, -1);
  }
}
