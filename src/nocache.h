#ifndef _NOCACHE_H_
#define _NOCACHE_H_

#include "cache.h"

class NoCache: public Cache {
  public:
    NoCache();
    virtual void begin();
    virtual Card get(const Card u);
    virtual void set(const Card u);
    virtual void purge(void);
    virtual int each(void( *callback)(Card c));
    virtual void verify(void);
  private:
    void warn();
};

#endif
