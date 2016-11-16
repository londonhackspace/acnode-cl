// A bunch of useful macros.

#define NOTIMPLEMENTED  Serial.println(String(__FILE__) + ":" + String(__LINE__) + ": Not implemented.");
#define DEBUG(x)        Serial.println(String(__FILE__) + ":" + String(__LINE__) + ":" + x);
