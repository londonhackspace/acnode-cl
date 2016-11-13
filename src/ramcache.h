#ifndef _RAMCACHE_H
#define _RAMCACHE_H

#include "cache.h"
#include "card.h"

#define CACHE_TTL 8 * 3600 * 1000 // eight hours
#define CACHE_CAPACITY 1500

class CacheEntry {
public:
  CacheEntry();
  CacheEntry(Card c);
  bool expired(unsigned long now);
  void expire();
  void touch(unsigned long now);
  unsigned long expires_at;
  Card card;
};

class RAMCache : public Cache {
public:
  virtual void begin();
  virtual Card get(Card u);
  virtual void set(const Card u);
  virtual void purge(void);
  virtual int each(void( *callback)(Card c));
  virtual void verify(void);
private:
  CacheEntry entries[CACHE_CAPACITY];
};

#endif
