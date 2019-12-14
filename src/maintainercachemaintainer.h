#ifndef MAINTAINER_CACHE_MAINTAINER_H
#define MAINTAINER_CACHE_MAINTAINER_H

#include "every.h"

class Cache;
class ACServer;
class Card;

/// Responsible for keeping the maintainerCache full of current maintainers
class MaintainerCacheMaintainer
{
public:
    MaintainerCacheMaintainer(Cache* maintainerCache, ACServer* acserver);
    ~MaintainerCacheMaintainer();

    void run();

private:
    void doCheck();

    // callback for use by cache iteration
    void onCard(Card u);

    // static wrapper
    static void onCardCallback(Card u);

    // some scratch space for processing the cache
    int currentCacheCount;
    Card* tempCache;
    uint8_t tempCacheCount;

    Cache* maintainerCache;
    ACServer* acserver;

    Every checkMaintainers;
    bool init;
};

#endif
