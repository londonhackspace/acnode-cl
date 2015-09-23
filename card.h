#ifndef _USER_H_
#define _USER_H_

#include <Energia.h>

#include <stdint.h>
#include <driverlib/eeprom.h>


class Card {
  public:
    Card(const uint8_t *uid, boolean uidlen, boolean status, boolean maintainer);
    Card();
    boolean operator==(const Card& other);
    boolean operator!=(const Card& other){return !(*this == other);}
    boolean compare_uid(const Card& other) const;
    // can this card use the tool?
    boolean is_user() const;
    // are they a maintainer?
    boolean is_maintainer() const;
    // set maintainer status
    void set_maintainer(boolean m);
    // set user status
    void set_user(boolean u);
    // true if its a 7 byte uid.
    boolean get_longuid() const;
    // fill *uid with the uid
    void get_uid(uint8_t *uid) const;
    // is it a valid card?
    boolean is_valid() const;
    // is it valid or not?
    void set_valid(boolean v);
    void dump(void) const;
    void str(char *str);
  private:
    boolean _uidlen;
    uint8_t _uid[7];
    boolean _maintainer;
    boolean _status;
    boolean _valid;
};

#endif

