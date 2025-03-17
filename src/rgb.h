#ifndef _RGB_H_
#define _RGB_H_

#include <Energia.h>
#include "colour.h"

#define SLOW_FLASH_INTERVAL 200 //msec
#define FAST_FLASH_INTERVAL 10 //msec

class RGB
{
  public:
    RGB(int pin_r, int pin_g, int pin_b);
    void begin(bool invert);
    void run();
    void solid(Colour &c);
    void flashing(Colour &c, bool fast_mode = false);
    void flashing(Colour &c, Colour &d, bool fast_mode = false);
    void rainbow();
    void off();
  private:
    void light_up(Colour &c);
    int _r;
    int _g;
    int _b;
    bool is_rainbow;
    enum {
      none,
      slow,
      fast
    } flashing_mode ;
    bool flashing_on;
    bool invert;
    unsigned long last_toggled_time;
    Colour colour1;
    Colour colour2;
};

#endif
