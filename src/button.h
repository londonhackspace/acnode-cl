#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <Energia.h>

#define BUTTON_DEBOUNCE 20 // ms
#define BUTTON_LONG_PRESS 1000 // ms

enum class ButtonEvent {
  NONE,
  PRESSED,
  SHORT_PRESS,
  LONG_PRESS
};

enum class ButtonState {
  OFF,
  SHORT_PRESS,
  LONG_PRESS
};

class Button
{
  public:
    Button(int pin);
    void begin();
    ButtonEvent poll();
  private:
    int _pin;
    ButtonState _state;
    unsigned long _change_start;
    unsigned long _press_start;
};

#endif

