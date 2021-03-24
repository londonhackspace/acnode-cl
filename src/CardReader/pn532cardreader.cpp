#include "pn532cardreader.h"
#include <Arduino.h>
#include "../acnode.h"

CardReader::~CardReader() {}

PN532CardReader::PN532CardReader(PN532 &nfc) :
	nfc(nfc)
{

}

PN532CardReader::~PN532CardReader()
{

}

void PN532CardReader::init()
{
	nfc.begin();

	uint32_t versiondata = nfc.getFirmwareVersion();

	if (! versiondata) {
	Serial.println("Didn't find PN53x board");
	syslog.syslog(LOG_EMERG, "Couldn't find PN532 board");
	while (1); // halt
	}

	// Got ok data, print it out!
	Serial.print("Found chip PN5"); Serial.print((versiondata>>24) & 0xFF, HEX);
	Serial.print(", Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
	Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

	// Set the max number of retry attempts to read from a card
	// This prevents us from waiting forever for a card, which is
	// the default behaviour of the PN532.
	//
	// with the powerdown thing we don't need a long timeout
	// so use 16...
	nfc.setPassiveActivationRetries(0x10);

	// configure board to read RFID tags
	nfc.SAMConfig();
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

void PN532CardReader::poll()
{
	
}