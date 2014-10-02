#include <Energia.h>
#include "user.h"
#include "utils.h"

// look up a uid in the eeprom and return a user struct
user *find_user(uint8_t * uid, int len) {
  return NULL;
}

// store a user in the eeprom
void store_user(user *u) {
  Serial.println("Would store a user: ");
  dump_user(u);
}

void dump_user(user * u) {
  Serial.print("UID: ");
  if (u->uidlen) {
    dumpHex(u->uid, 7);
  } else {
    dumpHex(u->uid, 4);
  }
  Serial.print("Maintainer:");
  Serial.println(u->maintainer);
  Serial.print("Status:");
  Serial.println(u->status);
}

// look through the eeprom to find a free slot.
int find_free(void) {
  return 64;
}

void list_users(void) {
  int address = USERBASE;
  int count = 0;
  user u;
  while (1) {
      EEPROMRead((uint32_t *)&u, 0, sizeof(u));
      if (u.valid == 1) {
        // not valid
        break;
      }
      // this is pointless now?
      if (u.end = 1) {
        break; 
      }
      dump_user(&u);
      count++;
  }
  Serial.print("currently storing: ");
  Serial.print(count);
  Serial.println(" users");
}

