#include "sdcache.h"

SDCache::SDCache() {
}

SDCache::SDCache(char *filename) {
  _filename = filename;
}

boolean SDCache::get(uint8_t *key, user *u) {
  if (!SD.exists(_filename)) return false;
  File f = SD.open(_filename, FILE_READ);
  user user_entry;
  bool found = false;
  while (f.available()) {
    f.read(&user_entry, sizeof(struct user));
    if (compare(user_entry.uid, key)) {
      found = true;
      break;
    }
  }

  if (found) memcpy(u, &user_entry, sizeof(struct user));
  f.close();
  return found;
}

void SDCache::set(uint8_t *key, user *u) {
  File f = SD.open(_filename, FILE_APPEND);
  user user_entry;
  while (f.available()) {
    f.read(&user_entry, sizeof(struct user));
    if (compare(user_entry.uid, key)) {
      f.seek(-sizeof(struct user));
      break;
    }
  }
  f.write((uint8_t *)u, sizeof(struct user));
  f.close();
}

void SDCache::purge() {
  SD.remove(_filename);
}

int SDCache::each(void(*callback)(user *u)) {
  File f = SD.open(_filename, FILE_READ);
  int counter = 0;
  user user_entry;
  while (f.available()) {
    f.read(&user_entry, sizeof(struct user));
    callback(&user_entry);
    counter++;
  }
  f.close();
  return counter;
}

boolean SDCache::compare(uint8_t *k1, uint8_t *k2) {
  int i = 0;
  while (i++ < 7) {
    if (k1[i] != k2[i]) return false;
  }
  return true;
}

