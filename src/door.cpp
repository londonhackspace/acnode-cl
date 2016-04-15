#include "door.h"
#include <Energia.h>

#define KEEP_OPEN_MILISECONDS 1500

Door::Door(int pin, int initialState) {
  this->pin = pin;
  this->initialState = initialState;
  this->openedAt = 0;
  pinMode(this->pin, OUTPUT);
  digitalWrite(this->pin, this->initialState);
}

void Door::open() {
  digitalWrite(this->pin, abs(1 - this->initialState));
  this->openedAt = millis();
}

void Door::close() {
  digitalWrite(this->pin, this->initialState);
  this->openedAt = 0;
}

bool Door::isOpen() {
  return this->openedAt > 0;
}

bool Door::maybeClose() {
  if (millis() - this->openedAt > KEEP_OPEN_MILISECONDS) {
    this->close();
    return true;
  }
  return false;
}
