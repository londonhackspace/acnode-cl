#ifndef _ROLE_H_
#define _ROLE_H_

#include "PN532.h"
#include "card.h"
#include "every.h"

class Role {
public:
  Role(PN532 &);
  virtual void run() = 0;
  void cardPresent(void (Role::*) (Card c));
  PN532 &nfc;
  Every *every_card;
};

#endif
