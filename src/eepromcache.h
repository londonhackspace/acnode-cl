#ifndef _EECACHE_H_
#define _EECACHE_H_

#include <stdint.h>

#include "cache.h"


class EEPromCache: public CacheBase {
  public:
    EEPromCache();

    virtual void begin();

    // the returned user must be freed by the caller
    virtual Card get(const Card u);
    virtual Card get(size_t n) override;
    virtual size_t count() override;
    virtual void set(const Card u);
    virtual void purge(void);
    virtual int each(void( *callback)(Card c));
    virtual void verify(void);

  private:
    void write_user(const user u, int address);
    int find_user(const Card u);
    int find_free(void);
    user card_to_struct(const Card u);
    Card struct_to_card(const user u);
};

#endif
