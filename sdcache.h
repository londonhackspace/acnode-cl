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
  
    virtual boolean get(uint8_t *key, user *u);
    virtual void set(uint8_t *key, user *u);
    virtual void purge(void);
    virtual int each(void( *callback)(user *));
    virtual boolean compare(uint8_t *k1, uint8_t *k2);
};

#endif
