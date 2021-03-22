#ifndef _REMOTERGB_H
#define _REMOTERGB_H

#include <cstdint>

class RemoteRGB
{
public:

	virtual void set(uint8_t r, uint8_t g, uint8_t b)=0;
};

#endif
