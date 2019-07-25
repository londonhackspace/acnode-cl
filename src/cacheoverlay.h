#ifndef _CACHE_OVERLAY_H_
#define _CACHE_OVERLAY_H_

#include "cache.h"

class CacheOverlay : public Cache
{
public:
    /*
     * Passes writes through to base, overlay is used as a fixed, read-only
     * extra layer and is queried first.
     */
    CacheOverlay(Cache* base, Cache* overlay);
    ~CacheOverlay();

    void begin() override;
    Card get(Card u) override;
    void set(const Card u) override;
    void purge(void) override;
    int each(void( *callback)(Card u)) override;
    void verify(void) override;
    void fill(void) override;
    void fill(int count) override;
    void list(void) override;

private:
    Cache* base;
    Cache* overlay;
};

#endif
