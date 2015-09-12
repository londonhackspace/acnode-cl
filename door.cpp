#include "door.h"

// Hints for the linker
int Door::_pin;
unsigned long Door::_opened_at;

Door::Door(int pin) {
  _pin = pin;
  _opened_at = 0;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
}

void Door::open() {
  digitalWrite(_pin, LOW);
  _opened_at = millis();
}

boolean Door::opened() {
  return _opened_at > 0;
}

void Door::close() {
  digitalWrite(_pin, HIGH);
  _opened_at = 0;
}

boolean Door::maybe_close() {
  if (millis() - _opened_at > 1500) {
    close();
    return true;
  }
  return false;
}
