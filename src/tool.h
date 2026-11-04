#ifndef _TOOL_H_
#define _TOOL_H_

#include <Energia.h>
#include "card.h"


class Tool
{
  public:
    enum ToolState {
      OFF,
      ON,
      TURNING_OFF
    };
    Tool(int pin);
    Tool(int pin, int onpin);
    void poll();
    void startrunning();
    void stoprunning();
    void on(Card user);
    void off();
    void begin();
    ToolState status(); 
  protected:
    bool can_turn_off(); // true if the tool can be switched off (i.e. minontime and off_delay have been met)

  private:
    int _toolpin;
    int _toolrunpin;
    ToolState _toolstate;
    unsigned long _start;  // used the runtime as detected by the _toolonpin
    unsigned long _ontime; // used for when we switch the tool on
    unsigned long _offtime; // used for when we switch the tool off
    Card tool_user;
};

#endif

