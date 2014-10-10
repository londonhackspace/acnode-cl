#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <Energia.h>
#include "user.h"

int get_url(char * path);
int querycard(user card);
bool networkCheckToolStatus();


#endif
