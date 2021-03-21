#ifndef _CARDREADER_H
#define _CARDREADER_H

#include "CardId.h"

class CardReader
{
public:
	virtual ~CardReader();

	virtual CardId getCard()=0;

	virtual void init()=0;
};

#endif
