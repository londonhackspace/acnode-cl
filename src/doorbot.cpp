#include "doorbot.h"

Doorbot::Doorbot(Door &d, Watchdog &w, PN532 &n, RGB &l) :
  Role(n),
  door(d),
  wdog(w),
  led(l)
  {
    this->announcer = NULL;
};

void Doorbot::enableAnnouncer(uint16_t port) {
  if (this->announcer) {
    delete this->announcer;
  }
  this->announcer = new Announcer(port);
  this->announcer->START();
}

void Doorbot::run() {
  wdog.feed();
  door.maybeClose();
  cardPresent((void (Role::*)(Card c))&Doorbot::handleCardPresent);
  if (door.isOpen()) {
    led.green();
  } else {
    led.blue();
  }
};

void Doorbot::handleCardPresent(Card c) {
  int status = querycard(c);
  announceCard(c);

  switch (status) {
    case -1:
        denyAccess();
      break;
    default:
      grantAccess();
  }
}

void Doorbot::announceCard(Card c) {
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
  led.red();
  delay(ACCESS_DENIED_LED_ON_TIME);
}

void Doorbot::grantAccess() {
  door.open();
}
