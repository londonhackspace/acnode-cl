#ifndef SERIALREADER_H
#define SERIALREADER_H

#include "cardreader.h"
#include "remotergb.h"

class SerialReader : 	public CardReader,
						public RemoteRGB
{
public:
	SerialReader();
	~SerialReader();

	CardId getCard() override;
	void init() override;

	void set(uint8_t r, uint8_t g, uint8_t b) override;

	void poll() override;

	// called from comms handler
	void onCard(CardId card) { lastCard = card; }

private:
	static void sendData(const unsigned char*,unsigned int);
	static unsigned int receiveData(unsigned char*,unsigned int);

	CardId lastCard;
};

#endif
