#ifndef _PN532CARDREADER_H
#define _PN532CARDREADER_H

#include "cardreader.h"
#include "PN532.h"

class PN532CardReader : public CardReader
{
public:
	PN532CardReader(PN532 &nfc);
	~PN532CardReader();

	CardId getCard() override;

	void init() override;

private:
	PN532 &nfc;
};

#endif
