#ifndef LITTLEFS_H
#define LITTLEFS_H

#include <lfs.h>

class SPIFlash;

class LittleFS
{
public:
	LittleFS(SPIFlash* flash);
	~LittleFS();

	void rename(const char* oldname, const char* newname);

	// Kind of a leaky abstraction but eh... the aim of this 
	// class is more to handle setup and connecting to the flash memory
	lfs_t* getLfsHandle() { return &lfs; }

	bool isReady() const { return ready; }

private:
	SPIFlash* flash;

	static int lfsRead(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, void *buffer, lfs_size_t size);
	static int lfsProg(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, const void *buffer, lfs_size_t size);
	static int lfsErase(const struct lfs_config *c, lfs_block_t block);
	static int lfsSync(const struct lfs_config *c);


	lfs_config config;
	lfs_t lfs;
	bool ready;
};

#endif
