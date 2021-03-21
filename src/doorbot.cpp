#include "doorbot.h"

#include <Ethernet.h>

Doorbot::Doorbot(Door &d, Watchdog &w, CardReader* reader, RGB &l, int button_pin, int door_release_pin) :
  Role(reader),
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
    this->lastDoorbellTime = millis() - (1000*60*3);
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
      if (millis() - this->lastScannedTime > 2000) { // Silence doorbell button for two seconds after a card read
        this->lastDoorbellTime = millis();
        announcer->BELL();
        Serial.println("BING BONG ");
        led.solid(ORANGE);
        delay(ACCESS_DENIED_LED_ON_TIME/2);
        led.solid(BLUE);
        delay(ACCESS_DENIED_LED_ON_TIME/2);
        led.solid(ORANGE);
      } else {
        Serial.println("Doorbell pushed within 2 seconds of card read");
      }
      break;
  }
  int poll_release = door_release_button.poll();
  switch (poll_release) {
    case SHORT_PRESS:
    case LONG_PRESS:
    grantAccess();
    if ((millis() - this->lastDoorbellTime) < (1000*60*3)) {
      // Three minutes. We'll announce a doorbell acknowledgement rather than just an exit
        announcer->EXIT(1);
        Serial.println('Door release with ack');
        this->lastDoorbellTime = millis() - (1000*60*3);
    } else {
          announcer->EXIT(0);
          Serial.print("Door release");
    }
  }
};

void Doorbot::handleCardPresent(Card c) {
  int status;
  Serial.println("Card present");
  c.dump();
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
