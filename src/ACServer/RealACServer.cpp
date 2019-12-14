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

HttpClient RealACServer::makeHttpClient()
{
    HttpClient http(aClient);
    http.setHttpWaitForDataDelay(200);
    http.setHttpResponseTimeout(HTTP_TIMEOUT);
    http.beginRequest();
    return http;
}

void RealACServer::sendHeaders(HttpClient& http)
{
    if (strlen(acsettings.secret) > 0)
    {
        http.sendHeader("X-AC-Key", acsettings.secret);
    }
    // Put ACServer into the newer, more sane mode
    http.sendHeader("X-AC-JSON", "1");
}

aJsonObject* RealACServer::getRequest(const char* path)
{
    HttpClient http = makeHttpClient();

    int ret = http.get(server, port, path, USER_AGENT);
    if(ret)
    {
        Serial.print("Error connecting to ");
        Serial.println(server);
        return nullptr;
    }

    sendHeaders(http);
    
    int start = millis();
    http.endRequest();

    int responseCode = http.responseStatusCode();
    int diff = millis() - start;
    Serial.print("HTTP result in ");
    Serial.print(diff);
    Serial.println("ms");
    if(responseCode < 200 || responseCode > 299)
    {
        Serial.print("Error: Response code was ");
        Serial.println(responseCode);
        return nullptr;
    }

    http.skipResponseHeaders();
    aJsonClientStream jsonStream(&http);
    aJsonObject* retJson = aJson.parse(&jsonStream);
    http.stop();
    return retJson;
}

aJsonObject* RealACServer::postRequest(const char* path)
{
    HttpClient http = makeHttpClient();

    int ret = http.post(server, port, path, USER_AGENT);
    if(ret)
    {
        Serial.print("Error connecting to ");
        Serial.println(server);
        return nullptr;
    }

    sendHeaders(http);
    
    int start = millis();
    http.endRequest();

    int responseCode = http.responseStatusCode();
    int diff = millis() - start;
    Serial.print("HTTP result in ");
    Serial.print(diff);
    Serial.println("ms");
    if(responseCode < 200 || responseCode > 299)
    {
        Serial.print("Error getting path: ");
        Serial.println(path);
        Serial.print("Error: Response code was ");
        Serial.println(responseCode);
        return nullptr;
    }

    http.skipResponseHeaders();
    aJsonClientStream jsonStream(&http);
    aJsonObject* retJson = aJson.parse(&jsonStream);
    http.stop();
    return retJson;
}

void RealACServer::handleCommon(BaseRecord* retVal, aJsonObject* jsonObj)
{
    if(!handleIntField(jsonObj, "numeric_status", &retVal->numericStatus))
    {
        Serial.println("Failed to extract numeric_status");
    }

    handleStringField(jsonObj, "error", &retVal->error);
    if(retVal->error)
    {
        Serial.print("Error field: ");
        Serial.println(retVal->error);
    }

    Serial.print("Numeric Status is ");
    Serial.println(retVal->numericStatus);
}

void RealACServer::handleStringField(aJsonObject* jsonObj, const char* name, char** data)
{
    aJsonObject* field = aJson.getObjectItem(jsonObj, name);
    if(field && field->type == aJson_String)
    {
        *data = new char[strlen(field->valuestring)+1];
        strcpy(*data, field->valuestring);
    }
}

bool RealACServer::handleIntField(aJsonObject* jsonObj, const char* name, int* data)
{
    aJsonObject* field = aJson.getObjectItem(jsonObj, name);
    if(field && field->type == aJson_Int)
    {
        *data = field->valueint;
        return true;
    }
    return false;
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

    handleCommon(retVal, jsonObj);
    handleStringField(jsonObj, "user_name", &retVal->userName);
    handleIntField(jsonObj, "user_id", &retVal->userId);
    // important: clear up after ourself
    aJson.deleteItem(jsonObj);
    Serial.print("queryCard returning ");
    Serial.println(retVal->numericStatus);

    return retVal;
}

StatusRecord* RealACServer::queryNodeStatus()
{
    static const char* pathformat = "/%d/status/";
    char buffer[9+10+1];    // 9 chars of static content,
                            // 10 chars of tool id
                            // 1 null terminator
    sprintf(buffer, pathformat, toolId);
    
    aJsonObject* jsonObj = getRequest(buffer);

    if(!jsonObj)
    {
        Serial.println("Failed to get json");
        return nullptr;
    }

    StatusRecord* retVal = new StatusRecord();

    handleCommon(retVal, jsonObj);

    handleStringField(jsonObj, "status", &retVal->status);
    handleStringField(jsonObj, "status_message", &retVal->statusMessage);

    // important: clear up after ourself
    aJson.deleteItem(jsonObj);

    return retVal;
}

