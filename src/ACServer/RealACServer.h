#ifndef _REALACSERVER_H
#define _REALACSERVER_H

#include "ACServer.h"

#include <stdint.h>

// forward declarations
typedef struct aJsonObject aJsonObject;
class Client;
class HttpClient;

/**
 * Concrete implementation of the ACServer interface
 */
class RealACServer : public ACServer
{
public:
    RealACServer(Client& aClient, const char* server, uint16_t port, uint32_t toolId);
    ~RealACServer();

    CardRecord* queryCard(const char* uid) override;
    StatusRecord* queryNodeStatus() override;
    ResultRecord* setToolStatus(uint8_t status, const char* cardUid) override;

private:
    HttpClient makeHttpClient();
    void sendHeaders(HttpClient& http);
    aJsonObject* getRequest(const char* path);
    aJsonObject* postRequest(const char* path);
    void handleCommon(BaseRecord* retVal, aJsonObject* jsonObj);
    void handleStringField(aJsonObject* jsonObj, const char* name, char** data);
    bool handleIntField(aJsonObject* jsonObj, const char* name, int* data);

    Client& aClient;
    const char* server;
    uint16_t port;
    uint32_t toolId;
};

#endif
