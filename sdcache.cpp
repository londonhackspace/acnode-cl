#include "sdcache.h"
#include "acnode.h"

SDCache::SDCache() {
}

SDCache::SDCache(char *filename) {
  _filename = filename;
}

void SDCache::begin() {
}

// caller needs to free returned user
Card SDCache::get(Card u) {
  if (!SD.exists(_filename)) return Card();

  File f = SD.open(_filename, FILE_READ);
  user user_entry, tu;
  memset(tu.uid, 0, 7);
  u.get_uid(tu.uid);

  bool found = false;
  while (f.available()) {
    f.read(&user_entry, sizeof(struct user));

    if (compare(user_entry.uid, tu.uid)) {
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
  File f = SD.open(_filename, FILE_APPEND);
  user user_entry, nu;

  memset(nu.uid, 0, 7);
  u.get_uid(nu.uid);

  nu.status = u.is_user();
  nu.maintainer = u.is_maintainer();
  nu.uidlen = u.get_longuid();

  while (f.available()) {
    f.read(&user_entry, sizeof(struct user));
    if (compare(user_entry.uid, nu.uid)) {
      f.seek(-sizeof(struct user));
      break;
    }
  }
  f.write((uint8_t *)&nu, sizeof(struct user));
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
    Card t(u.uid, u.uidlen, u.status, u.maintainer);
    callback(t);
    counter++;
  }
  f.close();
  return counter;
}

boolean SDCache::compare(const uint8_t *k1, const uint8_t *k2) {
  int i = 0;
  while (i++ < 7) {
    if (k1[i] != k2[i]) return false;
  }
  return true;
}

void SDCache::verify(void) {
}

