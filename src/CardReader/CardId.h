#ifndef _CARDID_H_
#define _CARDID_H_

#include <cstring>
#include <stdint.h>

struct CardId
{
	CardId() : 
		length(0)
	{
		memset(uid, 0, 8);
	}

	CardId(uint8_t* uid, size_t length) :
		length(length)
	{
		if(this->length > 8)
		{
			this->length = 8;
		}
		memset(this->uid, 0, 8);
		memcpy(this->uid, uid, this->length);
	}

	CardId(const CardId& other) :
		length(other.length)
	{
		memset(this->uid, 0, 8);
		memcpy(this->uid, other.uid, this->length);
	}

	bool operator==(const CardId& other) const
	{
		if(length == other.length)
		{
			return memcmp(uid, other.uid, length) == 0;
		}
		return false;
	}

	operator bool() const
	{
		return length != 0;
	}

	uint8_t uid[8];
	size_t length;
};

#endif
