#ifndef _SDCACHE_H_
#define _SDCACHE_H_

#include <SPI.h>
#include <SD.h>

#include "cache.h"

/* see:
 *
 * http://jasiek.me/2015/08/28/using-an-sd-card-with-a-ti-tiva-c-tm4129.html
 * 
 * for hardware instructions
 */

class SDCache: public Cache {
  protected:
    char *_filename; // should be const, but SD lib needs updating as well.

  public:
    SDCache(char *filename);
    SDCache();

    // caller needs to free returned user
    virtual user *get(user *u);
    virtual void set(const user *u);
    virtual void purge(void);
    virtual int each(void( *callback)(user *));
    virtual boolean compare(const uint8_t *k1, const uint8_t *k2);
    virtual void fill(void);
    virtual void list(void);
    virtual void verify(void);

};

#endif
