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
    void solid(Colour &c, unsigned long lockon_period = 0);
    void flashing(Colour &c, bool fast_mode, unsigned long lockon_period = 0);
    void flashing(Colour &c, Colour &d, bool fast_mode, unsigned long lockon_period = 0);
    void rainbow();
    void off();
  protected:
    void set_lockon_period(unsigned long lockon_period);
    enum FlashingMode {
      none,
      slow,
      fast
    };
    
    struct Settings {
      Settings()
       : colour1(BLACK), colour2(BLACK), flashing_mode(none), is_rainbow(false) 
       {}
      Settings(Colour colour1, Colour colour2 = BLACK, FlashingMode flashing_mode = none)
       : colour1(colour1), colour2(colour2), flashing_mode(flashing_mode), is_rainbow(false)
       {}
      bool operator!=(const Settings& other) const {
        return colour1.r != other.colour1.r || colour1.g != other.colour1.g || colour1.b != other.colour1.b ||
               colour2.r != other.colour2.r || colour2.g != other.colour2.g || colour2.b != other.colour2.b ||
               flashing_mode != other.flashing_mode || is_rainbow != other.is_rainbow;
      }
      Colour colour1;
      Colour colour2;
      FlashingMode flashing_mode;
      bool is_rainbow;    
    }; 

  private:
    void light_up(Colour &c);
    Settings current_settings;
    Settings next_settings;
    bool flashing_on;
    int _r;
    int _g;
    int _b;

    bool invert;
    unsigned long last_toggled_time;

    unsigned long lockon_period;
    unsigned long lockon_start;
};

extern RGB rgb;
#endif
