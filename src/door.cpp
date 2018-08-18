#include "door.h"
#include <Energia.h>

#define KEEP_OPEN_MILISECONDS 1500

Door::Door(int pin, int initialState, uint16_t holdTime) {
  this->pin = pin;
  this->initialState = initialState;
  this->openedAt = 0;
  this->holdTime = holdTime;
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
  if (millis() - this->openedAt > holdTime) {
    this->close();
    return true;
  }
  return false;
}
