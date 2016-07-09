#include "role.h"

Role::Role(PN532 &n) : nfc(n) {
  this->every_card = new Every(500);
}

void Role::cardPresent(void (Role::*handler) (Card c)) {
  // No need to check that often.
  if (!this->every_card->check()) return;

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
