#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <Energia.h>

#define NO_PRESS     0
#define SHORT_PRESS  1
#define LONG_PRESS   2

class Button
{
  public:
    Button(int pin);
    void begin();
    int poll();
  private:
    int _pin;
    int state;
    unsigned long start_millis;
    unsigned long last_sample;
};

#endif

