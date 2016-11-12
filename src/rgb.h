#ifndef _RGB_H_
#define _RGB_H_

#include <Energia.h>
#include "colour.h"

#define FLASH_INTERVAL 500 //msec

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
    unsigned long last_toggled_time;
    Colour colour;
};

#endif
