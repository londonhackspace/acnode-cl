#include <Energia.h>

static boolean on = false;

void tool_on(void) {
  if (!on) {
     Serial.println("TOOL ON!!!!");
    // switch tool on here.
    
    // end 
    on = true;
  }
}

void tool_off(void) {
  // and switch it off here.
  if (on) {
     Serial.println("TOOL OFF!!!!");
    // switch tool off here
    
    // end
    on = false;
  }
}


