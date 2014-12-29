#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <Energia.h>
#include "user.h"

int get_url(char * path);
int querycard(user card);
bool networkCheckToolStatus();
int setToolStatus(int status, user card);
void addNewUser(user card, user maintainer);
int toolUse(int status, user card);
int toolUseTime(user card, int time);
int reportToolUse(user card, int status);

#endif
