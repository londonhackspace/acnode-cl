#ifndef _EVERY_H_
#define _EVERY_H_

#include <Energia.h>

class Every
{
  public:
    Every(int period);
    void begin();
    bool check();
  private:
    unsigned long start;
    unsigned long _period;
};

#endif

