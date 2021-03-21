#include "role.h"

Role::Role(CardReader* reader) : reader(reader) {
  this->every_card = new Every(500);
}

void Role::cardPresent(void (Role::*handler) (Card c)) {
  // No need to check that often.
  if (!this->every_card->check()) return;

  CardId card = reader->getCard();

  if (!card) return;

  switch (card.length) {
    case 4:
      (this->*handler)(Card(card.uid, 0, 0, 0));
      break;
    case 7:
      (this->*handler)(Card(card.uid, 1, 0, 0));
      break;
    default:
      Serial.print("Odd card length?: ");
      Serial.println(card.length);
      break;
  }
}
