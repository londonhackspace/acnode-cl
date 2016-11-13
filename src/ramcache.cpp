#include "ramcache.h"
#include "network.h"

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
  purge();
}

Card RAMCache::get(Card u) {
  unsigned long now = millis();
  for (int i = 0; i < CACHE_CAPACITY; i++) {
    CacheEntry *entry = &entries[i];
    if (!entry->expired(now)) {
      if (entry->card.compare_uid(u)) return entry->card;
    }
  }
  return Card();
}

void RAMCache::set(const Card u) {
  unsigned long now = millis();
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
      int status = networking::querycard(entry->card);
      switch (status) {
        case 2:
          entry->card.set_maintainer(true);
        case 1:
          entry->card.set_user(true);
        default:
          entry->expire();
      }
    }
  }
}
