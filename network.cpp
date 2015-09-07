#include "network.h"
#include "acnode.h"
#include "user.h"

// both 'url' and 'path' here are misleading
// the string is actually "<method> <path>"
// e.g.:
//
// "GET /fish"
// "POST /wibble?foo=1"
//
int get_url(char * path) {
  int result = -1;

  Serial.print("Connecting to http://");
  Serial.print(acsettings.servername);
  Serial.print(":");
  Serial.print(acsettings.port);
  Serial.print(" // ");
  Serial.println(path);

  if (client.connect(acsettings.servername, acsettings.port)) {
    Serial.println("Connected");
    Serial.println("Querying");

    Serial.println(path);
    client.print(path);
    client.println(" HTTP/1.0");
    client.print("Host: ");
    client.println(acsettings.servername);
    client.println();

    int timeout = 0;

    while (!client.available()) {
       delay(25);
       timeout++;
       if (timeout > 400) {
         Serial.println("Timeout :(");
         break;
       }
    }

    if (client.available()) {
      char c;
      Serial.print("Got Response: >");

      int newlines = 0;
      boolean first = false;

      while (client.available()) {
        c = client.read();
        Serial.print(c);
        if (c == '\n') {
          Serial.print('\r');
          newlines += 1;
        } else {
          if (c != '\r') {
            newlines = 0;
          }
        }
        // we only want the 1st char returned
        if (first) {
          if (isdigit(c)) {
            result = c - '0';
          }
          first = false;
        }
        if (newlines == 2) {
          first = true;
        }
      }

      Serial.println("<");
      Serial.println("Disconnecting");
      client.flush();
      client.stop();
    }
    Serial.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  return result;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Get_card_permissions
int querycard(user card)
{
  char path[11 + 10 + 14 + 1];
  int result = -1;

  if (card.invalid) {
    return -1;
  }
  
  sprintf(path, "GET /%ld/card/", acsettings.nodeid);

  int len = card.uidlen ? 7 : 4;

  for(byte i=0; i < len; i++) {
    sprintf(path + strlen(path), "%02X", card.uid[i]);
  }

  result = get_url(path);

  // if it's 2 it's a maintainer.
  if (result == 1 || result == 2) {
    Serial.println("access granted");
  } else if (result == 0){
    Serial.println("access denied");
  } else {
    Serial.println("Network or acserver error");
  }
  return result;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Check_tool_status
int networkCheckToolStatus()
{
  char path[13 + 10 + 1];
  int result = -1;
  
  sprintf(path, "GET /%ld/status/", acsettings.nodeid);
  
  result = get_url(path);
  Serial.print("Status: ");
  Serial.println(result);
  
  return result;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Report_tool_status
int setToolStatus(int status, user card) {
  int ret = -1;
  
  Serial.println("Setting tool status:");
  // /[nodeID]/status/[new_status]/by/[cardWithAdminPermissions]
  char url[64];
  sprintf(url, "POST /%ld/status/%d/by/", acsettings.nodeid, status);
  uid_str(url + strlen(url), &card);
  Serial.println(url);

  ret = get_url(url);
  Serial.print("Got: ");
  Serial.println(ret);
  
  return ret;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Add_card
void addNewUser(user card, user maintainer)
{
  int ret;

  Serial.println("Adding card:");
  // /<nodeid>/grant-to-card/<trainee card uid>/by-card/<maintainer card uid>
  char url[64];
  sprintf(url, "POST /%ld/grant-to-card/", acsettings.nodeid);
  uid_str(url + strlen(url), &card);
  sprintf(url + strlen(url), "/by-card/");
  uid_str(url + strlen(url), &maintainer);
  Serial.println(url);

  ret = get_url(url);
  Serial.print("Got: ");
  Serial.println(ret);
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Tool_usage_.28live.29
// status: 1 == in use, 0 == out of use.
int toolUse(int status, user card) {
  int ret = -1;

  Serial.println("Updating tool Usage:");
  //  /[nodeID]/tooluse/[status]/[cardID]
  char url[64];
  sprintf(url, "POST /%ld/tooluse/%d/", acsettings.nodeid, status);
  uid_str(url + strlen(url), &card);
  Serial.println(url);

  ret = get_url(url);
  Serial.print("Got: ");
  Serial.println(ret);
  
  return ret;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Tool_usage_.28usage_time.29
// is the time here in ms or Seconds?
int toolUseTime(user card, int time) {
  int ret = -1;

  Serial.println("Setting tool status:");
  // /[nodeID]/tooluse/time/for/[cardID]/[timeUsed]
  char url[64];
  sprintf(url, "POST /%ld/tooluse/time/for/", acsettings.nodeid);
  uid_str(url + strlen(url), &card);
  sprintf(url + strlen(url), "/%d", time);
  
  Serial.println(url);

  ret = get_url(url);
  Serial.print("Got: ");
  Serial.println(ret);
  
  return ret;
}

int reportToolUse(user card, int status) {
  int ret = -1;

  Serial.println("Setting tool usage:");
  // /[nodeID]/tooluse/[status]/[cardID]

  char url[64];
  sprintf(url, "POST /%ld/tooluse/%d/", acsettings.nodeid, status);
  uid_str(url + strlen(url), &card);

  Serial.println(url);

  ret = get_url(url);
  Serial.print("Got: ");
  Serial.println(ret);

  return ret;
}

void settime(DateTime *dt) {
  if (client.connect(acsettings.servername, acsettings.port)) {
    client.println("GET / HTTP/1.0");
    client.println();
    client.flush();
    
    int timeout = 0;
    while (!client.available()) {
       delay(25);
       timeout++;
       if (timeout > 400) {
         Serial.println("Timeout :(");
         break;
       }
    }
    
    char lineBuffer[128];
    char dateStr[30];
    int i = 0;
    while (client.available()) {
      char c = client.read();
      if (c == '\n' || c == '\r') {
        if (strstr(lineBuffer, "Date:")) {
          memset(dateStr, 0x0, 30);
          strncpy(dateStr, lineBuffer + 6, 29);
          Serial.println("Date received from server: ");
          Serial.println(dateStr);
          break;
        }
        
        memset(lineBuffer, 0x0, 128);
        i = 0;
      } else {
        lineBuffer[i] = c;
        i++;
      }
    }
    
    tm epoch;
    // Mon, 07 Sep 2015 22:50:40 GMT
    if (convertString2DateTime(dateStr, "%a, %d %b %Y %T %Z", epoch)) {
      Serial.println("Time set.");
      dt->setTime(epoch);
    }
  }
}

