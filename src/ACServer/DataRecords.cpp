#include "DataRecords.h"

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