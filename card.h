#ifndef _USER_H_
#define _USER_H_

#include <Energia.h>

#include <stdint.h>
#include <driverlib/eeprom.h>

struct user {
unsigned int maintainer :1; // 1 if maintainer
unsigned int uidlen     :1; // 1 if 7, otherwise 4
unsigned int status     :1; // 1 if enabled
unsigned int invalid    :1; // 0 if valid - by default the eeprom is set to 0xff
unsigned int end        :1; // 1 if after the last valid uid 
unsigned int            :3; // pad to a whole byte
  uint8_t uid[7];
};

class Card {
  public:
    Card(const uint8_t *uid, boolean uidlen, boolean status, boolean maintainer);
    Card(struct user *u);
    Card();
    // copy
    // operator =
    boolean operator==(const Card& other);
    boolean operator!=(const Card& other){return !(*this == other);}
    boolean compare_uid(const Card& other);
    void dump(void);
    void str(char *str);
  private:
    int _uidlen;
    uint8_t _uid[7];
    boolean _maintainer;
    boolean _status;
};

// cache->get();
//user *get_user(user *u);
boolean compare_user(user *u1, user *u2);
boolean compare_uid(user *u1, user *u2);

void dump_user(const user * u);
void uid_str(char *str, user *u);

  
#endif

