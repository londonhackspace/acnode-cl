#include "rgb.h"

RGB::RGB(int pin_r, int pin_g, int pin_b) : colour1(BLACK), colour2(BLACK) {
  _r = pin_r;
  _g = pin_g;
  _b = pin_b;
  flashing_mode = none;
  flashing_on = false;
  is_rainbow = false;
  invert = false;
}

void RGB::begin(bool invert) {
  // Move this into the constructor?
  pinMode(_r, OUTPUT);
  pinMode(_g, OUTPUT);
  pinMode(_b, OUTPUT);
  this->invert = invert;
  run();
}

void RGB::run() {
  if (is_rainbow) {
    int index = (millis() / 300) % 6;
    light_up(RAINBOW[index]);
    return;
  }
  if (flashing_mode != none) {
    unsigned long now = millis();
    unsigned long flash_interval = flashing_mode == slow ? SLOW_FLASH_INTERVAL : FAST_FLASH_INTERVAL;
    if (now - last_toggled_time > flash_interval) {
      flashing_on ? light_up(colour1) : light_up(colour2);
      flashing_on = !flashing_on;
      last_toggled_time = now;
    }
  } else {
    light_up(colour1);
  }
}

void RGB::solid(Colour &c) {
  flashing_mode = none;
  is_rainbow = false;
  colour1 = c;
  run();
}

void RGB::flashing(Colour &c, bool fast_mode) {
  flashing(c, BLACK, fast_mode);
}

void RGB::flashing(Colour &c, Colour &d, bool fast_mode) {
  flashing_mode = fast_mode ? fast : slow;
  is_rainbow = false;
  colour1 = c;
  colour2 = d;
  run();
}

void RGB::rainbow() {
  is_rainbow = true;
  run();
}

void RGB::off() {
  solid(BLACK);
}

void RGB::light_up(Colour &c) {
  uint8_t localR = c.r;
  uint8_t localG = c.g;
  uint8_t localB = c.b;
  
  if(invert)
  {
    localR = 255 - localR;
    localG = 255 - localG;
    localB = 255 - localB;
  }

  analogWrite(_r, localR);
  analogWrite(_g, localG);
  analogWrite(_b, localB);
}
