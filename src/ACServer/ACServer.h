#ifndef _ACSERVER_H
#define _ACSERVER_H

// Forward declarations
struct BaseRecord;
struct CardRecord;
struct StatusRecord;
struct ResultRecord;

#include <stdint.h>

/**
 * Interface to talk to an ACServer
 */
class ACServer
{
public:
    virtual ~ACServer();

    /**
     * Query a card by UID, returning the resulting record
     *
     * \returns a CardRecord (which you must free) or NULL if the lookup failed
     */
    virtual CardRecord* queryCard(const char* uid)=0;

    /**
     * Query the status of this node, returning the resulting record
     *
     * \returns a StatusRecord (which you must free) or NULL if the lookup failed
     */
    virtual StatusRecord* queryNodeStatus()=0;

    /**
     * Set the status of the tool
     *
     */
    virtual ResultRecord* setToolStatus(uint8_t status, const char* cardUid)=0;
};

#endif
