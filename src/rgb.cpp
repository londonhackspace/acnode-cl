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
  color(64, 0, 64);
}

void RGB::red() {
  color(0, 255, 255);
}

void RGB::green() {
  color(255, 0, 255);
}

void RGB::blue() {
  color(255, 255, 0);
}

void RGB::yellow() {
  color(0, 0, 255);
}

void RGB::orange() {
  color(0, 200, 255);
}

void RGB::off() {
  color(255, 255, 255);
}

void RGB::color(byte r, byte g, byte b) {
  analogWrite(_r, r);
  analogWrite(_g, g);
  analogWrite(_b, b);
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
