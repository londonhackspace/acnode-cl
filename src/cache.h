#ifndef _CACHE_H_
#define _CACHE_H_

#include "card.h"

struct user {
unsigned int maintainer :1; // 1 if maintainer
unsigned int uidlen     :1; // 1 if 7, otherwise 4
unsigned int status     :1; // 1 if enabled
unsigned int invalid    :1; // 0 if valid - by default the eeprom is set to 0xff
unsigned int end        :1; // 1 if after the last valid uid
unsigned int            :3; // pad to a whole byte
  uint8_t uid[7];
};

struct Cache {
  public:
    virtual ~Cache();

    virtual void begin() = 0;

    // look up a uid in the cache and return a user struct
    // the returned user must be freed by the caller
    virtual Card get(Card u) = 0;

    // add a card to the cache
    virtual void set(const Card u) = 0;

    // empty all cards from the cache
    virtual void purge(void) = 0;

    // iterate through the cache
    virtual int each(void( *callback)(Card u)) = 0;

    // fill up the cache with test entries
    void fill(void);
    void fill(int count);

    // list the contents of the cache
    void list(void);

    // verify each entry in the cache against the acserver
    virtual void verify(void) = 0;

  protected:
    char *_filename;

};

#endif