ResultRecord* RealACServer::sendAndReceivePost(const char* url)
{
    aJsonObject* jsonObj = postRequest(url);

    if(!jsonObj)
    {
        Serial.println("Failed to get json");
        return nullptr;
    }

    ResultRecord* retVal = new ResultRecord();

    handleCommon(retVal, jsonObj);

    handleStringField(jsonObj, "success", &retVal->successMesage);

    // important: clear up after ourself
    aJson.deleteItem(jsonObj);

    return retVal;
}

ResultRecord* RealACServer::setToolStatus(uint8_t status, const char* cardUid)
{
    static const char* pathformat = "/%d/status/%d/by/%s";
    char buffer[9+10+3+14+1];   // 9 chars of static content,
                                // 10 chars of tool id
                                // 3 digits of status
                                // 14 digits of card uid
                                // 1 null terminator
    sprintf(buffer, pathformat, toolId, status, cardUid);

    return sendAndReceivePost(buffer);
}

ResultRecord* RealACServer::reportToolUseTime(int time, const char* cardUid)
{
    static const char* pathformat = "/%d/tooluse/time/for/%s/%d";
    char buffer[20+10+14+10+1]; // 20 chars of static content,
                                // 10 chars of tool id
                                // 14 digits of card uid
                                // 10 of time (32-bit int)
                                // 1 null terminator
    sprintf(buffer, pathformat, toolId, cardUid, time);

    return sendAndReceivePost(buffer);
}

ResultRecord* RealACServer::reportToolUse(const char* cardUid, uint8_t state)
{
    static const char* pathformat = "/%d/tooluse/%d/%s";
    char buffer[11+10+3+14+1];  // 11 chars of static content,
                                // 10 chars of tool id
                                // 3 digits of state
                                // 14 digits of card uid
                                // 1 null terminator
    sprintf(buffer, pathformat, toolId, state, cardUid);

    return sendAndReceivePost(buffer);
}

ResultRecord* RealACServer::addNewUser(const char* maintainerUid, const char* userUid)
{
    static const char* pathformat = "/%d/grant-to-card/%s/by-card/%s";
    char buffer[11+10+14+14+1];  // 25 chars of static content,
                                // 10 chars of tool id
                                // 14 digits of card uid
                                // 14 digits of card uid
                                // 1 null terminator
    sprintf(buffer, pathformat, toolId, userUid, maintainerUid);

    return sendAndReceivePost(buffer);
}

MaintainerListRecord* RealACServer::getToolMaintainers()
{
    static const char* pathformat = "/%d/maintainers";
    char buffer[14+10+1];   // 14 chars of static content,
                            // 10 chars of tool id
                            // 1 null terminator
    sprintf(buffer, pathformat, toolId);

    aJsonObject* jsonObj = getRequest(buffer);

    if(!jsonObj)
    {
        Serial.println("Failed to get json");
        return nullptr;
    }

    MaintainerListRecord* retVal = new MaintainerListRecord();

    handleStringField(jsonObj, "error", &retVal->error);
    if(retVal->error)
    {
        Serial.print("Error field: ");
        Serial.println(retVal->error);

        // important: clear up after ourself
        aJson.deleteItem(jsonObj);
        retVal->success = false;
        return retVal;
    }

    if(jsonObj->type != aJson_Array)
    {
        Serial.println("Error: Data not in expected format");

        // important: clear up after ourself
        aJson.deleteItem(jsonObj);
        delete retVal;
        return nullptr;
    }

    unsigned char count = aJson.getArraySize(jsonObj);
    int outCursor = 0;
    for(unsigned char i = 0; i < count; ++i)
    {
        aJsonObject* entry = aJson.getArrayItem(jsonObj, i);
        if(entry->type != aJson_String)
        {
            Serial.println("Skipping maintainer entry - not a string!");
            continue;
        }
        strcpy(retVal->maintainers[outCursor++], entry->valuestring);
    }

    Serial.print("Got maintainers: ");
    Serial.println(outCursor);
    retVal->success = true;
    retVal->count = outCursor;

    // important: clear up after ourself
    aJson.deleteItem(jsonObj);

    return retVal;
}
