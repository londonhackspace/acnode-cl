#include "sdcache.h"

SDCache::SDCache() {
}

SDCache::SDCache(char *filename) {
  _filename = filename;
}

// caller needs to free returned user
user * SDCache::get(user *u) {
  if (!SD.exists(_filename)) return NULL;

  File f = SD.open(_filename, FILE_READ);
  user *user_entry = new user;

  bool found = false;
  while (f.available()) {
    f.read(user_entry, sizeof(struct user));
    if (compare(user_entry->uid, u->uid)) {
      found = true;
      break;
    }
  }

//  if (found)
//    memcpy(ret, &user_entry, sizeof(struct user));
  f.close();
  
  if (!found) {
    return NULL;
  }
  
  return user_entry; 
}

void SDCache::set(const user *u) {
  File f = SD.open(_filename, FILE_APPEND);
  user user_entry;
  while (f.available()) {
    f.read(&user_entry, sizeof(struct user));
    if (compare(user_entry.uid, u->uid)) {
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

boolean SDCache::compare(const uint8_t *k1, const uint8_t *k2) {
  int i = 0;
  while (i++ < 7) {
    if (k1[i] != k2[i]) return false;
  }
  return true;
}

void SDCache::fill(void) {
}

void SDCache::list(void) {
}

void SDCache::verify(void) {
}

