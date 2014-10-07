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

