#ifndef _TOOL_H_
#define _TOOL_H_

#include <Energia.h>
#include "user.h"


class Tool
{
  public:
    Tool(int pin);
    void on(user user);
    void off(user user);
    void begin();
  private:
    int _toolpin;
    boolean _toolon;
    unsigned long _start;
};

#endif

