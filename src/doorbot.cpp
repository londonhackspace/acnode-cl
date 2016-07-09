#include "doorbot.h"

Doorbot::Doorbot(Door &d, Watchdog &w, PN532 &n, RGB &l) :
  door(d),
  wdog(w),
  nfc(n),
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
  cardPresent(&Doorbot::handleCardPresent);
  if (door.isOpen()) {
    led.green();
  } else {
    led.blue();
  }
};

void Doorbot::cardPresent(void (Doorbot::*handler) (Card c)) {
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  nfc.powerDown();

  if (!success) return;

  switch (uidLength) {
    case 4:
      (this->*handler)(Card(uid, 0, 0, 0));
      break;
    case 7:
      (this->*handler)(Card(uid, 1, 0, 0));
      break;
    default:
      Serial.print("Odd card length?: ");
      Serial.println(uidLength);
      break;
  }
}

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
  char buffer[14];
  c.str(buffer);
  this->announcer->RFID(buffer);
}

void Doorbot::denyAccess() {
  led.red();
  delay(ACCESS_DENIED_LED_ON_TIME);
}

void Doorbot::grantAccess() {
  door.open();
}
