#include "network.h"
#include "acnode.h"
#include "user.h"

int get_url(char * path) {
  int result = -1;

  Serial.print("Connecting to http://");
  Serial.print(acsettings.servername);
  Serial.print(":");
  Serial.println(acsettings.port);

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

int querycard(user card)
{
  char path[13 + 14 + 1];
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
  } else {
    Serial.println("access denied");
  }
  return result;
}

bool networkCheckToolStatus()
{
  char path[13 + 2];
  int result = -1;
  
  sprintf(path, "GET /%ld/status/", acsettings.nodeid);
  
  result = get_url(path);
  Serial.print("Status: ");
  Serial.println(result);
  
  if (result == 1) {
    return true;
  }

  return false;
}

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

