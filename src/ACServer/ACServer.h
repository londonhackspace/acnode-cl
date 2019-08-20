#ifndef _ACSERVER_H
#define _ACSERVER_H

// Forward declarations
struct CardRecord;

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
};

#endif
