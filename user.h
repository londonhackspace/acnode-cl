#ifndef _USER_H_
#define _USER_H_

struct user {
  //  uint8_t state;
  /* state is :
   bit 0: 1 if maintainer, 0 if user
   bit 1: 1 if 4 byte uid, 7 otherwise
   bit 2: 1 if enabled, 0 if disabled.
   bit 3: 1 if end of listing.
   */

unsigned int maintainer: 
  1;
unsigned int uidlen: 
  1; // 1 if 7, otherwise 4
unsigned int status: 
  1; // 1 if enabled
unsigned int end: 
  1; // after the last valid uid 
unsigned int :
  4; // pad to a whole byte
  uint8_t uid[7];
};

#endif

