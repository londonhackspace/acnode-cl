#ifndef _RGB_H_
#define _RGB_H_

#include <Energia.h>
#include "colour.h"
#include "CardReader/remotergb.h"

#define FLASH_INTERVAL 500 //msec

class RGB
{
  public:
    RGB(int pin_r, int pin_g, int pin_b, RemoteRGB* remote);
    void begin();
    void run();
    void solid(Colour &c);
    void flashing(Colour &c);
    void flashing(Colour &c, Colour &d);
    void rainbow();
    void off();

    void setRemote(RemoteRGB* r) { remote = r; }

  private:
    void light_up(Colour &c);
    int _r;
    int _g;
    int _b;
    bool is_rainbow;
    bool is_flashing;
    bool flashing_on;
    unsigned long last_toggled_time;
    Colour colour1;
    Colour colour2;

    RemoteRGB* remote;
    int lastRemoteR;
    int lastRemoteG;
    int lastRemoteB;
};

#endif
