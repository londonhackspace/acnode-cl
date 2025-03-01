#include "network.h"
#include "acnode.h"
#include "version.h"
#include "utils.h"
#include "ACServer/RealACServer.h"
#include "ACServer/DataRecords.h"

#define HTTP_TIMEOUT 3000
#define USER_AGENT "ACNode rev " STRINGIFY(GIT_REVISION)

namespace networking {
  const char *user_agent() {
    return USER_AGENT;
  }

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Get_card_permissions
int querycard(Card card)
{
  RealACServer acs(client, acsettings.servername, acsettings.port, acsettings.nodeid);
  char cardId[15];
  card.str(cardId);
  CardRecord* cr = acs.queryCard(cardId);

  int result = -1;
  if(cr)
  {
    result = cr->numericStatus;
    delete cr;
  }

  Serial.print("acserver said: ");
  Serial.println(result);

  return result;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Check_tool_status
int networkCheckToolStatus()
{
  int result = -1;
  RealACServer acs(client, acsettings.servername, acsettings.port, acsettings.nodeid);
  StatusRecord* sr = acs.queryNodeStatus();
  
  if(sr)
  {
    result = sr->numericStatus;
    delete sr;
  }

  return result;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Report_tool_status
int setToolStatus(int status, Card card) {
  int ret = -1;

  RealACServer acs(client, acsettings.servername, acsettings.port, acsettings.nodeid);
  
  char cardId[15];
  card.str(cardId);

  ResultRecord* rr = acs.setToolStatus(status, cardId);
  
  if(rr)
  {
    ret = rr->numericStatus;
    delete rr;
  }  

  return ret;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Report_door_access
int doorEvent(int status, Card card) {
  int ret = -1;

  RealACServer acs(client, acsettings.servername, acsettings.port, acsettings.nodeid);
  
  char cardId[15];
  card.str(cardId);

  ResultRecord* rr = acs.doorEvent(status, cardId);
  
  if(rr)
  {
    ret = rr->numericStatus;
    delete rr;
  }  

  return ret;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Add_card
void addNewUser(Card card, Card maintainer)
{
  Serial.println("Adding card:");
  RealACServer acs(client, acsettings.servername, acsettings.port, acsettings.nodeid);

  char userId[15];
  card.str(userId);

  char maintainerId[15];
  maintainer.str(maintainerId);

  ResultRecord* rr = acs.addNewUser(maintainerId, userId);

  if(rr)
  {
    if(rr->isSuccess())
    {
      Serial.println("Added user");
    }
    else if(rr->error)
    {
      Serial.print("Error: Failed to add user: ");
      Serial.println(rr->error);
    }
    delete rr;
  }
  else
  {
    Serial.println("Error: Failed to add user: Unknown error");
  }
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Tool_usage_.28usage_time.29
// Time reported is in SECONDS.
int toolUseTime(Card card, int time) {
  int ret = -1;

  RealACServer acs(client, acsettings.servername, acsettings.port, acsettings.nodeid);
  
  char cardId[15];
  card.str(cardId);

  ResultRecord* rr = acs.reportToolUseTime(time, cardId);

  if(rr)
  {
    ret = rr->numericStatus;
    delete rr;
  }
  
  return ret;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Tool_usage_.28live.29
// status: 1 == in use, 0 == out of use.
int reportToolUse(Card card, int status) {
  int ret = -1;

  RealACServer acs(client, acsettings.servername, acsettings.port, acsettings.nodeid);
  char cardId[15];
  card.str(cardId);

  Serial.println("Setting tool usage:");
  ResultRecord* rr = acs.reportToolUse(cardId, status);

  if(rr)
  {
    ret = rr->numericStatus;
    delete rr;
  }

  return ret;
}

bool have_valid_ip() {
  IPAddress addr = Ethernet.localIP();
  if(addr == INADDR_NONE) {
    return false;
  }

  if(addr[0] == 169 && addr[1] == 254) {
    return false;
  }

  return true;
}

}
