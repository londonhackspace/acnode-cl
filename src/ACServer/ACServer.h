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
     * \brief Query a card by UID, returning the resulting record
     *
     * \param uid The UID of the card to query
     *
     * \returns a CardRecord (which you must free) or NULL if the lookup failed
     */
    virtual CardRecord* queryCard(const char* uid)=0;

    /**
     * \brief Query the status of this node
     *
     * \returns a StatusRecord (which you must free) or NULL if the lookup failed
     */
    virtual StatusRecord* queryNodeStatus()=0;

    /**
     * \brief Set the status of the tool
     *
     * \param status 1 for in service, 0 for out of service
     *
     * \returns a ResultRecord (which you must free) indicating the 
     *          success/failure of the request or NULL if it failed to make the request
     */
    virtual ResultRecord* setToolStatus(uint8_t status, const char* cardUid)=0;

    /**
     * \brief Report tool usage time
     *
     * \param time Usage time in seconds
     * \param cardUid The UID of the card that used the tool
     * \returns a ResultRecord (which you must free) indicating the 
     *          success/failure of the request or NULL if it failed to make the request
     */ 
    virtual ResultRecord* reportToolUseTime(int time, const char* cardUid)=0;

    /**
     * \brief Report the current usage state of the tool
     * 
     * \param cardUid The UID of the card that is using the tool
     * \param state 1 for start of use, 0 for end of use
     *
     * \returns a ResultRecord (which you must free) indicating the 
     *          success/failure of the request or NULL if it failed to make the request
     */
    virtual ResultRecord* reportToolUse(const char* cardUid, uint8_t state)=0;

    /**
     * \brief Add a new user to the tool
     * 
     * \param maintainerUid The UID of the maintainer's card
     * \param userUid The UID of the new user's card
     *
     * \returns a ResultRecord (which you must free) indicating the 
     *          success/failure of the request or NULL if it failed to make the request
     */
    virtual ResultRecord* addNewUser(const char* maintainerUid, const char* userUid)=0;
};

#endif
