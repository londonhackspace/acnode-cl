// This is a simplified, cut down version of the reader firmware tickcounter
// to support the comms protocol
#include "tickcounter.h"

#include <Arduino.h>
#include <limits.h>

unsigned long int tickcounter_get()
{
    return millis();
}

unsigned long int compare_timer_values(unsigned long int a, unsigned long int b)
{
    unsigned long int retval = 0;
    if(b > a)
    {
        retval = ULONG_MAX-b;
        b = 0;
    }
    return retval+(a-b);
}
