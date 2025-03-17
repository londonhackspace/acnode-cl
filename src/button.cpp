#include "button.h"

Button::Button(int pin) {
  _pin = pin;
}

#define WAITING 0
#define START 1

void Button::begin() {
  pinMode(_pin, INPUT_PULLUP);
  Serial.print("Button init on pin ");
  Serial.println(_pin, DEC);
  _state = ButtonState::OFF;
  _change_start = 0;
  _press_start = 0;
}

ButtonEvent Button::poll() {
  bool pin_state = digitalRead(_pin);
  unsigned long time = millis();

  // First debounce

  bool button_changed = false;
  /*Serial.print("Current state ");
  Serial.print(_state == ButtonState::OFF ? "OFF" : "ON");
  Serial.print(" : button ");
  Serial.println(pin_state ? "off" : "on");*/

  if (pin_state == (_state == ButtonState::OFF)) {
    _change_start = 0;
    //Serial.println("no diff");
  } else if (_change_start == 0) {
    _change_start = time;
  } else if (_change_start + BUTTON_DEBOUNCE < time) {
    button_changed = true;
  }

  if (button_changed) {
    Serial.println("Button Changed State");
  }
  // The button has stayed in the same new state for BUTTON_DEBOUNCE ms
  
  // pullup, so on by default.

  if (button_changed) {
    if (_state == ButtonState::OFF) {
      _press_start = _change_start;
      _state = ButtonState::SHORT_PRESS;
      return ButtonEvent::PRESSED;
    }
    if (_state == ButtonState::SHORT_PRESS) {
      _state = ButtonState::OFF;
      return ButtonEvent::SHORT_PRESS;
    }
    _state = ButtonState::OFF;
    return ButtonEvent::NONE;
  }

  if (_state == ButtonState::SHORT_PRESS && (_press_start + BUTTON_LONG_PRESS) < time) {
    _state = ButtonState::LONG_PRESS;
    return ButtonEvent::LONG_PRESS;
  }

  return ButtonEvent::NONE;
}

