#include "syslog.h"

// Constructor
Syslog::Syslog() {
  _server = NULL;
  _ident = NULL;
  _fac = LOG_USER;
  online = true;
}

// for checking that we can find the server name
void Syslog::do_dns(const char *name, struct ip_addr *ipaddr, void *arg)
{
  ip_addr_t *result = (ip_addr_t *)arg;

  /* BEWARE: lwip stack has been modified to set ipaddr
   * to IPADDR_NONE if the lookup failed */
  result->addr = ipaddr->addr;
}

// set server, our ident, and the facility to use.
void Syslog::begin(const char *server, const char * ident, int fac) {
  _server = server;
  _fac = fac;
  _ident = ident;

  ip_addr_t ip;
  ip.addr = 0;
  // 2 seconds
  int timeout = 200;

  if (!online)
   return;

  dns_gethostbyname(_server, &ip, do_dns, &ip);

  // in Udp.beginPacket there is no timeout
  // so if we don't check this here then we will
  // hang if we cant find a server to talk to.
  while(!ip.addr) {
    delay(10);
    timeout--;
    if (timeout < 0)
     break;
  }

  if(ip.addr == IPADDR_NONE) {
    Serial.print("unable to find ip for: ");
    Serial.println(_server);

    _server = NULL;
    return;
  }
  Udp.begin(1234);
}

// in case we need to change facility.
void Syslog::setFacility(int fac) {
  _fac = fac;
}

void Syslog::offline() {
  online = false;
}

// send a message with a priority
void Syslog::syslog(int pri, const char *message) {
  // XXX test not very good here.
  if (strlen(message) > (SYSLOGBUFFERSIZE - 6)) {
     Serial.print("log message too long: ");
     Serial.println(message);
     return;
  }

  snprintf(packetBuffer, SYSLOGBUFFERSIZE, "<%d>%s %s", LOG_MAKEPRI(_fac, pri), _ident, message);
  Serial.println(packetBuffer);

  if (_server == NULL) {
     Serial.println("syslog server not set");
     return;
  }

  if (!online)
   return;

  Udp.beginPacket(_server, 514);
  Udp.write(packetBuffer);
  Udp.endPacket();
}
