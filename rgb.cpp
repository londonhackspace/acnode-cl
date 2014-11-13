#include "rgb.h"

RGB::RGB(int pin_r, int pin_g, int pin_b) {
  _r = pin_r;
  _g = pin_g;
  _b = pin_b;
}
void RGB::begin() {
  pinMode(_r, OUTPUT);
  pinMode(_g, OUTPUT);
  pinMode(_b, OUTPUT);
}

void RGB::white() {
  analogWrite(_r, 64);
  analogWrite(_g, 0);
  analogWrite(_b, 64);
}

void RGB::red() {
  off();
  analogWrite(_r, 0);
}
void RGB::green() {
  off();
  analogWrite(_g, 0);
}
void RGB::blue() {
  off();
  analogWrite(_b, 0);
}

void RGB::yellow() {
  off();
  analogWrite(_r, 0);
  analogWrite(_g, 0);
}
void RGB::orange() {
  off();
  analogWrite(_r, 0);
  analogWrite(_g, 200);
}

void RGB::off() {
  analogWrite(_r, 255);
  analogWrite(_g, 255);
  analogWrite(_b, 255);
}

void RGB::pretty () {
  const int leds[] = {_r, _g, _b};

  for (unsigned int l = 0; l < sizeof(leds) / 4 ; l++) {
    analogWrite(leds[l], 0);
    for (unsigned int i = 0; i < sizeof(leds) / 4 ; i++) {
      if (i != l ) {
        for (int j = 0; j < 255 ; j++) {
          analogWrite(leds[i], j);
          delay(5);
        }
        for (int j = 255; j > 0 ; j--) {
          analogWrite(leds[i], j);
          delay(5);
        }
      }
      delay(50);
    }
  }
}

