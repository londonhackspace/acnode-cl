#include "Energia.h"
#include "utils.h"

void dumpHex(const uint8_t* buffer, int len)
{
  for(byte i=0; i < len; i++) {
    char text[4];
    sprintf(text, "%02X\x00", (byte)(*(buffer + i)));
    Serial.print(text);
  }
  Serial.println("");
}

