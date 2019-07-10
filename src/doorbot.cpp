#include "doorbot.h"

#include <Ethernet.h>

Doorbot::Doorbot(Door &d, Watchdog &w, PN532 &n, RGB &l, int button_pin, int door_release_pin) :
  Role(n),
  door(d),
  wdog(w),
  led(l),
  button(button_pin),
  door_release_button(door_release_pin)
  {
    Serial.println("Doorbot role starting");
    this->announcer = NULL;
    button.begin();
    door_release_button.begin();
  };

void Doorbot::enableAnnouncer(Announcer* announcer) {
  if (this->announcer) {
    delete this->announcer;
  }
  this->announcer = announcer;
}

void Doorbot::run() {
  wdog.feed();
  door.maybeClose();
  led.run();
  cardPresent((void (Role::*)(Card c))&Doorbot::handleCardPresent);
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
};

void Doorbot::handleCardPresent(Card c) {
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

  if(status >= 0) {
    cache->set(c);
    grantAccess();
    announceCard(c,1);
  } else if(status == -1) {
    announceCard(c,0);
    denyAccess();
  } else {
    networkingError();
    announceCard(c,-1);
  }
}

void Doorbot::announceCard(Card c, int granted) {
  if(!announcer)
    return;
  // Debouncing.
  if (this->lastScanned != c || millis() - this->lastScannedTime > 5000) {
    char buffer[15];
    c.str(buffer);
    this->announcer->RFID(buffer, granted);
    this->lastScanned = c;
    this->lastScannedTime = millis();
  }
}

void Doorbot::denyAccess() {
  led.solid(RED);
  delay(ACCESS_DENIED_LED_ON_TIME);
}

void Doorbot::grantAccess() {
  door.open();
}

void Doorbot::networkingError() {
  // Captain, these lights are blinking out of sequence. What shall we do?
  // Get them to blink in sequence!
  // We have a networking error, so blink RED/BLUE/RED/BLUE to highlight some problem
  led.solid(RED);
  delay(ACCESS_DENIED_LED_ON_TIME/4);
  led.solid(BLUE);
  delay(ACCESS_DENIED_LED_ON_TIME/4);
  led.solid(RED);
  delay(ACCESS_DENIED_LED_ON_TIME/4);
  led.solid(BLUE);
  delay(ACCESS_DENIED_LED_ON_TIME/4);
  led.solid(BLACK);
}
