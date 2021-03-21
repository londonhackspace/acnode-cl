#ifndef _ROLE_H_
#define _ROLE_H_

#include "PN532.h"
#include "card.h"
#include "every.h"
#include "CardReader/cardreader.h"

class Role {
public:
  Role(CardReader*);
  virtual void run() = 0;
  void cardPresent(void (Role::*) (Card c));
  CardReader* reader;
  Every *every_card;
};

#endif
