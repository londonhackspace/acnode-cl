#ifndef _UTILS_H_
#define _UTILS_H_

void dumpHex(const uint8_t* buffer, int len);
void duration_str(char * str, unsigned long duration);

#define STRINGIFY_(X) #X
#define STRINGIFY(X) STRINGIFY_(X)

#endif

