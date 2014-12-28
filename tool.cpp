#include "tool.h"
#include "acnode.h"
#include "utils.h"

Tool::Tool(int pin) {
  _toolpin = pin;
  _toolon = false;
  _toolonpin = -1;
}

volatile int tool_on_state = 0;
volatile int tool_change_flag = 0;
int tool_on_pin;

// If we have a seperate pin to tell us when the tool is actually running
Tool::Tool(int pin, int onpin) {
  _toolpin = pin;
  _toolon = false;
  _toolonpin = onpin;
  tool_on_pin = onpin;
  memset(&tool_user, 0, sizeof(user));
}

// not a member function since it's an ISR.
void toolonisr() {
  int state = digitalRead(tool_on_pin);

  if (state != tool_on_state) {
    tool_change_flag = 1;
  }
  tool_on_state = state;
}

void Tool::begin() {
  pinMode(_toolpin, OUTPUT);
  digitalWrite(_toolpin, LOW);

  if (_toolonpin) {
    tool_on_state = 0;
    tool_change_flag = 0;
    pinMode(_toolonpin, INPUT_PULLUP);
    attachInterrupt(_toolonpin, toolonisr, CHANGE);
  }
}

void Tool::poll() {
  // do we need to run this?
  if (!_toolonpin) {
    return;
  }

  // has anything happened?
  if (!tool_change_flag) {
    // if not return
    return;
  }

  Serial.print("Tool state changed: ");
  Serial.println(tool_on_state);
  tool_change_flag = 0;

  // the laser signal is inverted
  if (tool_on_state == 0) {
    startrunning();
  }

  if (tool_on_state == 1) {
    stoprunning();
  }
}

void Tool::startrunning() {
    _start = millis();
}

void Tool::stoprunning() {
    char msg[64];
    unsigned long duration = (millis() - _start) / 1000;

    if (duration < 5) {
      // just ignore very short runs, the air assist comes on for ~3 secs when the laser cutter key is switched on.
      return;
    }

    Serial.print("Tool off after: ");
    Serial.print(duration);
    Serial.println(" seconds");

    snprintf(msg, 64, "Tool off after running for ");
    duration_str(msg + strlen(msg), duration);
    snprintf(msg + strlen(msg), 64 - strlen(msg), " for ");
    uid_str(msg + strlen(msg), &tool_user);

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    // update how long the tool has run for.
    acsettings.runtime += duration;
    set_settings(acsettings);

    snprintf(msg, 64, "Total run time now: ");
    duration_str(msg + strlen(msg), acsettings.runtime);

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);
}

void Tool::on(user user) {
  if (!_toolon) {
    char msg[64];
    if (acsettings.status == 0 && !user.maintainer) {
      // if the tool is disabled only maintainers can switch it on.
      sprintf(msg, "Tool out of service, ignoring user ");
      uid_str(msg + strlen(msg), &user);

      Serial.println(msg);
      syslog.syslog(LOG_NOTICE, msg);
      return;
    }

    sprintf(msg, "Tool switched on for ");
    uid_str(msg + strlen(msg), &user);

    if (user.maintainer) {
      sprintf(msg + strlen(msg), " (Maintainer)");
    }

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    if (!_toolonpin) {
      startrunning();
    }

    // switch tool on here.
    digitalWrite(_toolpin, HIGH);
    // end 
    _toolon = true;
    memcpy(&tool_user, &user, sizeof(user));
  }
}

void Tool::off(user user) {
  // and switch it off here.
  if (_toolon) {

    // switch tool off here
    digitalWrite(_toolpin, LOW);
    // end

    if (!_toolonpin) {
      stoprunning();
    }

    _toolon = false;
  }
}


