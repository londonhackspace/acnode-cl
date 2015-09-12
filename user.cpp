#include <Energia.h>
#include "user.h"
#include "utils.h"
#include "acnode.h"
#include "network.h"

// look up a uid in the eeprom and return a user struct
// the returned user must be freed by the caller
user *get_user(user *u) {
  int address = find_user(u);

  if (address < 0) {
    return NULL;
  }

  user *nu = new user;
  EEPROMRead((uint32_t *)nu, address, sizeof(user));
  return nu;
}

// find a user and return it's address
int find_user(user *u) {
  int address = USERBASE;
  boolean found = false;
  user tu; // this user

  while (1) {
      EEPROMRead((uint32_t *)&tu, address, sizeof(user));
      if (tu.invalid == 1 && tu.end == 0) {
        // not valid, skip it
        address += sizeof(user);
        continue;
      }

      // we've hit the end
      if (tu.end == 1) {
        break;
      }

      if (tu.uidlen == u->uidlen) {
        if (memcmp(tu.uid, u->uid, u->uidlen ? 7 : 4) == 0) {
          found = true;
          break;
        }
      }
      address += sizeof(user);
  }

  if (found) {
    return address;
  }
  return -1;
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


/*
 * 3 ways we could want to store a user:
 *
 * a) store at a particular address
 * b) store in a user slot that was not valid, but was not the end of the userlist
 * c) on the end of the userlist.
 * for a and b we just store it, for c we need to mark the next slot as the end
 */

// store a user in the eeprom
void store_user(user *u) {
  int address;

  Serial.println("Storeing user: ");
  dump_user(u);
  u->end = 0;

  // the user might already exists, don't store it more then once...
  address = find_user(u);
  if (address < 0) {
    // might be the end or an invalid slot in the middle somewhere.
    address = find_free();
  }

  Serial.print("Will save at: ");
  Serial.println(address);
  write_user(u, address);
}

void write_user(const user *u, int address) {
  user ou;
  int ret;

  EEPROMRead((uint32_t *)&ou, address, sizeof(user));
  if (ou.end == 1) {
    // the last entry
    // so write the end marker to the next slot.
    Serial.println("moved end marker");
    dumpHex((uint8_t *)&ou, sizeof(user));
    Serial.println();
    ret = EEPROMProgram((uint32_t *)&ou, address + sizeof(user), sizeof(user));

    if (ret != 0) {
      Serial.print("Writeing problem: ");
      Serial.println(ret, HEX);
    }
  }
  dumpHex((uint8_t *)u, sizeof(user));
  Serial.println();
  ret = EEPROMProgram((uint32_t *)u, address, sizeof(user));
  Serial.print("written at: ");
  Serial.println(address);

  if (ret != 0) {
    Serial.print("Writeing problem: ");
    Serial.println(ret, HEX);
  }
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

// look through the eeprom to find a free slot.
int find_free(void) {
  unsigned int address = USERBASE;
  unsigned int eesize;

  eesize = EEPROMSizeGet();

  user u;

  while (1) {
      EEPROMRead((uint32_t *)&u, address, sizeof(user));
      if (u.invalid == 1 && u.end == 0) {
        // not valid
        break;
      }
      if (u.end == 1) {
        break;
      }
      address += sizeof(u);

      // the 2nd test here is if sizeof(u) is not a multiple of the space we have
      // available and so there is only partial space at the end of the eeprom.
      if (address >= eesize || (address + sizeof(u)) > eesize ) {
        char msg[64];

        // just overwrite the entry at the beginning if we are full.
        address = USERBASE;

        snprintf(msg, 64, "Have run out of space for cacheing cards!");
        Serial.println(msg);
        syslog.syslog(LOG_CRIT, msg);

        break;
      }
  }
  return address;
}

// fill the card cache with junk users to see how well we cope with runing out of space.
void fill_users(void) {
  int i;
  user u;

  u.uidlen = 0;
  u.status = 1;
  u.invalid = 0;
  u.end = 0;

  for (i = 0 ; i < 1000 ; i++) {
    u.uid[0] = i & 0xff;
    u.uid[1] = (i >> 8) & 0xff;
    store_user(&u);
    wdog.feed();
  }
}

void list_users(void) {
  int address = USERBASE;
  int count = 0;
  user u;
  while (1) {
    EEPROMRead((uint32_t *)&u, address, sizeof(user));

    if (u.end == 1) {
      break; 
    }
    if (u.invalid == 0) {
      dump_user(&u);
      count++;
    }
    address += sizeof(user);
  }
  Serial.print("currently storing: ");
  Serial.print(count);
  Serial.println(" users");
  
/*
  uint8_t data[16];
  address = 0;
  for (address = USERBASE; address < (USERBASE + (64 * 4)) ; address += 16) {
    Serial.print(address);
    Serial.print(": ");
    EEPROMRead((uint32_t *)data, address, 16);
    dumpHex(data, 16);
    Serial.println("");
  }
  */
}

// clear the userdb, this marks the first entry as the end.
void nuke_users(void) {
  user u;
  memset(&u, 0xff, sizeof(user));
  u.invalid = 1;
  u.end = 1;
  write_user(&u, USERBASE);
}

// go through the cached users and check them against the acserver
void verify_users(void) {
  int address = USERBASE;
  int count = 0;
  user u;

  while (1) {
    wdog.feed();
    EEPROMRead((uint32_t *)&u, address, sizeof(user));

    if (u.end == 1) {
      break;
    }
    if (u.invalid == 0) {
      if (querycard(u) == 0) {
        Serial.println("User no-longer valid: ");
        dump_user(&u);
        u.invalid = 1;
        write_user(&u, address);
      } else {
        count++;
      }
    }
    address += sizeof(user);
  }
  Serial.print("currently storing: ");
  Serial.print(count);
  Serial.println(" users");
}

