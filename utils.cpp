#include "Energia.h"
#include "utils.h"

void dumpHex(const uint8_t* buffer, int len)
{
  for(byte i=0; i < len; i++) {
    char text[4];
    sprintf(text, "%02X", (byte)(*(buffer + i)));
    text[2] = 0;
    Serial.print(text);
  }
}

// turns a duration into HH:MM:SS
// the string needs to have enough room.
void duration_str(char * str, unsigned long duration)
{
    int hours = 0, mins = 0;

    if (duration > 3600) {
      hours = duration / 3600;
      duration -= hours * 3600;
    }
    if (duration > 60) {
      mins = duration / 60;
      duration -= mins * 60;
    }

    sprintf(str, "%02d:%02d:%02d", hours, mins, (int)duration);
}

