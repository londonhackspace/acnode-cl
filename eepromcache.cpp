#include <Energia.h>
#include <stdint.h>
#include <driverlib/eeprom.h>

#include "acnode.h"
#include "network.h"
#include "utils.h"

#include "eepromcache.h"


EEPromCache::EEPromCache() {
}

void EEPromCache::begin() {
  // N.B. settings.cpp actually inits the eeprom
  uint32_t eesize;

  eesize = EEPROMSizeGet();

  Serial.print("EEPROM Size: ");
  Serial.println(eesize);

  Serial.print("Space for cards: ");
  Serial.println(eesize - USERBASE);

  Serial.print("Can store ");
  Serial.print((eesize - USERBASE) / sizeof(user));
  Serial.println(" cards.");
}


Card EEPromCache::get(Card u) {
  int address = find_user(u);

  if (address < 0) {
    return Card();
  }

  user nu;

  EEPROMRead((uint32_t *)&nu, address, sizeof(user));

  return struct_to_card(nu);
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
void EEPromCache::set(const Card u) {
  int address;

  Serial.println("Storeing user: ");
  u.dump();

  // the user might already exists, don't store it more then once...
  address = find_user(u);
  if (address < 0) {
    // might be the end or an invalid slot in the middle somewhere.
    address = find_free();
  }

  Serial.print("Will save at: ");
  Serial.println(address);
  user nu = card_to_struct(u);
  write_user(nu, address);
}

// clear the userdb, this marks the first entry as the end.
void EEPromCache::purge(void) {
  user u;
  memset(&u, 0xff, sizeof(user));
  u.invalid = 1;
  u.end = 1;
  write_user(u, USERBASE);
}

int EEPromCache::each(void( *callback)(Card u)) {
  int address = USERBASE;
  int count = 0;
  user u;

  while (1) {
    EEPROMRead((uint32_t *)&u, address, sizeof(user));
    wdog.feed();

    if (u.end == 1) {
      break;
    }
    if (u.invalid == 0) {
      Card t(u.uid, u.uidlen, u.status, u.maintainer);
      callback(t);
      count++;
    }
    address += sizeof(user);
  }
  return count;
}

// go through the cached users and check them against the acserver
void EEPromCache::verify(void) {
  int address = USERBASE;
  int count = 0;
  user u;

  while (1) {
    EEPROMRead((uint32_t *)&u, address, sizeof(user));

    if (u.end == 1) {
      break;
    }
    if (u.invalid == 0) {
      Card tc(u.uid, u.uidlen, u.status, u.maintainer);
      if (querycard(tc) == 0) {
        Serial.println("User no-longer valid: ");
        tc.dump();
        u.invalid = 1;
        write_user(u, address);
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

// find a user and return it's address
int EEPromCache::find_user(const Card u) {
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

      Card tc(tu.uid, tu.uidlen, tu.status, tu.maintainer);

      if (u.compare_uid(tc)) {
        found = true;
        break;
      }
      address += sizeof(user);
  }

  if (found) {
    return address;
  }
  return -1;
}

void EEPromCache::write_user(const user u, int address) {
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
  dumpHex((uint8_t *)&u, sizeof(user));
  Serial.println();
  ret = EEPROMProgram((uint32_t *)&u, address, sizeof(user));
  Serial.print("written at: ");
  Serial.println(address);

  if (ret != 0) {
    Serial.print("Writeing problem: ");
    Serial.println(ret, HEX);
  }
}

// look through the eeprom to find a free slot.
int EEPromCache::find_free(void) {
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

user EEPromCache::card_to_struct(const Card u) {
  user nu;

  nu.status = u.is_user();
  nu.maintainer = u.is_maintainer();
  if (u.get_longuid()) {
    nu.uidlen = 1;
  } else {
    nu.uidlen = 0;
  }

  u.get_uid(nu.uid);
  return nu;
}

Card EEPromCache::struct_to_card(const user u) {
  Card nc(u.uid, u.uidlen, u.status, u.maintainer);
  return nc;
}

