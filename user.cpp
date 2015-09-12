#include <Energia.h>
#include "user.h"
#include "utils.h"
#include "sdcache.h"

SDCache cache("CACHE");

// look up a uid in the eeprom and return a user struct
// the returned user must be freed by the caller
user *get_user(user *u) {
  user *found = new user;
  if (cache.get(u->uid, found)) {
    return found;
  } else {
    free(found);
    return NULL;
  }
}

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

void store_user(user *u) {
    cache.set(u->uid, u);
}

void dump_user(user * u) {
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

void list_users_callback(user *u) {
  dump_user(u);
}

void list_users(void) {
  int count = cache.each(list_users_callback);
  
  Serial.print("currently storing: ");
  Serial.print(count);
  Serial.println(" users");

}

// clear the userdb, this marks the first entry as the end.
void nuke_users(void) {
  cache.purge();
}

