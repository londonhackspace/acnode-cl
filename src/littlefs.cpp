#include "littlefs.h"
#include "spiflash.h"

#include <Energia.h>

LittleFS::LittleFS(SPIFlash* flash) :
	flash(flash)
{
	memset(&config, 0, sizeof(config));
	config.context = this;
	config.read = LittleFS::lfsRead;
	config.prog = LittleFS::lfsProg;
	config.erase = LittleFS::lfsErase;
	config.sync = LittleFS::lfsSync;

	config.read_size = 1;
	config.prog_size = 1;
	config.block_size = flash->getSectorSize();
	config.block_count = flash->getCapacity()/flash->getSectorSize();
	config.block_cycles = 200;

	Serial.print("LittleFS: Block size ");
	Serial.println(config.block_size);
	Serial.print("LittleFS: Block count ");
	Serial.println(config.block_count);

	// cache a page or 1Kb, whichever is larger
	config.cache_size = max(flash->getPageSize(), 1024);
	config.lookahead_size = 64;

	int retval = lfs_mount(&lfs, &config);
	if(retval == LFS_ERR_CORRUPT)
	{
		Serial.println("LittleFS: Formatting filesystem");
		retval = lfs_format(&lfs, &config);
		if(retval != LFS_ERR_OK)
		{
			Serial.print("LittleFS: Error formatting: ");
			Serial.println(retval);
		}
		else
		{
			retval = lfs_mount(&lfs, &config);
		}
	}
	if(retval != LFS_ERR_OK)
	{
		Serial.print("LittleFS: Error mounting filesystem: ");
		Serial.println(retval);
	}

	ready = retval == LFS_ERR_OK;
}

LittleFS::~LittleFS()
{

}

int LittleFS::lfsRead(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, void *buffer, lfs_size_t size)
{
	LittleFS* self = reinterpret_cast<LittleFS*>(c->context);

	if(!self->flash->isReady())
	{
		return LFS_ERR_IO;
	}

	self->flash->read((block*self->flash->getSectorSize())+off, size, buffer);

	return LFS_ERR_OK;
}

int LittleFS::lfsProg(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size)
{
	LittleFS* self = reinterpret_cast<LittleFS*>(c->context);

	if(!self->flash->isReady())
	{
		return LFS_ERR_IO;
	}

	self->flash->write((block*self->flash->getSectorSize())+off, size, buffer);

	return LFS_ERR_OK;
}

int LittleFS::lfsErase(const struct lfs_config *c, lfs_block_t block)
{
	LittleFS* self = reinterpret_cast<LittleFS*>(c->context);

	if(!self->flash->isReady())
	{
		return LFS_ERR_IO;
	}

	self->flash->eraseBlock(block * self->flash->getSectorSize());

	return LFS_ERR_OK;
}

int LittleFS::lfsSync(const struct lfs_config *c)
{
	return LFS_ERR_OK;
}

void LittleFS::rename(const char* oldname, const char* newname)
{
	lfs_rename(&lfs, oldname, newname);
}