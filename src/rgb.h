#ifndef _RGB_H_
#define _RGB_H_

#include <Energia.h>

class Colour {
public:
  Colour(byte r, byte g, byte b);
  byte r, g, b;
};

#define FLASH_INTERVAL 500 //msec

static Colour RED(0, 255, 255);
static Colour GREEN(255, 0, 255);
static Colour BLUE(255, 255, 0);
static Colour YELLOW(0, 0, 255);
static Colour ORANGE(0, 200, 255);
static Colour WHITE(0, 0, 0);
static Colour BLACK(255, 255, 255);

class RGB
{
  public:
    RGB(int pin_r, int pin_g, int pin_b);
    void begin();
    void run();
    void solid(Colour &c);
    void flashing(Colour &c);
    void off();
  private:
    void light_up(Colour &c);
    int _r;
    int _g;
    int _b;
    bool is_flashing;
    bool flashing_on;
    unsigned long last_called_run;
    Colour &colour;
};

#endif
