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


user *get_user(user *u);
boolean compare_user(user *u1, user *u2);
boolean compare_uid(user *u1, user *u2);

void dump_user(const user * u);
void uid_str(char *str, user *u);


  
#endif

