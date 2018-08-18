#ifndef _DOOR_H_
#define _DOOR_H_

#include <stdint.h>

class Door {
public:
  Door(int, int, uint16_t);
  void open();
  void close();
  bool isOpen();
  bool maybeClose();
private:
  int pin;
  int initialState;
  uint16_t holdTime;
  unsigned long openedAt;
};

#endif
