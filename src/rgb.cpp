#include "rgb.h"

RGB::RGB(int pin_r, int pin_g, int pin_b) : current_settings(), next_settings() {
  _r = pin_r;
  _g = pin_g;
  _b = pin_b;
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

void RGB::set_lockon_period(unsigned long lockon_period) {
  if (lockon_period > 0 && current_settings != next_settings) {
    current_settings = next_settings;
    Serial.print("Locking on for ");
    Serial.print(lockon_period);
    Serial.println(" ms");
    this->lockon_period = lockon_period;
    this->lockon_start = millis();
  } else if (this->lockon_period == 0) {
    current_settings = next_settings;
  } 
}

void RGB::run() {
  if (lockon_period > 0) {
    if (millis() - lockon_start > lockon_period) {
      lockon_period = 0;
      current_settings = next_settings;
    }
  }

  if (current_settings.is_rainbow) {
    int index = (millis() / 300) % 6;
    light_up(RAINBOW[index]);
    return;
  }
  if (current_settings.flashing_mode != none) {
    unsigned long now = millis();
    unsigned long flash_interval = current_settings.flashing_mode == slow ? SLOW_FLASH_INTERVAL : FAST_FLASH_INTERVAL;
    if (now - last_toggled_time > flash_interval) {
      flashing_on ? light_up(current_settings.colour1) : light_up(current_settings.colour2);
      flashing_on = !flashing_on;
      last_toggled_time = now;
    }
  } else {
    light_up(current_settings.colour1);
  }
}

void RGB::solid(Colour &c, unsigned long lockon_period) {
  next_settings = Settings(c);
  set_lockon_period(lockon_period);
  run();
}

void RGB::flashing(Colour &c, bool fast_mode, unsigned long lockon_period) {
  flashing(c, BLACK, fast_mode, lockon_period);
}

void RGB::flashing(Colour &c, Colour &d, bool fast_mode, unsigned long lockon_period) {
  next_settings = Settings(c, d, fast_mode ? fast : slow);
  set_lockon_period(lockon_period);
  run();
}

void RGB::rainbow() {
  next_settings.is_rainbow = true;
  set_lockon_period(0);
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

RGB rgb(PM_0, PM_1, PM_2);
