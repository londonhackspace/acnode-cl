#ifndef _ANNOUNCER_H_
#define _ANNOUNCER_H_

class Announcer {
public:
  virtual void RFID(char *, int granted) = 0;
  virtual void START() = 0;
  virtual void BELL() = 0;
  virtual void EXIT(int doorbellack) = 0;
  virtual void WEDGED() = 0;
  virtual void ALIVE() = 0;
  virtual void TOOL_DEACTIVATE() = 0;

  // for maintainance tasks of the announcer
  virtual void run() {}
};

#endif
