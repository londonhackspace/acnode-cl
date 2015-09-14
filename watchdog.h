#ifndef _WDOG_H_
#define _WDOG_H_

#include <Energia.h>

class Watchdog
{
  public:
    Watchdog(void);
    Watchdog(uint32_t period);
    void begin();
    void feed();
    void stop();
    void disable();
    void enable();
    boolean was_reset();
  private:
    uint32_t _period;
    boolean _running;
    boolean _was_wdog_reset;
};

#endif

