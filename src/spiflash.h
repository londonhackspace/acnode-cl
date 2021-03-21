#ifndef SPIFLASH_H
#define SPIFLASH_H

#include <cstdint>
#include <cstddef>

/* SPIFlash - provides storage support for SPI flash devices
 * 
 * This is a fairly simple implementation, and only works for
 * SPI flash devices supporting JEDEC SFDP. This allows the reliable
 * detection of flash devices without too much guesswork.
 */
class SPIFlash
{
public:
	~SPIFlash();

	// Returns a SPIFlash object if there's any flash available
	static SPIFlash* get(uint8_t CSPin);

	// Checks for some flash memory
	static bool detect(uint8_t CSPin);

	bool isReady() const  { return ready; }

	uint32_t getSectorSize() const { return sectorSize; }
	uint32_t getPageSize() const { return pageSize; }
	uint32_t getCapacity() const { return capacity; }

	void read(size_t start, size_t count, void* buffer) const;
	void write(size_t start, size_t count, const void* buffer);
	void eraseBlock(size_t start);
	void eraseChip();

private:
	SPIFlash(uint8_t CSPin);

	// helper, used both in construction and detection, for reading the SFDP table
	static void readSFDP(uint8_t CSPin, uint32_t address, uint8_t* buffer, size_t length);

	// Interrogate flash for parameters
	// called from constructor
	bool init();
	bool readBasicSFDPHeader(uint32_t* table, size_t length);

	bool isBusy() const;

	// Writes need a little special treatment to deal with block sizes
	void writeInternal(size_t start, size_t count, const void* buffer);

	void writeEnable();

	uint8_t CSPin;
	bool ready; // if true, init was successful

	uint32_t capacity;
	uint32_t pageSize;
	uint32_t sectorSize;
	uint8_t sectorEraseCommand;
};

#endif
