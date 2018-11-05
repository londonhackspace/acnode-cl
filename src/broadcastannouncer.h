#ifndef _BROADCASTANNOUNCER_H_
#define _BROADCASTANNOUNCER_H_

#include "announcer.h"

#include <EthernetUdp.h>

// Only one instance is permitted.

class BroadcastAnnouncer : public Announcer{
public:

    EthernetUDP _udp;
    IPAddress _host;
    int _port;

    BroadcastAnnouncer(int);
    void RFID(char *cardId, int granted) override;
    void START() override;
    void BELL() override;
    void EXIT() override;
    void WEDGED() override;
    void ALIVE() override;
};

#endif
