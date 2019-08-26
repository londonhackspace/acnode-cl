#include "maintainercachemaintainer.h"
#include "card.h"
#include "cache.h"
#include "ACServer/DataRecords.h"
#include "ACServer/ACServer.h"
#include <Arduino.h>

// Arduino.h brings in stupid declarations that clash with <functional>
#undef min
#undef max

// ugly hack for callbacks
static MaintainerCacheMaintainer* currentCacheMaintainer = nullptr;

void MaintainerCacheMaintainer::onCardCallback(Card u)
{
    if(currentCacheMaintainer)
    {
        currentCacheMaintainer->onCard(u);
    }
}

MaintainerCacheMaintainer::MaintainerCacheMaintainer(Cache* maintainerCache, ACServer* acserver) :
    currentCacheCount(0),
    tempCache(nullptr),
    tempCacheCount(0),
    maintainerCache(maintainerCache),
    acserver(acserver),
    checkMaintainers(600000), // check every 10 minutes
    init(true)
{
    
}

MaintainerCacheMaintainer::~MaintainerCacheMaintainer()
{

}

void MaintainerCacheMaintainer::run()
{
    if(init)
    {
        init = false;
        checkMaintainers.begin();
        doCheck();
    }
    else if(checkMaintainers.check())
    {
        doCheck();
    }
}

void MaintainerCacheMaintainer::onCard(Card u)
{
    // ignore stale entries
    if(!u.is_valid())
    {
        return;
    }

    for(uint8_t i = 0; i < tempCacheCount; ++i)
    {
        if(u == tempCache[i])
        {
            // abuse the valid flag to indicate we have seen it
            tempCache[i].set_valid(false);
            return;
        }
    }

    // not in our list? nuke it!
    u.set_valid(false);
    maintainerCache->set(u);
}

static uint8_t hexDigit2Int(char d)
{
    if(d >= '0' && d <= '9')
    {
        return d-'0';
    }

    if(d >= 'A' && d <= 'F')
    {
        return d-'A'+10;
    }

    if(d >= 'a' && d <= 'f')
    {
        return d-'a'+10;
    }

    return 0;
}

static Card str2Card(const char* str)
{
    int bytes = strlen(str)/2;

    uint8_t uid[7] = { 0 };

    for(int i=0; i < bytes; ++i)
    {
        uid[i] = (hexDigit2Int(str[2*i]) << 4 ) + (hexDigit2Int(str[(2*i)+1]));
    }

    return Card(uid, bytes == 7, true, true);
}

void MaintainerCacheMaintainer::doCheck()
{
    Serial.println("Checking maintainer cache");
    MaintainerListRecord* mlr = acserver->getToolMaintainers();

    if(!mlr)
    {
        Serial.println("Error while retrieving maintainer list");
        return;
    }

    if(mlr->isSuccess())
    {
        currentCacheCount = 0;
        Serial.print("Got ");
        Serial.print(mlr->count);
        Serial.println(" maintainers");

        tempCache = new Card[mlr->count];
        tempCacheCount = mlr->count;

        // first convert to the usual format
        for(uint8_t i = 0; i < mlr->count; ++i)
        {
            tempCache[i] = str2Card(mlr->maintainers[i]);
        }

        /*
            Right... So... This isn't hugely efficient but will work.
            The Cache API is a bit of a turd for this actually. For each card
            in the cache, we check it against our list. If it is in our list, we
            abuse the "valid" flag to mark it as processed. If not, we mark it as invalid
            in the maintainer cache. Anything then "valid" in our list is a new entry.
        */
        currentCacheMaintainer = this;
        maintainerCache->each(onCardCallback);
        currentCacheMaintainer = nullptr;
        for(uint8_t i = 0; i < tempCacheCount; ++i)
        {
            if(tempCache[i].is_valid())
            {
                maintainerCache->set(tempCache[i]);
            }
        }

        delete[] tempCache;
        tempCacheCount = 0;
    }
    else
    {
        Serial.print("Error getting maintainer list: ");
        if(mlr->error)
        {
            Serial.println(mlr->error);
        }
        else
        {
            Serial.println("Unknown Error");
        }
    }

    delete mlr;
}
