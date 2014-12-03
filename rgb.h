#ifndef _RGB_H_
#define _RGB_H_

#include <Energia.h>

class RGB
{
  public:
    RGB(int pin_r, int pin_g, int pin_b);
    void begin();
    void red();
    void green();
    void blue();
    void yellow();
    void orange();
    void white();
    void off();
    void pretty();
  private:
    int _r;
    int _g;
    int _b;
    enum Colour {RED, GREEN, BLUE, YELLOW, ORANGE, WHITE, OFF};
    Colour colour;
};

#endif

