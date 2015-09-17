#include <Energia.h>
#include "card.h"
#include "utils.h"
#include "cache.h"
#include "acnode.h"

Card::Card(const uint8_t *uid, boolean uidlen, boolean status, boolean maintainer) {
  _status = status;
  _maintainer = maintainer;
  _uidlen = uidlen;
  memcpy(_uid, uid, uidlen ? 7 : 4);
  _valid = true;
}

/*
Card::Card(struct user *u) {
  _status = u->status;
  _maintainer = u->maintainer;
  _uidlen = u->uidlen;
  memcpy(_uid, u->uid, u->uidlen ? 7 : 4);
}
*/

Card::Card() {
  _status = false;
  _maintainer = false;
  _uidlen = 0;
  memset(_uid, '\0', 7);
  _valid = false;
}

// compare everything
boolean Card::operator==(const Card& other) {
  if (!compare_uid(other)) {
    return false;
  }
  if (_maintainer != other._maintainer) {
    return false;
  }
  if (_status != other._status) {
    return false;
  }
//  if (invalid != other->invalid) {
//    return false;
//  }
  return true;
}

// just compare the uid
boolean Card::compare_uid(const Card& other) const {
  if (_uidlen != other._uidlen) {
    return false;
  }
  if (memcmp(_uid, other._uid, _uidlen ? 7 : 4) != 0) {
    return false;
  }
  return true;
}

// can this card use the tool?
boolean Card::is_user() const {
  if (!_valid) {
    return false;
  }
  if (_status || _maintainer) {
    return true;
  }
  return false;
}

// are they a maintainer?
boolean Card::is_maintainer() const {
  if (!_valid) {
    return false;
  }
  return _maintainer;
}

// set maintainer status
void Card::set_maintainer(boolean m) {
  _maintainer = m;
  _valid = true;
}

// set user status
void Card::set_user(boolean u) {
  _status = u;
  _valid = true;
}

// get the length of the uid
int Card::get_longuid() const {
  return (_uidlen ? 7 : 4);
}

// fill *uid with the uid
void Card::get_uid(uint8_t *uid) const {
  int i;
  
  for (i =0 ; i < (_uidlen ? 7 : 4); i++) {
    uid[i] = _uid[i];
  }
}

boolean Card::is_valid(void) const {
  return _valid;
}

void Card::set_valid(boolean v) {
  _valid = v;
  if (!_valid) {
    _status = false;
    _maintainer = false;
  }
}

void Card::dump(void) const {
  Serial.print("UID: ");
  if (_uidlen) {
    dumpHex(_uid, 7);
  } else {
    dumpHex(_uid, 4);
    Serial.print("      ");
  }
  Serial.print(" Maintainer:");
  Serial.print(_maintainer);
  Serial.print(" valid:");
  Serial.print(_valid);
  Serial.print(" Status:");
  Serial.println(_status);
}

// the string pointer needs to have at 9 or 15 bytes of space
void Card::str(char *str) {
  int len;

  len = _uidlen ? 7 : 4;

  for(int i = 0; i < len; i++) {
    sprintf(str, "%02X", _uid[i]);
    str[2] = 0;
    str = str + 2;
  }
  str[0] = 0;
}

/*

// returns true if the uid is the same
boolean compare_uid(user *u1, user *u2) {
  if (u1->uidlen != u2->uidlen) {
    return false;
  }
  if (memcmp(u1->uid, u2->uid, u1->uidlen ? 7 : 4) != 0) {
    return false;
  }
  return true;
}

// returns true if the users are the same
boolean compare_user(user *u1, user *u2) {
  if (u1->uidlen != u2->uidlen) {
    return false;
  }
  if (memcmp(u1->uid, u2->uid, u1->uidlen ? 7 : 4) != 0) {
    return false;
  }
  if (u1->maintainer != u2->maintainer) {
    return false;
  }
  if (u1->status != u2->status) {
    return false;
  }
  if (u1->invalid != u2->invalid) {
    return false;
  }
  return true;
}

void dump_user(const user * u) {
  Serial.print("UID: ");
  if (u->uidlen) {
    dumpHex(u->uid, 7);
  } else {
    dumpHex(u->uid, 4);
    Serial.print("      ");
  }
  Serial.print(" Maintainer:");
  Serial.print(u->maintainer);
  Serial.print(" Status:");
  Serial.print(u->status);
  Serial.print(" invalid:");
  Serial.print(u->invalid);
  Serial.print(" end:");
  Serial.println(u->end);
}

// assumes that we have room in the string
void uid_str(char *str, user *u) {
  int len;

  len = u->uidlen ? 7 : 4;

  for(int i = 0; i < len; i++) {
    sprintf(str, "%02X", u->uid[i]);
    str[2] = 0;
    str = str + 2;
  }
  str[0] = 0;
}

*/

