#include "RealACServer.h"
#include "DataRecords.h"

#include "../utils.h"

#include <HttpClient.h>
#include <aJSON.h>
#include <stdio.h>

// TODO: remove this dependency so this is more testable
#include "Energia.h"

/////////////////////////////////////////
// Nicked from old network.cpp
// Please replace me with less messy code
/////////////////////////////////////////
#include "../acnode.h"
#define HTTP_TIMEOUT 3000
#define USER_AGENT "ACNode rev " STRINGIFY(GIT_REVISION)

// End old parts

ACServer::~ACServer() {}

RealACServer::RealACServer(Client& aClient, const char* server, uint16_t port, uint32_t toolId) :
    aClient(aClient),
    server(server),
    port(port),
    toolId(toolId)
{

}

RealACServer::~RealACServer()
{

}

aJsonObject* RealACServer::getRequest(const char* path)
{
    HttpClient http(aClient);
    http.setHttpWaitForDataDelay(200);
    http.setHttpResponseTimeout(HTTP_TIMEOUT);
    http.beginRequest();

    int ret = http.get(server, port, path, USER_AGENT);
    if(ret)
    {
        Serial.print("Error connecting to ");
        Serial.println(server);
        return nullptr;
    }

    if (strlen(acsettings.secret) > 0)
    {
        http.sendHeader("X-AC-Key", acsettings.secret);
    }
    // Put ACServer into the newer, more sane mode
    http.sendHeader("X-AC-JSON", "1");
    http.endRequest();

    int responseCode = http.responseStatusCode();
    if(responseCode < 200 || responseCode > 299)
    {
        Serial.print("Error: Response code was ");
        Serial.println(responseCode);
        return nullptr;
    }

    http.skipResponseHeaders();
    aJsonClientStream jsonStream(&http);
    return aJson.parse(&jsonStream);
}

CardRecord* RealACServer::queryCard(const char* uid)
{
    static const char* pathformat = "/%d/card/%s";
    char buffer[7+10+14+1]; // 7 chars of static content,
                            // 10 chars of tool id
                            // 14 chars of card ID
                            // 1 null terminator
    sprintf(buffer, pathformat, toolId, uid);
    
    aJsonObject* jsonObj = getRequest(buffer);

    if(!jsonObj)
    {
        Serial.println("Failed to get json");
        return nullptr;
    }

    CardRecord* retVal = new CardRecord();

    aJsonObject* numericValField = aJson.getObjectItem(jsonObj, "numeric_status");
    if(numericValField && numericValField->type == aJson_Int)
    {
        retVal->numericStatus = numericValField->valueint;
    }
    else
    {
        Serial.println("Failed to extract numeric_status");
    }

    aJsonObject* errorField = aJson.getObjectItem(jsonObj, "error");
    if(errorField && errorField->type == aJson_String)
    {
        retVal->error = new char[strlen(errorField->valuestring)];
        strcpy(retVal->error, errorField->valuestring);
    }

    aJsonObject* nameField = aJson.getObjectItem(jsonObj, "user_name");
    if(nameField && nameField->type == aJson_String)
    {
        retVal->error = new char[strlen(errorField->valuestring)];
        strcpy(retVal->error, errorField->valuestring);
    }
    
    aJsonObject* idField = aJson.getObjectItem(jsonObj, "user_id");
    if(idField && idField->type == aJson_Int)
    {
        retVal->userId = idField->valueint;
    }

    // important: clear up after ourself
    aJson.deleteItem(jsonObj);

    return retVal;
}
