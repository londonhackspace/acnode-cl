#include "sdcache.h"
#include "acnode.h"
#include "network.h"

SDCache::SDCache() {
}

SDCache::SDCache(char *filename) {
  _filename = filename;
}

void SDCache::begin() {
}

Card SDCache::get(Card u) {
  if (!SD.exists(_filename)) return Card();

  File f = SD.open(_filename, FILE_READ);
  user user_entry, tu;
  memset(tu.uid, 0, 7);
  u.get_uid(tu.uid);

  bool found = false;
  while (f.available()) {
    f.read(&user_entry, sizeof(struct user));

    if (user_entry.uidlen == tu.uidlen && compare(user_entry.uid, tu.uid)) {
      found = true;
      break;
    }
  }

//  if (found)
//    memcpy(ret, &user_entry, sizeof(struct user));
  f.close();

  if (!found) {
    return Card();
  }

  Card nc(user_entry.uid, user_entry.uidlen, user_entry.status, user_entry.maintainer);

  return nc;
}

void SDCache::set(const Card u) {
  File f = SD.open(_filename, FILE_WRITEREAD);
  user user_entry, nu;
  int spare = -1; // if we pass an invalid entry keep track of where it was so we can save there.
  boolean add_needed = true;

  // writeing starts with the position at the end of the file, so go back
  // to the beginning.
  f.seek(0);

  memset(nu.uid, 0, 7);
  u.get_uid(nu.uid);

  nu.status = u.is_user();
  nu.maintainer = u.is_maintainer();
  nu.uidlen = u.get_longuid();
  nu.invalid = 0;

  while (f.available()) {
    f.read(&user_entry, sizeof(struct user));

    if (!user_entry.invalid) {
      if (user_entry.uidlen == nu.uidlen && compare(user_entry.uid, nu.uid)) {
        // do we need to update the user?
        if (user_entry.maintainer != nu.maintainer || user_entry.status != nu.status) {
          // yes, so update it.
          Serial.println("Updating user in the cache");
          f.seek(f.position() - sizeof(struct user));
          f.write((uint8_t *)&nu, sizeof(struct user));
        }
        // and we don't need to do anything else
        add_needed = false;
        break;
      }
    }
    // if we find an invalid entry keep it for later so we
    // can overwrite it.
    if (user_entry.invalid && spare == -1) {
      spare = f.position();
      spare -= sizeof(struct user);
    }
  }
  if (spare >= 0) {
    f.seek(spare);
  }
  if (add_needed) {
    Serial.print("adding needed, writing at ");
    Serial.println(f.position());
    f.write((uint8_t *)&nu, sizeof(struct user));
  }
  f.close();
}

void SDCache::purge() {
  SD.remove(_filename);
}

int SDCache::each(void(*callback)(Card u)) {
  File f = SD.open(_filename, FILE_READ);
  int counter = 0;
  user u;
  while (f.available()) {
    f.read(&u, sizeof(struct user));
    wdog.feed();
    if (!u.invalid) {
      Card t(u.uid, u.uidlen, u.status, u.maintainer);
      callback(t);
      counter++;
    }
  }
  f.close();
  return counter;
}

boolean SDCache::compare(const uint8_t *k1, const uint8_t *k2) {
  int i;
  for (i = 0; i < 7 ; i++) {
    if (k1[i] != k2[i]) return false;
  }
  return true;
}

void SDCache::verify(void) {
  File f = SD.open(_filename, FILE_WRITEREAD);
  int counter = 0;
  int status;
  boolean update = false;

  user u;

  f.seek(0);

  while (f.available()) {
    f.read(&u, sizeof(struct user));
    update = false;

    wdog.feed();
    if (!u.invalid) {
      Card t(u.uid, u.uidlen, u.status, u.maintainer);
      status = networking::querycard(t);
      if (status == 0) {
        Serial.println("User no-longer valid: ");
        t.dump();
        u.invalid = 1;
        update = true;
      } else if (status == 2 && !u.maintainer) {
        Serial.println("making user a maintainer");
        u.maintainer = 1;
        update = true;
        counter++;
      } else if (status == 1 && u.maintainer) {
        Serial.println("downgrading to a user");
        u.maintainer = 0;
        update = true;
        counter++;
      } else {
        counter++;
      }
      if (update) {
        f.seek(f.position() - sizeof(struct user));
        f.write((uint8_t *)&u, sizeof(struct user));
      }
    }
  }
  f.close();
  Serial.print("currently storing: ");
  Serial.print(counter);
  Serial.println(" users");
}
