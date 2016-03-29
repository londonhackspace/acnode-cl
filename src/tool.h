#ifndef _TOOL_H_
#define _TOOL_H_

#include <Energia.h>
#include "card.h"


class Tool
{
  public:
    Tool(int pin);
    Tool(int pin, int onpin);
    void poll();
    void startrunning();
    void stoprunning();
    void on(Card user);
    void off();
    void begin();
    boolean status(); // true if the tool has been switched on
  private:
    int _toolpin;
    int _toolrunpin;
    boolean _toolon;
    boolean _turnoff;
    unsigned long _start;  // used the runtime as detected by the _toolonpin
    unsigned long _ontime; // used for when we switch the tool on
    Card tool_user;
};

#endif

