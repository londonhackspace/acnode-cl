#include "nocache.h"

NoCache::NoCache() { }

void NoCache::begin() {
  
}

Card NoCache::get(const Card u) {
  warn();
  return Card();
}

void NoCache::set(const Card u) {
  warn();
}

void NoCache::purge() {
  warn();
}

int NoCache::each(void( *callback)(Card c)) {
  warn();
  return 0;
}

void NoCache::verify() {
  warn();
}

void NoCache::warn() {
  Serial.println("Warning: Cache is disabled.");
}
