#ifndef _NOCACHE_H_
#define _NOCACHE_H_

#include "cache.h"

class NoCache: public CacheBase {
  public:
    NoCache();
    virtual void begin();
    virtual Card get(const Card u);
    virtual void set(const Card u);
    virtual Card get(size_t n) override;
    virtual size_t count() override;
    virtual void purge(void);
    virtual int each(void( *callback)(Card c));
    virtual void verify(void);
  private:
    void warn();
};

#endif
