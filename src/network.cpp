#include "network.h"
#include "acnode.h"
#include "version.h"

// both 'url' and 'path' here are misleading
// the string is actually "<method> <path>"
// e.g.:
//
// "GET /fish"
// "POST /wibble?foo=1"
//
int get_url(char * path) {
  int result = -1;
  char outb[160];
  char ret[256];
  int retp = 0;

  ret[0] = '\0';

  // just incase there is any left over data.
  client.flush();
  client.stop();

  Serial.print("Connecting to http://");
  Serial.print(acsettings.servername);
  Serial.print(":");
  Serial.print(acsettings.port);
  Serial.print(" // ");
  Serial.print(path);

  if (acsettings.netverbose) { Serial.println(); }

  if (client.connect(acsettings.servername, acsettings.port)) {
    strcpy(outb, path);
    strcpy(outb + strlen(outb), " HTTP/1.0");
    strcpy(outb + strlen(outb), "\r\n");
    strcpy(outb + strlen(outb), "Host: ");
    strcpy(outb + strlen(outb), acsettings.servername);
    strcpy(outb + strlen(outb), "\r\n");
    strcpy(outb + strlen(outb), "User-Agent: ACNode ");
    strcpy(outb + strlen(outb), ACVERSION);
    strcpy(outb + strlen(outb), ", Energia ");
    sprintf(outb + strlen(outb), "%d", ENERGIA);
    strcpy(outb + strlen(outb), "\r\n");
    if (isprint(acsettings.secret[0])) {
      sprintf(outb + strlen(outb), "X-AC-Key: %s\r\n", acsettings.secret);
    }
    strcpy(outb + strlen(outb), "\r\n");

    client.print(outb);

    if (acsettings.netverbose) {
      Serial.println(outb);
    }

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
      int c;
      if (acsettings.netverbose) {
        Serial.println("Got Response:");
        Serial.print(">");
      }

      int newlines = 0;
      boolean first = false;

      while (client.connected() || client.available()) {
        c = client.read();
        // chars are actually unsigned?
        if (c == -1 || c == 0xff) {
          continue;
        }
        if (!isprint(c) && c != 0xa && c != 0xd) {
          Serial.print("Got a bogus c: ");
          Serial.println(c, HEX);
        }
        if (acsettings.netverbose) {
          Serial.print((char)c);
        }
        ret[retp++] = (char)c;
        ret[retp] = '\0';
        if (retp + 1 > 256) {
          retp = 0;
        }
        if (c == '\n') {
          if (acsettings.netverbose) { Serial.print('\r'); }
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
      if (acsettings.netverbose) {
        Serial.println("<");
      }
      client.flush();
      client.stop();
    }
    if (acsettings.netverbose) { Serial.println(); }
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

  if (acsettings.netverbose) {
     Serial.print("Result");
  }
  Serial.print(" : ");
  Serial.println(result);

  if (result == -1) {
    Serial.println(ret);
  }

  return result;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Get_card_permissions
int querycard(Card card)
{
  char path[11 + 10 + 14 + 1];
  int result = -1;

  sprintf(path, "GET /%d/card/", acsettings.nodeid);

  card.str(path + strlen(path));

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
  
  sprintf(path, "GET /%d/status/", acsettings.nodeid);
  
  result = get_url(path);
  
  return result;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Report_tool_status
int setToolStatus(int status, Card card) {
  int ret = -1;
  
  Serial.println("Setting tool status:");
  // /[nodeID]/status/[new_status]/by/[cardWithAdminPermissions]
  char url[64];
  sprintf(url, "POST /%d/status/%d/by/", acsettings.nodeid, status);
  card.str(url + strlen(url));

  ret = get_url(url);
  return ret;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Add_card
void addNewUser(Card card, Card maintainer)
{
  char url[64];

  Serial.println("Adding card:");

  // /<nodeid>/grant-to-card/<trainee card uid>/by-card/<maintainer card uid>
  sprintf(url, "POST /%d/grant-to-card/", acsettings.nodeid);
  card.str(url + strlen(url));
  sprintf(url + strlen(url), "/by-card/");
  maintainer.str(url + strlen(url));

  get_url(url);
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Tool_usage_.28usage_time.29
// is the time here in ms or Seconds?
int toolUseTime(Card card, int time) {
  int ret = -1;

  Serial.println("Setting tool status:");
  // /[nodeID]/tooluse/time/for/[cardID]/[timeUsed]
  char url[64];
  sprintf(url, "POST /%d/tooluse/time/for/", acsettings.nodeid);
  card.str(url + strlen(url));
  sprintf(url + strlen(url), "/%d", time);

  ret = get_url(url);
  return ret;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Tool_usage_.28live.29
// status: 1 == in use, 0 == out of use.
int reportToolUse(Card card, int status) {
  int ret = -1;

  Serial.println("Setting tool usage:");
  // /[nodeID]/tooluse/[status]/[cardID]

  char url[64];
  sprintf(url, "POST /%d/tooluse/%d/", acsettings.nodeid, status);
  card.str(url + strlen(url));

  ret = get_url(url);
  return ret;
}

