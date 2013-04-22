//#include "AckData.hpp"
#include "../include/AckData.hpp"
#include <string.h>

//____________________________________________________________________
dcs::fee::AckData::AckData()
{
  size       = 0;
  flags      = 0;
  errorCode  = 0;
  status     = 0;
}

//____________________________________________________________________
dcs::fee::AckData::AckData(const AckData& newPacket)
{
  set(newPacket.serverName, 
      newPacket.size, 
      newPacket.data, 
      newPacket.flags, 
      newPacket.errorCode, 
      newPacket.status);
}

//____________________________________________________________________
dcs::fee::AckData::AckData(const std::string& newServerName, 
			   const size_t newSize, 
			   const DataArray& newData, 
			   const FlagBits newFlags, 
			   const short newErrorCode, 
			   const short newStatus)
{
  set(newServerName, newSize, newData, newFlags, newErrorCode, newStatus);
}

//____________________________________________________________________
dcs::fee::AckData::AckData(const char* newServerName, 
			   const size_t newSize, 
			   const char* newData, 
			   const FlagBits newFlags, 
			   const short newErrorCode, 
			   const short newStatus)
{
  set(newServerName, newSize, newData, newFlags, newErrorCode, newStatus);
}

//____________________________________________________________________
dcs::fee::AckData::~AckData()
{}

//____________________________________________________________________
dcs::fee::AckData& dcs::fee::AckData::operator=(const AckData& newPacket)
{
  if(this == &newPacket) return *this;
  set(newPacket.serverName, 
      newPacket.size, 
      newPacket.data, 
      newPacket.flags, 
      newPacket.errorCode, 
      newPacket.status);
  return *this;
}

//____________________________________________________________________
void 
dcs::fee::AckData::set(const std::string& newServerName, 
		       const size_t newSize, 
		       const DataArray& newData, 
		       const FlagBits newFlags, 
		       const short newErrorCode, 
		       const short newStatus)
{
  serverName    = newServerName;
  size          = newSize;
  flags         = newFlags;
  errorCode     = newErrorCode;
  status        = newStatus;
  if (size <= 0) return;
  
  int arraySize = ((newSize-1)/sizeof(DataArray::value_type))+1;
  if (data.size() < arraySize) data.resize(arraySize);
  memcpy(&(data[0]), &(newData[0]), size);
}

//____________________________________________________________________
void 
dcs::fee::AckData::set(const char* newServerName, 
		       const size_t newSize, 
		       const char* newData, 
		       const FlagBits newFlags, 
		       const short newErrorCode, 
		       const short newStatus)
{
  serverName    = newServerName;
  size          = newSize;
  flags         = newFlags;
  errorCode     = newErrorCode;
  status        = newStatus;
  if (size <= 0) return;
  
  int arraySize = ((newSize-1)/sizeof(DataArray::value_type))+1;
  if (data.size() < arraySize) data.resize(arraySize);
  memcpy(&(data[0]), &(newData[0]), size);
}

//____________________________________________________________________
void const
dcs::fee::AckData::get(std::string& newServerName, 
		       size_t& newSize, 
		       DataArray& newData, 
		       FlagBits& newFlags, 
		       short& newErrorCode, 
		       short& newStatus)
{
  newServerName = serverName;
  newSize       = size;
  newFlags      = flags;
  newErrorCode  = errorCode;
  newStatus     = status;
  if (newSize <= 0) return;

  int newArraySize=((newSize-1)/sizeof(DataArray::value_type))+1;
  if (newData.size() < newArraySize) newData.resize(newArraySize);
  memcpy(&(newData[0]), &(data[0]), newSize);
}

//____________________________________________________________________
void const
dcs::fee::AckData::get(char*& newServerName, 
		       size_t& newSize, 
		       char*& newData, 
		       FlagBits& newFlags, 
		       short& newErrorCode, 
		       short& newStatus)
{
  newServerName = const_cast<char*>(serverName.c_str());
  newSize       = size;
  newFlags      = flags;
  newErrorCode  = errorCode;
  newStatus     = status;
  newData       = 0;

  if (newSize <= 0) return;

  int newArraySize = ((newSize-1)/sizeof(DataArray::value_type))+1;
  newData = new char[newArraySize];
  memcpy(&(newData[0]), &(data[0]), newSize);
}

//____________________________________________________________________
//
// EOF
//

