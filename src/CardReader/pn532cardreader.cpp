#include "pn532cardreader.h"

CardReader::~CardReader() {}

PN532CardReader::PN532CardReader(PN532 &nfc) :
	nfc(nfc)
{

}

PN532CardReader::~PN532CardReader()
{

}

CardId PN532CardReader::getCard()
{
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0, 0};  // Buffer to store the returned UID
  	uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

	// Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
	// 'uid' will be populated with the UID, and uidLength will indicate
	// if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
	bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

	nfc.powerDown();

	if(success)
	{
		return CardId(uid, uidLength);
	}

	return CardId();
}