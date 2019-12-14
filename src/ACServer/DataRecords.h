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

struct ResultRecord : public BaseRecord
{
    ResultRecord();
    ~ResultRecord();

    char* successMesage;
};

// This one doesn't use the usual pattern of numeric status
// it's a bit under 4kb in total, so not completely crazy
// on this size of microcontroller.
struct MaintainerListRecord
{
    MaintainerListRecord();
    ~MaintainerListRecord();
    bool isSuccess() { return success; }

    bool success;
    char* error;
    char maintainers[255][15];
    uint8_t count;
};

#endif
