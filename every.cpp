#include "every.h"

Every::Every(int period) {
  _period = period;
}

void Every::begin() {
  start = millis();
}

bool Every::check() {
  if ((millis() - start) > _period) {
    start = millis();
    return true;
  }
  return false;
}


