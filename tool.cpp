#include "tool.h"

Tool::Tool(int pin) {
  _toolpin = pin;
  _toolon = false;
}

void Tool::begin() {
  pinMode(_toolpin, OUTPUT);
}

void Tool::on() {
  if (!_toolon) {
     Serial.println("TOOL ON!!!!");
    // switch tool on here.
    digitalWrite(_toolpin, HIGH);
    // end 
    _toolon = true;
  }
}

void Tool::off() {
  // and switch it off here.
  if (_toolon) {
     Serial.println("TOOL OFF!!!!");
    // switch tool off here
    digitalWrite(_toolpin, LOW);
    // end
    _toolon = false;
  }
}


