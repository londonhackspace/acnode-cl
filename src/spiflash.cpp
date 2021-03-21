#include "spiflash.h"
#include "acnode.h"

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

// A lot of this was loosely backported from my rewrite project
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

	if(capacity >= 0xffffff)
	{
		Serial.println("SPIFlash: Limiting capacity since we only support 3-byte addresses");
		capacity = 0xffffff;
	}

	// depending on what standard the device supports, we can
	// get more information using the later tables however they
	// are only present on newer devices
	if((length/4) >= 11)
	{
		uint8_t n = uint8_t((table[10] >> 4) & 0xf);
		pageSize = 1 << n;
	}
	else
	{
		if((table[0] & 0x4) == 0x4)
		{
			// we know the flash supports at least this.
			// It might support more, but we can't be sure
			pageSize = 64;
		}
		else
		{
			// Oh dear. This will be really slow.
			pageSize = 1;
			Serial.println("SPIFlash: Device reports it does not support 64Kb page writes");
			Serial.println("SPIFlash: This means we must assume we can only write one byte at a time!");
			Serial.println("SPIFlash: This will be slow!");
		}
	}

	// do we have the higher tables? Very early implementations
	// supposedly only had 4
	if((length/4) < 9)
	{
		Serial.println("SPIFlash: SFDP table contains fewer than 9 fields - using legacy values");
		if((table[0] & 0x3) == 1)
		{
			// 4Kb mode supported
			sectorSize = 4096;
			sectorEraseCommand = (table[0] >> 8) & 0xff;
		}
		else
		{
			Serial.println("SPIFlash: 4kb mode not supported - not enough information to proceed");
			return false;
		}
	}
	else
	{
		// Find the smallest block size we can erase
		// This is all LittleFS cares about
		uint32_t minSize = ~0;
		uint8_t minInstruction = 0;

		// There are two bytes for 4 possible sizes
		for(int i=7; i<9; ++i)
		{
			// upper word
			uint32_t size = ((table[i] >> 16) & 0xff);
			if(size != 0)
			{
				size = 1 << size;
				Serial.print("SPIFlash: Command 0x");
				Serial.print((table[i] >> 24) & 0xff, HEX);
				Serial.print(" can erase ");
				Serial.print(size, DEC);
				Serial.print(" bytes (raw 0x");
				Serial.print(((table[i] >> 16) & 0xff), HEX);
				Serial.println(")");
				if(size < minSize)
				{
					minSize = size;
					minInstruction = (table[i] >> 24) & 0xff;
				}
			}

			// lower word
			size = (table[i] & 0xff);
			if(size != 0)
			{
				size = 1 << size;
				Serial.print("SPIFlash: Command 0x");
				Serial.print((table[i] >> 8) & 0xff, HEX);
				Serial.print(" can erase ");
				Serial.print(size, DEC);
				Serial.print(" bytes (raw 0x");
				Serial.print( (table[i]  & 0xff), HEX);
				Serial.println(")");
				if(size < minSize)
				{
					minSize = size;
					minInstruction = (table[i] >> 8) & 0xff;
				}
			}
		}

		sectorEraseCommand = minInstruction;
		sectorSize = minSize;

		Serial.print("SPIFlash: Will use erase command 0x");
		Serial.print(sectorEraseCommand, HEX);
		Serial.print(" to erase ");
		Serial.print(sectorSize);
		Serial.println(" bytes at a time");
	}

	return true;
}

void SPIFlash::read(size_t start, size_t count, void* buffer) const
{
	while(isBusy())
	{
		wdog.feed();
		sleep(1);
	}

	digitalWrite(CSPin, LOW);
	SPI.transfer(0x03); // Read command
	SPI.transfer((start >> 16) & 0xff);
	SPI.transfer((start >> 8) & 0xff);
	SPI.transfer(start & 0xff);
	for(size_t i = 0; i < count; ++i)
	{
		reinterpret_cast<uint8_t*>(buffer)[i] = SPI.transfer(0x00);
	}
	digitalWrite(CSPin, HIGH);

}


void SPIFlash::write(size_t start, size_t count, const void* buffer)
{
	uint32_t progress = 0;

	while(progress < count)
	{
		// We can write at most pageSize bytes
		uint32_t toWrite = min(count-progress, pageSize);
		
		// calculate the address we write to this time
		uint32_t dest = start + progress;
		
		//we can only write to the next page boundary
		uint32_t pageOffset = dest % pageSize;
		if((toWrite + pageOffset) > pageSize)
		{
			toWrite -= pageOffset;
		}

		writeInternal(dest, toWrite, reinterpret_cast<const uint8_t*>(buffer)+progress);
		progress += toWrite;
	}
}

void SPIFlash::writeInternal(size_t start, size_t count, const void* buffer)
{
	while(isBusy())
	{
		wdog.feed();
		sleep(1);
	}

	writeEnable();

	digitalWrite(CSPin, LOW);
	SPI.transfer(0x02); // Write command
	SPI.transfer((start >> 16) & 0xff);
	SPI.transfer((start >> 8) & 0xff);
	SPI.transfer(start & 0xff);
	SPI.transfer(const_cast<void*>(buffer), count);
	digitalWrite(CSPin, HIGH);
}

void SPIFlash::eraseBlock(size_t start)
{
	while(isBusy())
	{
		wdog.feed();
		sleep(1);
	}

	Serial.println("SPI Flash Erasing");

	writeEnable();
	digitalWrite(CSPin, LOW);
	SPI.transfer(sectorEraseCommand);
	SPI.transfer((start >> 16) & 0xff);
	SPI.transfer((start >> 8) & 0xff);
	SPI.transfer(start & 0xff);
	digitalWrite(CSPin, HIGH);
}

bool SPIFlash::isBusy() const
{
	digitalWrite(CSPin, LOW);
	SPI.transfer(0x05);
	uint8_t result = SPI.transfer(0x00);
	digitalWrite(CSPin, HIGH);
	return (result & 0x01);
}

void SPIFlash::writeEnable()
{
	while(isBusy())
	{
		wdog.feed();
		sleep(1);
	}

	digitalWrite(CSPin, LOW);
	SPI.transfer(0x06); // Write Enable command
	digitalWrite(CSPin, HIGH);
}

void SPIFlash::eraseChip()
{
	while(isBusy())
	{
		wdog.feed();
		sleep(1);
	}
	
	writeEnable();

	digitalWrite(CSPin, LOW);
	SPI.transfer(0xc7); // Chip erase command
	digitalWrite(CSPin, HIGH);
}