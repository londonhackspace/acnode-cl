#include "network.h"
#include "acnode.h"

int get_url(char * path) {
  int result = -1;
  boolean first = true;

  Serial.print("Connecting to http://");
  Serial.print(acsettings.servername);
  Serial.print(":");
  Serial.println(acsettings.port);

  if (client.connect(acsettings.servername, acsettings.port)) {
    Serial.println("Connected");
    Serial.println("Querying");

    Serial.println(path);
    client.println(path);
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

      while (client.available()) {
        c = client.read();
        Serial.print(c);
        if (c == '\n') {
          Serial.print('\r');
        }
        // we only want the 1st char returned
        if (first) {
          if (isdigit(c)) {
            result = c - '0';
          }
          first = false;
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

