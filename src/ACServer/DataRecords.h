#ifndef DATARECORDS_H
#define DATARECORDS_H

#include <stdint.h>

struct BaseRecord
{
    BaseRecord();
    virtual ~BaseRecord();

    bool isSuccess() const { return numericStatus > 0; }

    uint8_t numericStatus;
    char* error;
};

struct CardRecord : public BaseRecord
{
    CardRecord();
    ~CardRecord();

    char* userName;
    int userId;
};

#endif
