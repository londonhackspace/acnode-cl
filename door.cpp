#include "door.h"

// Hints for the linker
int Door::_pin;
int Door::_npin;
unsigned long Door::_opened_at;

Door::Door(int pin, int npin) {
  _pin = pin;
  _npin = npin;
  _opened_at = 0;
  pinMode(_pin, OUTPUT);
  pinMode(_npin, OUTPUT);
  digitalWrite(_pin, HIGH);
  digitalWrite(_npin, LOW);
}

void Door::open() {
  digitalWrite(_pin, LOW);
  digitalWrite(_npin, HIGH);
  _opened_at = millis();
}

boolean Door::opened() {
  return _opened_at > 0;
}

void Door::close() {
  digitalWrite(_pin, HIGH);
  digitalWrite(_npin, LOW);
  _opened_at = 0;
}

boolean Door::maybe_close() {
  if (millis() - _opened_at > 1500) {
    close();
    return true;
  }
  return false;
}
