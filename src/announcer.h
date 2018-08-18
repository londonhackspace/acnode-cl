#ifndef _ANNOUNCER_H_
#define _ANNOUNCER_H_

class Announcer {
public:
  virtual void RFID(char *) = 0;
  virtual void START() = 0;
  virtual void BELL() = 0;
};

#endif
