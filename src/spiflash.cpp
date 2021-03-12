#include "spiflash.h"

#include <Energia.h>
#include <SPI.h>

SPIFlash::SPIFlash(uint8_t CSPin) :
	CSPin(CSPin),
	ready(false)
{
	pinMode(CSPin, OUTPUT);
	digitalWrite(CSPin, HIGH);
	SPI.begin();

	ready = init();
}

SPIFlash::~SPIFlash()
{

}

bool SPIFlash::detect(uint8_t CSPin)
{
	// set up CS pin and SPI
	pinMode(CSPin, OUTPUT);
	digitalWrite(CSPin, HIGH);
	SPI.begin();

	uint32_t val = 0;
	readSFDP(CSPin, 0x00, (uint8_t*)&val, 4);

	// see if this is the start of a valid JEDEC table
	if(val == 0x50444653)
	{
		// Tear down SPI since we're just checking
		SPI.end();
		return true;
	}
	else
	{
		Serial.print("Flash eheader is ");
		Serial.println(val, HEX);
	}

	// Tear down SPI since we're just checking
	SPI.end();

	return false;
}

void SPIFlash::readSFDP(uint8_t CSPin, uint32_t address, uint8_t* buffer, size_t length)
{
	// attempt to grab the first JEDEC table byte
	digitalWrite(CSPin, LOW);
	SPI.transfer(0x5a);
	SPI.transfer(((address & 0xff0000) >> 16) & 0xff);
	SPI.transfer(((address & 0xff00) >> 8) & 0xff);
	SPI.transfer(address & 0xff);
	SPI.transfer(0xff); // dummy byte

	for(size_t i = 0; i < length; ++i)
	{
		buffer[i] = SPI.transfer(0x00);
	}

	digitalWrite(CSPin, HIGH);
}

SPIFlash* SPIFlash::get(uint8_t CSPin)
{
	if(detect(CSPin))
	{
		return new SPIFlash(CSPin);
	}

	return nullptr;
}

bool SPIFlash::init()
{
	uint32_t headerBuffer[2];

	// first read the overall header
	readSFDP(CSPin, 0x04, reinterpret_cast<uint8_t*>(headerBuffer), 4);

	if(((headerBuffer[0] >> 24) & 0xff) != 0xff)
	{
		Serial.print("SPIFlash: Unsupported SFDP access protocol 0x");
		Serial.println(((headerBuffer[0] >> 24) & 0xff), HEX);
		return false;
	}

	uint8_t parameterHeaderCount = ((headerBuffer[0] >> 16) & 0xff)+1;

	Serial.print("SPIFlash: Found ");
	Serial.print(parameterHeaderCount, DEC);
	Serial.print(" SFDP parameter header(s) with SFDP version ");
	Serial.print((headerBuffer[0] >> 8) & 0xff, DEC);
	Serial.print(".");
	Serial.println(headerBuffer[0] & 0xff, DEC);

	// read parameter headers
	uint32_t ptr = 0x8;

	for(uint8_t tableNumber = 0; tableNumber < parameterHeaderCount; ++tableNumber)
	{
		readSFDP(CSPin, ptr, reinterpret_cast<uint8_t*>(headerBuffer), 8);

		uint8_t parameterLength = (headerBuffer[0] >> 24) & 0xff;
		uint8_t majorRevision = (headerBuffer[0] >> 16) & 0xff;
		uint8_t minorRevision = (headerBuffer[0] >> 8) & 0xff;
		uint16_t parameterId = (headerBuffer[0] & 0xff) |
								((headerBuffer[1] >> 16) & 0xff00);

		Serial.print("SPIFlash: Got table 0x");
		Serial.print(parameterId, HEX);
		Serial.print(" version ");
		Serial.print(majorRevision, DEC);
		Serial.print(".");
		Serial.print(minorRevision, DEC);
		Serial.print(" length ");
		Serial.print(parameterLength*4, DEC);
		Serial.println(" bytes");

		uint32_t tableBuffer[16];

		// we only actually care about one table
		if(parameterId == 0xff00)
		{
			// clamp parameter length to what we believe to be the maximum
			if(parameterLength > 16)
			{
				parameterLength = 16;	
			}
			readSFDP(CSPin, headerBuffer[1] & 0xffffff, reinterpret_cast<uint8_t*>(tableBuffer), 4*parameterLength);
			readBasicSFDPHeader(tableBuffer, 4*parameterLength);
		}

		ptr += 0x8;
	}


	return true;
}

bool SPIFlash::readBasicSFDPHeader(uint32_t* table, size_t length)
{
	if(table[1] & 0x80000000)
	{
		// capacity is 2^data[1] bits - subtract 3 to get bytes
		int pow = (table[1] & 0x7fffffff) - 3;
		capacity = 2 << pow;
	}
	else
	{
		// Capacity is directly specified in bits -> divide by 8 (shift by 3) to get bytes
		capacity = (table[1] & 0x7fffffff) >> 3;
	}
	Serial.print("SPIFlash: Capacity is ");
	Serial.print(capacity, DEC);
	Serial.println(" bytes");
	return true;
}