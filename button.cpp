#include "button.h"

Button::Button(int pin) {
  _pin = pin;
}

#define WAITING 0
#define START 1

void Button::begin() {
  pinMode(_pin, INPUT_PULLUP);
  state = WAITING;
}

int Button::poll() {
  // pullup, so on by default.
  if (digitalRead(_pin) == 0) {
    if (state == WAITING) {
      // first time we've noticed this press, so keep track of when it happened
      state = START;
      start_millis = millis();
    }
  } else {
    // button is up
    if (state == START) {
      // we are waiting to see what happens with the button, it's just gone
      // back up, so lets see how long it took
      state = WAITING;
//      Serial.println(millis() - start_millis);

      // 600 here seems to be ok.
      // short presses are 120-180 ms or so.
      if ((millis() - start_millis) < 600) {
        return SHORT_PRESS;
      } else {
        return LONG_PRESS;
      }

    }
  }
    
  return NO_PRESS;
}

