#include "DataRecords.h"
#include <cstring>

BaseRecord::BaseRecord() : 
    numericStatus(-1),
    error(nullptr)
{

}

BaseRecord::~BaseRecord()
{
    delete[] error;
}

CardRecord::CardRecord() :
    userName(nullptr)
{

}

CardRecord::~CardRecord()
{
    delete[] userName;
}

StatusRecord::StatusRecord() :
    status(nullptr),
    statusMessage(nullptr)
{

}

StatusRecord::~StatusRecord()
{
    delete[] status;
    delete[] statusMessage;
}

ResultRecord::ResultRecord() :
    successMesage(nullptr)
{

}

ResultRecord::~ResultRecord()
{
    delete[] successMesage;
}

MaintainerListRecord::MaintainerListRecord() :
    success(false),
    error(nullptr),
    count(0)
{
    memset(maintainers, 0, sizeof(maintainers));
}

MaintainerListRecord::~MaintainerListRecord()
{
    if(!success)
    {
        delete[] error;
    }
}
