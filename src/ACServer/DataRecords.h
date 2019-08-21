#ifndef DATARECORDS_H
#define DATARECORDS_H

#include <stdint.h>

struct BaseRecord
{
    BaseRecord();
    virtual ~BaseRecord();

    bool isSuccess() const { return numericStatus > 0; }

    int numericStatus;
    char* error;
};

struct CardRecord : public BaseRecord
{
    CardRecord();
    ~CardRecord();

    char* userName;
    int userId;
};

struct StatusRecord : public BaseRecord
{
    StatusRecord();
    ~StatusRecord();

    char* status;
    char* statusMessage;
};

#endif
