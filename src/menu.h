#ifndef _MENU_H
#define _MENU_H

#include "rgb.h"
#include "button.h"
#include "settings.h"
#include "card.h"

#define MENU_ACTIVE_TIMEOUT 30000

enum MenuState {
  IDLE,
  READY_TO_ADD_USER,
  READY_TO_TAKE_OFFLINE,
  READY_TO_TAKE_ONLINE,
  ADDING_USER
};

class Menu {
public:
  Menu(RGB &rgb, int button_pin);
  void run(Card *c);
  bool active();
  void reset();
private:
  void advance(bool is_maintainer);
  void select(Card *c);
  void take_offline(Card *c);
  void take_online(Card *c);
  void reset_if_timed_out();
  Card added_by;
  Button button;
  RGB &led;
  unsigned long menu_activated_at;
  MenuState state;
};

#endif
