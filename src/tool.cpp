#include "tool.h"
#include "acnode.h"
#include "utils.h"
#include "network.h"

Tool::Tool(int pin) {
  _toolpin = pin;
  _toolon = false;
  _toolrunpin = -1;
  _turnoff = false;
}

volatile int tool_on_state = 0;
volatile int tool_change_flag = 0;
int tool_on_pin;

// If we have a seperate pin to tell us when the tool is actually running
Tool::Tool(int pin, int runpin) {
  _toolpin = pin;
  _toolon = false;
  _toolrunpin = runpin;
  tool_on_pin = runpin;
  memset(&tool_user, 0, sizeof(user));
  _turnoff = false;
}

// not a member function since it's an ISR.
// The tool can change (lower?) this pin to say when it running
// useful for when we want to record laser cutter tube usage etc.
void toolrunisr() {
  int state = digitalRead(tool_on_pin);

  if (state != tool_on_state) {
    tool_change_flag = 1;
  }
  tool_on_state = state;
}

void Tool::begin() {
  pinMode(_toolpin, OUTPUT);
  if (acsettings.toolonpin_activehigh) {
    digitalWrite(_toolpin, LOW);
  } else {
    digitalWrite(_toolpin, HIGH);
  }

  if (_toolrunpin) {
    tool_change_flag = 0;
    if (acsettings.toolrunpin_activehigh) {
      pinMode(_toolrunpin, INPUT_PULLDOWN);
    } else {
      pinMode(_toolrunpin, INPUT_PULLUP);
    }
    tool_on_state = digitalRead(_toolrunpin);
    attachInterrupt(_toolrunpin, toolrunisr, CHANGE);
  }
}

void Tool::poll() {
  if (_turnoff) {
      unsigned long duration = (millis() - _ontime) / 1000;
      // have we been running for more than the minimum on time?
      if (duration > acsettings.minontime) {
        // if so we can turn off now
        _turnoff = false;

        // report to the acserver that we're done with this tool
        networking::reportToolUse(tool_user, 0);
        off();
      }
  }

  // do we need to run this?
  if (!_toolrunpin) {
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

  int offstate, onstate;
  if (acsettings.toolrunpin_activehigh) {
    offstate = 0;
    onstate = 1;
  } else {
    offstate = 1;
    onstate = 0;
  }

  if (tool_on_state == onstate) {
    if (!_toolon) {
      char msg[] = "Tool appears to have started running while it's off!";
      Serial.println(msg);
      syslog.syslog(LOG_WARNING, msg);
    } else {
      startrunning();
    }
  }

  if (tool_on_state == offstate) {
    if (!_toolon) {
      char msg[] = "Tool appears to have stopped running while it's off!";
      Serial.println(msg);
      syslog.syslog(LOG_WARNING, msg);
    } else {
      stoprunning();
    }
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
    tool_user.str(msg + strlen(msg));

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    // update how long the tool has run for.
    acsettings.runtime += duration;
    set_settings(acsettings);

    snprintf(msg, 64, "Total run time now: ");
    duration_str(msg + strlen(msg), acsettings.runtime);

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    if (network) {
      networking::toolUseTime(tool_user, duration);
    }
}

void Tool::on(Card user) {
  if (!_toolon) {
    char msg[64];
    if (acsettings.status == 0 && !user.is_maintainer()) {
      // if the tool is disabled only maintainers can switch it on.
      sprintf(msg, "Tool out of service, ignoring user ");
      user.str(msg + strlen(msg));

      Serial.println(msg);
      syslog.syslog(LOG_NOTICE, msg);
      return;
    }

    sprintf(msg, "Tool switched on for ");
    user.str(msg + strlen(msg));

    if (user.is_maintainer()) {
      sprintf(msg + strlen(msg), " (Maintainer)");
    }

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    if (!_toolrunpin) {
      startrunning();
    }

    // tell acserver that the tool is in use
    networking::reportToolUse(user, 1);

    // switch tool on here.
    if (acsettings.toolonpin_activehigh) {
      digitalWrite(_toolpin, HIGH);
    } else {
      digitalWrite(_toolpin, LOW);
    }
    // end
    _toolon = true;
    _ontime = millis();
    tool_user = user;
  }

  if (_turnoff) {
    // we were asked to turn the tool off
    // but are now being asked to turn it on again
    // so don't turn it off.
    _turnoff = false;
    // looks as if we cope ok if the tool is turned on by a different user
  }
}

void Tool::off() {
  // and switch it off here.
  if (_toolon) {

    if (acsettings.minontime > 0) {
      unsigned long duration = (millis() - _ontime) / 1000.0;
      // have we been running for less than the minimum on time?
      Serial.print("minimum on, ");
      Serial.print("duration: ");
      Serial.print(duration);
      Serial.print(" minontime ");
      Serial.println(acsettings.minontime);
      if (duration < acsettings.minontime) {
        // if so we need to turn the tool off in the future
        _turnoff = true;
        Serial.println("Not turning off yet");
        return;
      }
    }

    // switch tool off here
    if (acsettings.toolonpin_activehigh) {
      digitalWrite(_toolpin, LOW);
    } else {
      digitalWrite(_toolpin, HIGH);
    }
    // end

    if (!_toolrunpin) {
      stoprunning();
    }

    _toolon = false;
    _turnoff = false;
  }
}

// used to turn the button green when the tool is powerd on.
boolean Tool::status() {
  return _toolon;
}
