//#include <FeeSampleClient.hpp>
#include "feeclient/include/FeeSampleClient.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cstdarg>
#include <sstream>
#include <unistd.h>
//#include <pthread.h>
//#include "LockGuard.hpp"
#include "feeclient/include/LockGuard.hpp"
//#include "AckData.hpp"
#include "feeclient/include/AckData.hpp"

//====================================================================
#define logLevelAll       0
#define logLevelBenchmark 1
#define logLevelDebug     2
#define logLevelInfo      3
#define logLevelWarning   4
#define logLevelError     5
#define logLevelFatal     6
#define logLevelNone      7
#define logLevel          logLevelAll

// Client flags 
#define FEECLIENTFLAGS 0x0 // bit 0x2 to switch on check sum
namespace 
{
  //====================================================================
  struct FeeException : public std::exception
  {
    std::string fWhat;
    virtual ~FeeException() throw () {}
    FeeException(const char* func, const char* format, ...)
      : fWhat("")
    {
      static char buf[1024];
      va_list ap;
      va_start(ap, format);
      vsprintf(buf,format, ap);
      va_end(ap);
      std::stringstream s;
      s << std::setw(8) << "Error" << " from " << func << ": " << buf;
      fWhat = s.str();
    }
    const char* what() const throw () { return fWhat.c_str(); }
  };
#define FeeWarn(ref, id, format, ... ) \
   FeeLogging(logLevelWarning, ref, id, __func__, format, ##__VA_ARGS__)
#define FeeError(format, ... ) \
   throw FeeException(__func__, format, ##__VA_ARGS__);
#ifndef __DEBUG
#define FeeDebug(format, ... ) 
#else 
#define FeeDebug(format, ... ) \
   FeeLogging(logLevelDebug,0,0,__func__, format, ##__VA_ARGS__);
#endif
  dcs::fee::Mutex mut;
}

//____________________________________________________________________
bool 
dcs::fee::FeeSampleClient::writeReadData(std::string&  serverName, 
					 size_t&       size, 
					 DataArray&    data, 
					 FlagBits&     flags, 
					 short&        errorCode, 
					 short&        status)
{
  FeeDebug("FeeClient::writeReadData(%s,%d,%p,%d,%d,%d)", 
	   serverName.c_str(),size,&(data[0]),flags,
	   errorCode,status);
  int id  = 0;
  int ret = 0;
  try {
    FeeDebug("FeeClient::writeReadData: size is %d", size);
    if(serverName.empty())   FeeError("No serverName");
    if(size == 0)            FeeWarn(1,  -1, "Size is zero");
    // This doesn't work, because reads may not have allocated
    // enough memory when we enter here.  However, that memory
    // will be allocated later, so there's no need to worry
    // really.
    if (size > data.size() * sizeof(DataArray::value_type)) {
      FeeError("to little data");
      return false;
    }
    
    size_t osize = size;
    FeeDebug("sending command %d", osize);
    id = sendCommand(const_cast<char*>(serverName.c_str()), osize, 
		     reinterpret_cast<char*>(&(data[0])), flags); 
    FeeDebug(" id returned is %d", id);
    if(id <= 0) FeeError("id=%id, sendCommand failed", id);
    
    flags       = 0;
    errorCode   = 0;
    status      = 0;
    ret = getAcknowledge(serverName, size, data, id, flags, 
			 errorCode, status);
    
    FeeDebug("size returned is %d", size);
    if(!ret) FeeError("ret=%i, getAcknowledge failed", ret);
  }
  catch (std::exception& e) {
    std::cerr << "FeeClient::writeReadData(" << serverName << "," 
          << size << "," << &(data[0]) << "," << flags << ","
	      << errorCode << "," << status << "):\n\t" 
          << e.what() << std::endl;
    return false;
  }
  return true;
}

//____________________________________________________________________
int
dcs::fee::FeeSampleClient::getAcknowledge(std::string&  serverName, 
					  size_t&       size, 
					  DataArray&    data, 
					  unsigned int  id, 
					  FlagBits&     flags, 
					  short&        errorCode, 
					  short&        status)
{
  FeeDebug("FeeClient::getAcknowledge(%s,%d,%p,%d,%d,%d,%d)", 
	   serverName.c_str(), size, &(data[0]), 
	   id, flags, errorCode, status);
  int uTimeout   = 2500000;
  int uSleepTime =   50000;
  int uTimeLeft  = uTimeout;
  int done       = false;
  AckDataBuffer::iterator it;
  try {
    if(id <= 0)       FeeError("id=%id, invalid id", id);
    while(!done){
      FeeDebug("Time left (usec) is now %i", uTimeLeft);
      if(!(uTimeLeft-uSleepTime)){
        deactivateIDinList(id);
      }
      usleep(uSleepTime);
      if(uTimeLeft){
	LockGuard l(mut);
        it=fAckDataBuffer.find(id);
        if(it!=fAckDataBuffer.end()){
          it->second->get(serverName, size, data, flags, errorCode, status);
          fAckDataPool.push_front(it->second);
          fAckDataBuffer.erase(it);
          done = true;
        }
      }
      else{
        FeeError("Time left (usec) is now %i, no response from FeeServer within timeout",
                 uTimeLeft);
        done = true;
      }
      uTimeLeft -= uSleepTime;
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 0;
  }
  return uTimeLeft;
}

//____________________________________________________________________
void
dcs::fee::FeeSampleClient::FeeLogging(int         type, 
				      int         ref, 
				      int         id, 
				      const char* func, 
				      const char* format, 
				      ...)
{
  if (type > logLevel) return;
  
  static char buf[1024];
  va_list ap;
  va_start(ap, format);
  vsprintf(buf,format, ap);
  va_end(ap);
  std::cerr << "id=" << id << ": "; 
  switch(type){
  case logLevelDebug:
    std::cerr << std::setw(8)  << "Debug"; break;
  case logLevelWarning:
    std::cerr << std::setw(8)  << "Warning"; break;
  case logLevelError: 
    std::cerr << std::setw(8)  << "Error"; break;
  }
  std::cerr << "(" << ref << ") from " << func << ": " << buf << std::endl;
}

//____________________________________________________________________
void
dcs::fee::FeeSampleClient::acknowledgeHandler(char*        serverName, 
					      unsigned int size, 
					      char*        data, 
					      unsigned int id, 
					      FlagBits     flags, 
					      short        errorCode, 
					      short        status)
{
  AckDataPool::iterator it;
  try {
    FeeDebug("FeeClient, handling acknowledge from %s of size %d", 
	     serverName, size);
    if(!serverName)   FeeError("no serverName");
    if(!*serverName)  FeeError("no *serverName");
    if(!data)         FeeError("no data");
    if(id == 0 && status < 0) {
      std::cerr << "Received status " << status << " from FeeServer\n"
		<< "serverName=" << serverName << "\n"
		<< "size      =" << size << "\n"
		<< "id        =" << id << "\n" 
		<< "flags     =" << flags << "\n"
		<< "errorCode =" << errorCode << std::endl;
    }
    else{
      LockGuard l(mut);
      if(fAckDataPool.empty()){
        fAckDataPool.push_front(new AckData());
      }
      it=fAckDataPool.begin();
      FeeDebug("Storing data of size %d", size);
      
      (**it).set(serverName, 
		 size, 
		 data, 
		 flags, 
		 errorCode, 
		 status);
      fAckDataBuffer[id]=*it;
      fAckDataPool.erase(it);
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return;
  }
}

//____________________________________________________________________
void
dcs::fee::FeeSampleClient::messageHandler(const dcs::fee::MessageStruct msg)
{
  std::cerr << "Message Type: " << msg.eventType << ", " 
	    << "Source: "       << msg.source << ", "
	    << "Date: "         << msg.date << "\n"
	    << "- "             << msg.description << " -\n" 
	    << std::endl;
}

//____________________________________________________________________
//
// EOF
//

