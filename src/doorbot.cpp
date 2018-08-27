#include "doorbot.h"

Doorbot::Doorbot(Door &d, Watchdog &w, PN532 &n, RGB &l) :
  Role(n),
  door(d),
  wdog(w),
  led(l)
  {
    this->announcer = NULL;
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
  announcer->run();
};

void Doorbot::handleCardPresent(Card c) {
  int status;
  Card cached = cache->get(c);
  if (cached.compare_uid(c) && cached.is_valid()) {
    status = 1;
  } else {
    status = networking::querycard(c);
  }
  announceCard(c);

  if(status >= 0) {
    cache->set(c);
    grantAccess();
  } else if(status == -1) {
    denyAccess();
  } else {
    networkingError();
  }
}

void Doorbot::announceCard(Card c) {
  if(!announcer)
    return;
  // Debouncing.
  if (this->lastScanned != c || millis() - this->lastScannedTime > 5000) {
    char buffer[14];
    c.str(buffer);
    this->announcer->RFID(buffer);
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
