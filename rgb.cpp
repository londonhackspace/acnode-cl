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
  colour = OFF;
}

void RGB::white() {
  if (colour == WHITE)
    return;
  analogWrite(_r, 64);
  analogWrite(_g, 0);
  analogWrite(_b, 64);
  colour = WHITE;
}

void RGB::red() {
  if (colour == RED)
    return;
  off();
  analogWrite(_r, 0);
  colour = RED;
}

void RGB::green() {
  if (colour == GREEN)
    return;
  off();
  analogWrite(_g, 0);
  colour = GREEN;
}

void RGB::blue() {
  if (colour == BLUE)
    return;
  off();
  analogWrite(_b, 0);
  colour = BLUE;
}

void RGB::yellow() {
  if (colour == YELLOW)
    return;
  off();
  analogWrite(_r, 0);
  analogWrite(_g, 0);
  colour = YELLOW;
}

void RGB::orange() {
  if (colour == ORANGE)
    return;
  off();
  analogWrite(_r, 0);
  analogWrite(_g, 200);
  colour = ORANGE;
}

void RGB::off() {
  if (colour == OFF)
    return;
  analogWrite(_r, 255);
  analogWrite(_g, 255);
  analogWrite(_b, 255);
  colour = OFF;
}

void RGB::set(Colour c) {
  switch (c) {
    case RED:
      red();
      break;
    case GREEN:
      green();
      break;
    case BLUE:
      blue();
      break;
    case YELLOW:
      yellow();
      break;
    case ORANGE:
      orange();
      break;
    case WHITE:
      white();
      break;
    case OFF:
      off();
      break;
  }
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

