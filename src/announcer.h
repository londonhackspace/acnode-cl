#ifndef _ANNOUNCER_H_
#define _ANNOUNCER_H_

class Announcer {
public:
  virtual void RFID(char *) = 0;
  virtual void START() = 0;
  virtual void BELL() = 0;
  virtual void EXIT() = 0;

  // for maintainance tasks of the announcer
  virtual void run() {}
};

#endif
