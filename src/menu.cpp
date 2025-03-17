#include "menu.h"
#include "network.h"

Menu::Menu(RGB &rgb, int button_pin) :
  button(button_pin),
  led(rgb),
  menu_activated_at(1<<31),
  state(IDLE),
  pending(false) 
{
  button.begin();
}

void Menu::reset_if_timed_out() {
  unsigned long now = millis();
  if (now - menu_activated_at > MENU_ACTIVE_TIMEOUT) {
    reset();
  }
}

void Menu::run(Card *c) {
  // reset_if_timed_out();

  if (c->is_valid()) {
    ButtonEvent event = button.poll();
    switch (event) {
      case ButtonEvent::SHORT_PRESS:
        advance(c->is_maintainer());
        pending = false; 
        break;
      case ButtonEvent::LONG_PRESS:
        select(c);
        pending = false;
        break;
      case ButtonEvent::PRESSED:
        pending = true;
        break;
      case ButtonEvent::NONE:
        break;
    }
  }/* else {
    Serial.println("No Card Present");
    state = IDLE;
    pending = false;
  }*/

  switch (state) {
    case READY_TO_ADD_USER:
      led.flashing(YELLOW, pending);
      break;
    case READY_TO_TAKE_OFFLINE:
      led.flashing(RED, pending);
      break;
    case READY_TO_TAKE_ONLINE:
      led.flashing(GREEN, pending);
      break;
    case ADDING_USER:
      if (pending) {
        led.flashing(YELLOW, true);
      } else {
        led.solid(YELLOW);
      }
      break;
    case IDLE:
      {
        Colour colour = c->is_maintainer() ? ORANGE : GREEN;
        if (pending) {
          led.flashing(colour, true);
        } else {
          led.solid(colour);
        }
      }
      break;
    case TAKEN_ONLINE:
      led.solid(GREEN);
      break;
    case TAKEN_OFFLINE:
      led.solid(RED);
      break;
  }

  if (state == ADDING_USER) {
    if (c->is_valid() && *c != added_by) {
      networking::addNewUser(*c, added_by);
      reset();
    }
  }
}

void Menu::advance(bool is_maintainer)
{
  bool is_online = (acsettings.status == 1);
  menu_activated_at = millis();
  if (is_maintainer) {
    switch(state) {
      case IDLE:
        state = READY_TO_ADD_USER;
        break;
      case READY_TO_ADD_USER:
        state = is_online ? READY_TO_TAKE_OFFLINE : READY_TO_TAKE_ONLINE;
        break;
      case READY_TO_TAKE_OFFLINE:
      case READY_TO_TAKE_ONLINE:
      case ADDING_USER:
      case TAKEN_OFFLINE:
      case TAKEN_ONLINE:
        state = IDLE;
        break;
      default:
          DEBUG("Unhandled");
    }
  } else {
    // An ordinary user can only take a machine offline.
    state = (state == IDLE) ? READY_TO_TAKE_OFFLINE : IDLE;
  }
}

void Menu::select(Card *c) {
  switch (state) {
    case READY_TO_ADD_USER:
      added_by = *c;
      menu_activated_at = millis();
      state = ADDING_USER;
      break;
    case READY_TO_TAKE_OFFLINE:
      take_offline(c);
      state = TAKEN_OFFLINE;
      break;
    case READY_TO_TAKE_ONLINE:
      take_online(c);
      led.solid(GREEN); // Immediately change the LED colour rather than wait for polling. Strictly speaking, it should be orange for maintainer
      state = TAKEN_ONLINE;
      break;
    case ADDING_USER:
    case IDLE:
    case TAKEN_ONLINE:
    case TAKEN_OFFLINE:
      break;
  }
}

void Menu::reset() {
  menu_activated_at = 1<<31;
  added_by = Card();
  state = IDLE;
  pending = false;
}

void Menu::take_offline(Card *c) {
  networking::setToolStatus(0, *c);
  acsettings.status = 0;
  set_settings(acsettings);
}

void Menu::take_online(Card *c) {
  networking::setToolStatus(1, *c);
  acsettings.status = 1;
  set_settings(acsettings);
}

bool Menu::active() {
  return state != IDLE;
}

bool Menu::is_adding_user() {
  return state == ADDING_USER;
}
