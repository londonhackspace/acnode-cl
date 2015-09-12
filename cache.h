#ifndef _CACHE_H_
#define _CACHE_H_

#include "user.h"

struct Cache {
  public:
//    virtual Cache(char *filename) = 0;
//    virtual Cache() = 0;
  
    virtual boolean get(uint8_t *key, user *u) = 0;
    virtual void set(uint8_t *key, user *u) = 0;
    virtual void purge(void) = 0;
    virtual int each(void( *callback)(user *)) = 0;
    virtual boolean compare(uint8_t *k1, uint8_t *k2) = 0;

  protected:
    char *_filename;
  
};

#endif
