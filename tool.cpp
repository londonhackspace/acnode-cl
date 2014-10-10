#include "tool.h"
#include "acnode.h"

Tool::Tool(int pin) {
  _toolpin = pin;
  _toolon = false;
}

void Tool::begin() {
  pinMode(_toolpin, OUTPUT);
}

void Tool::on(user user) {
  if (!_toolon) {
    char msg[64];
    sprintf(msg, "Tool On for ");
    uid_str(msg + strlen(msg), &user);

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    _start = millis();

    // switch tool on here.
    digitalWrite(_toolpin, HIGH);
    // end 
    _toolon = true;
  }
}

void Tool::off(user user) {
  // and switch it off here.
  if (_toolon) {
    char msg[64];

    unsigned long duration = (millis() - _start) / 1000;

    Serial.print("Tool off after: ");
    Serial.print(duration);
    Serial.println(" seconds");

    int hours = 0, mins = 0;

    if (duration > 3600) {
      hours = duration / 3600;
      duration -= hours * 3600;
    }
    if (duration > 60) {
      mins = duration / 60;
      duration -= mins * 60;
    }

    snprintf(msg, 64, "Tool off after running for %02d:%02d:%02d for ", hours, mins, (int)duration);
    uid_str(msg + strlen(msg), &user);

    Serial.println(msg);
    syslog.syslog(LOG_NOTICE, msg);

    // switch tool off here
    digitalWrite(_toolpin, LOW);
    // end
    _toolon = false;
  }
}


