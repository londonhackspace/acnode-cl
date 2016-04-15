#ifndef _DOOR_H_
#define _DOOR_H_

class Door {
public:
  Door(int, int);
  void open();
  void close();
  bool isOpen();
  bool maybeClose();
private:
  int pin;
  int initialState;
  unsigned long openedAt;
};

#endif
