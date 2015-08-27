#ifndef _DOOR_H_
#define _DOOR_H_

#include <Energia.h>

class Door {
  // There can be only one door attached.

 public:
   static int _pin;
   static unsigned long _opened_at;
   
   Door(int pin);
   static void open();
   static boolean opened();
   static void close();
   static boolean maybe_close();
};

#endif

