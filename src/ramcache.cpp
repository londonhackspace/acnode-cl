#include "ramcache.h"
#include "network.h"
#include "watchdog.h"

extern Watchdog wdog;

CacheEntry::CacheEntry() :
  expires_at(0),
  card()
{
}

CacheEntry::CacheEntry(Card c) :
  card(c)
{
  expires_at = millis() + CACHE_TTL;
}

bool CacheEntry::expired(unsigned long now) {
  return expires_at < now;
}

void CacheEntry::touch(unsigned long now) {
  expires_at = now + CACHE_TTL;
}

void CacheEntry::expire() {
  expires_at = 0;
}

void RAMCache::begin() {
  Serial.println("RAM Cache TTL: " + String(CACHE_TTL));
  Serial.println("RAM Cache memory: " + String(sizeof(CacheEntry) * CACHE_CAPACITY));
  purge();
}

Card RAMCache::get(Card u) {

  unsigned long now = millis();
  for (int i = 0; i < CACHE_CAPACITY; i++) {
    CacheEntry *entry = &entries[i];
    if (!entry->expired(now)) {
      if (entry->card.compare_uid(u)) {
        entry->touch(now);
        //DEBUG("RAM Cache lookup: HIT")
        return entry->card;
      }
    }
  }
  DEBUG("RAM Cache lookup: MISS")
  return Card();
}

Card RAMCache::get(size_t n) {
  if (n<CACHE_CAPACITY) {
    return entries[n].card;
  }
  return Card();   
}

size_t RAMCache::count() {
  return CACHE_CAPACITY;
}

void RAMCache::set(const Card u) {
  unsigned long now = millis();
  // Card is already in the cache.
  if (get(u).compare_uid(u)) return;

  CacheEntry *slot = NULL;
  for (int i = 0; i < CACHE_CAPACITY; i++) {
    CacheEntry *entry = &entries[i];
    if (entry->expired(now)) {
      slot = entry;
      break;
    }
  }
  // No slots left, find the oldest entry.
  if (slot == NULL) {
    slot = &entries[0];
    for (int i = 1; i < CACHE_CAPACITY; i++) {
      CacheEntry *entry = &entries[i];
      if (entry->expires_at < slot->expires_at) {
        slot = entry;
      }
    }
  }

  *slot = u;
}

void RAMCache::purge() {
  for (int i = 0; i < CACHE_CAPACITY; i++) {
    entries[i].expire();
  }
}

int RAMCache::each(void( *callback)(Card c)) {
  unsigned long now = millis();
  int count = 0;
  for (int i = 0; i < CACHE_CAPACITY; i++) {
    CacheEntry *entry = &entries[i];
    if (!entry->expired(now)) {
      count++;
      callback(entry->card);
    }
  }
  return count;
}

void RAMCache::verify() {
  unsigned long now = millis();
  for (int i = 0; i < CACHE_CAPACITY; i++) {
    CacheEntry *entry = &entries[i];
    if (!entry->expired(now)) {
      wdog.feed();
      int status = networking::querycard(entry->card);
      switch (status) {
        case 2:
          entry->card.set_maintainer(true);
          entry->touch(now);
          break;
        case 1:
          entry->card.set_user(true);
          entry->touch(now);
          break;
        case -1: // Card is not known at all - actually, this is returned for expired members too
          entry->touch(now);
          entry->expire();
          break;
        case -99: // Networking error
          entry->touch(now);
          break;
        default:
          entry->touch(now);
          break;
      }
    }
  }
}
