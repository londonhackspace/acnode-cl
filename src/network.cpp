#include "network.h"
#include "acnode.h"
#include "version.h"

#define HTTP_TIMEOUT 3000

namespace networking {
  const char *user_agent() {
    static char user_agent[64];
    sprintf(user_agent, "ACNode rev %s", GIT_REVISION);
    return user_agent;
  }

  void log(int l) {
#ifdef LOGGING
    Serial.print("> ");
    Serial.println(l);
#endif
  }

  void log(uint8_t *data) {
#ifdef LOGGING
  Serial.print("> ");
  Serial.println((char*)data);
#endif
  }

  void log(char *d1, char *d2) {
#ifdef LOGGING
    Serial.print("< ");
    Serial.print(d1);
    Serial.println(d2);
#endif
  }

int handle_response(int success, HttpClient &http) {
  if (strlen(acsettings.secret) > 0) {
    http.sendHeader("X-AC-Key", acsettings.secret);
  }
  http.endRequest();
  wdog.feed();
  int result = -99;
  if (success == HTTP_SUCCESS) {
    if (http.responseStatusCode() == 200) {
      if (http.available()) {
        http.skipResponseHeaders();
        wdog.feed();

        uint8_t buffer[16];
        http.read(buffer, 16);
        log(buffer);
        result = strtol((const char *)buffer, NULL, 10);

        if (result < 0) {
          result = (-result) & 0xff;
          result = -result;
        }
      } else {
        result = -98;
      }
    } else {
      result = -http.responseStatusCode();
    }
  }
  return result;
}

int get_url(char *path) {
    int result;
    HttpClient http(client);
    http.setHttpWaitForDataDelay(200);
    http.setHttpResponseTimeout(HTTP_TIMEOUT);
    http.beginRequest();

    log("GET ", path);
    wdog.feed();
    result = handle_response(http.get(acsettings.servername, acsettings.port, path, user_agent()), http);
    http.stop();
    log(result);
    return result;
}

int post_url(char *path) {
  int result;
  HttpClient http(client);
  http.setHttpWaitForDataDelay(200);
  http.setHttpResponseTimeout(HTTP_TIMEOUT);
  http.beginRequest();

  log("POST ", path);
  wdog.feed();
  result = handle_response(http.post(acsettings.servername, acsettings.port, path, user_agent()), http);
  http.stop();
  log(result);
  return result;
}



// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Get_card_permissions
int querycard(Card card)
{
  char path[11 + 10 + 14 + 1];
  int result = -100;
  sprintf(path, "/%d/card/", acsettings.nodeid);
  card.str(path + strlen(path));

  result = get_url(path);

  Serial.print("acserver said: ");
  Serial.println(result);

  return result;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Check_tool_status
int networkCheckToolStatus()
{
  char path[13 + 10 + 1];
  int result = -1;
  sprintf(path, "/%d/status/", acsettings.nodeid);

  result = get_url(path);

  return result;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Report_tool_status
int setToolStatus(int status, Card card) {
  int ret = -1;

  Serial.println("Setting tool status:");
  // /[nodeID]/status/[new_status]/by/[cardWithAdminPermissions]
  char url[64];
  sprintf(url, "/%d/status/%d/by/", acsettings.nodeid, status);
  card.str(url + strlen(url));

  ret = post_url(url);
  return ret;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Add_card
void addNewUser(Card card, Card maintainer)
{
  char url[64];

  Serial.println("Adding card:");

  // /<nodeid>/grant-to-card/<trainee card uid>/by-card/<maintainer card uid>
  sprintf(url, "/%d/grant-to-card/", acsettings.nodeid);
  card.str(url + strlen(url));
  sprintf(url + strlen(url), "/by-card/");
  maintainer.str(url + strlen(url));

  post_url(url);
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Tool_usage_.28usage_time.29
// Time reported is in SECONDS.
int toolUseTime(Card card, int time) {
  int ret = -1;

  Serial.println("Setting tool status:");
  // /[nodeID]/tooluse/time/for/[cardID]/[timeUsed]
  char url[64];
  sprintf(url, "/%d/tooluse/time/for/", acsettings.nodeid);
  card.str(url + strlen(url));
  sprintf(url + strlen(url), "/%d", time);

  ret = post_url(url);
  return ret;
}

// https://wiki.london.hackspace.org.uk/view/Project:Tool_Access_Control/Solexious_Proposal#Tool_usage_.28live.29
// status: 1 == in use, 0 == out of use.
int reportToolUse(Card card, int status) {
  int ret = -1;

  Serial.println("Setting tool usage:");
  // /[nodeID]/tooluse/[status]/[cardID]

  char url[64];
  sprintf(url, "/%d/tooluse/%d/", acsettings.nodeid, status);
  card.str(url + strlen(url));

  ret = post_url(url);
  return ret;
}

}
