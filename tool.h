#ifndef _TOOL_H_
#define _TOOL_H_

#include <Energia.h>

class Tool
{
  public:
    Tool(int pin);
    void on();
    void off();
    void begin();
  private:
    int _toolpin;
    boolean _toolon;
};

#endif

