#ifndef LFSCACHE_H
#define LFSCACHE_H

#include "littlefs.h"
#include "cache.h"

class LFSCache : public CacheBase
{
public:
	LFSCache(LittleFS* fs);
	~LFSCache();

	void begin() override;
    Card get(Card u) override;
    Card get(size_t n) override;
    size_t count() override;
    void set(const Card u) override;
    void purge(void) override;
    int each(void( *callback)(Card u)) override;
    void verify(void) override;

private:
	LittleFS* fs;
	lfs_file_t* fp;
};

#endif
