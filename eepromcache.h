#ifndef _EECACHE_H_
#define _EECACHE_H_

#include <stdint.h>

#include "cache.h"


class EEPromCache: public Cache {
  public:
    EEPromCache();

    virtual boolean get(uint8_t *key, user *u);
    virtual void set(const user *u);
    virtual void purge(void);
    virtual int each(void( *callback)(user *));
    virtual boolean compare(const uint8_t *k1, const uint8_t *k2);
    virtual void fill(void);
    virtual void list(void);
    virtual void verify(void);

};

#endif
