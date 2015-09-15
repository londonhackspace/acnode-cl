#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <Energia.h>
#include "card.h"
#include <DateTimeLibrary.h>

int get_url(char * path);
int querycard(user card);
int networkCheckToolStatus();
int setToolStatus(int status, user card);
void addNewUser(user card, user maintainer);
int toolUseTime(user card, int time);
int reportToolUse(user card, int status);
void settime(DateTime *);

#endif
