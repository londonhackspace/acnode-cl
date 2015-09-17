#ifndef _EECACHE_H_
#define _EECACHE_H_

#include <stdint.h>

#include "cache.h"


class EEPromCache: public Cache {
  public:
    EEPromCache();

    // the returned user must be freed by the caller
    virtual Card get(const Card u);
    virtual void set(const Card u);
    virtual void purge(void);
    virtual int each(void( *callback)(user *));
//    virtual boolean compare(const uint8_t *k1, const uint8_t *k2);
    virtual void fill(void);
    virtual void list(void);
    virtual void verify(void);

  private:
    void write_user(const user u, int address);
    int find_user(const Card u);
    int find_free(void);
    user card_to_struct(const Card u);
    Card struct_to_card(const user u);
};

#endif
