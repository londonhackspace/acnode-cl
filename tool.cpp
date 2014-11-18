#include "tool.h"
#include "acnode.h"
#include "utils.h"

Tool::Tool(int pin) {
  _toolpin = pin;
  _toolon = false;
}

void Tool::begin() {
  pinMode(_toolpin, OUTPUT);
  digitalWrite(_toolpin, HIGH);
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
    sprintf(msg, "Tool On for ");
    uid_str(msg + strlen(msg), &user);

    if (user.maintainer) {
      sprintf(msg + strlen(msg), " (Maintainer)");
    }

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    _start = millis();

    // switch tool on here.
    digitalWrite(_toolpin, LOW);
    // end 
    _toolon = true;
  }
}

void Tool::off(user user) {
  // and switch it off here.
  if (_toolon) {
    char msg[64];

    // switch tool off here
    digitalWrite(_toolpin, HIGH);
    // end

    unsigned long duration = (millis() - _start) / 1000;

    Serial.print("Tool off after: ");
    Serial.print(duration);
    Serial.println(" seconds");

    snprintf(msg, 64, "Tool off after running for ");
    duration_str(msg + strlen(msg), duration);
    snprintf(msg + strlen(msg), 64 - strlen(msg), " for ");
    uid_str(msg + strlen(msg), &user);

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    // update how long the tool has run for.
    acsettings.runtime += duration;
    set_settings(acsettings);

    snprintf(msg, 64, "Total run time now: ");
    duration_str(msg + strlen(msg), duration);

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    _toolon = false;
  }
}


