#ifndef _REALACSERVER_H
#define _REALACSERVER_H

#include "ACServer.h"

#include <stdint.h>

// forward declarations
typedef struct aJsonObject aJsonObject;
class Client;

/**
 * Concrete implementation of the ACServer interface
 */
class RealACServer : public ACServer
{
public:
    RealACServer(Client& aClient, const char* server, uint16_t port, uint32_t toolId);
    ~RealACServer();

    CardRecord* queryCard(const char* uid) override;

private:
    aJsonObject* getRequest(const char* path);

    Client& aClient;
    const char* server;
    uint16_t port;
    uint32_t toolId;
};

#endif
