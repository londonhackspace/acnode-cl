#ifndef _TOOL_H_
#define _TOOL_H_

#include <Energia.h>
#include "user.h"


class Tool
{
  public:
    Tool(int pin);
    Tool(int pin, int onpin);
    void poll();
    void startrunning();
    void stoprunning();
    void on(user user);
    void off(user user);
    void begin();
  private:
    int _toolpin;
    int _toolonpin;
    boolean _toolon;
    unsigned long _start;
    user tool_user;
};

#endif

