#include <Energia.h>

static boolean on = false;
int relay_pin = PG_1;

void tool_on(void) {
  if (!on) {
     Serial.println("TOOL ON!!!!");
    // switch tool on here.
    digitalWrite(relay_pin, HIGH);
    // end 
    on = true;
  }
}

void tool_off(void) {
  // and switch it off here.
  if (on) {
     Serial.println("TOOL OFF!!!!");
    // switch tool off here
    digitalWrite(relay_pin, LOW);
    // end
    on = false;
  }
}


