#include "nocache.h"

NoCache::NoCache() { }

void NoCache::begin() {
  
}

Card NoCache::get(const Card u) {
  warn();
  return Card();
}

Card NoCache::get(size_t n)
{
  return Card();
}

size_t NoCache::count()
{
  return 0;
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
