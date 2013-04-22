//#include "FeeClientCore.hpp"
#include "../include/FeeClientCore.hpp"
//#include "FeeAckInfo.hpp"
#include "../include/FeeAckInfo.hpp"
//#include "FeeItemInfo.hpp"
#include "../include/FeeItemInfo.hpp"
//#include "Timestamp.hpp"
#include "../include/Timestamp.hpp"

//#include "FeeMsgInfo.hpp"
#include "../include/FeeMsgInfo.hpp"
//#include "FeeClientLogger.hpp"
#include "../include/FeeClientLogger.hpp"

#include <stdio.h>

#ifdef __DEBUG
#include <iostream>
#endif

// using namespace std;
using namespace dcs::fee;

//____________________________________________________________________
FeeClientCore::FeeClientCore(FeeClientLibInterface* interface) 
{
  mpInterface = interface;
  mpLogger = FeeClientLogger::getLogger();
  mpLogger->registerInterface(mpInterface);
  mState = FEE_CLIENT_INIT_STATE;
}


//____________________________________________________________________
FeeClientCore::~FeeClientCore() 
{
  //clean up
  // delete FeeItemInfo objects
  if (!mFeeItemInfoCollection.empty()) {
    std::vector<FeeItemInfo*>::iterator itInfo = 
      mFeeItemInfoCollection.begin();
    while (itInfo != mFeeItemInfoCollection.end()) {
      if (*itInfo != 0) {
	delete *itInfo;
      }
      ++itInfo;
    }
    mFeeItemInfoCollection.clear();
  }
  
  // delete FeeAckInfo objects
  if (!mFeeAckInfoCollection.empty()) {
    std::vector<FeeAckInfo*>::iterator itInfo = mFeeAckInfoCollection.begin();
    while (itInfo != mFeeAckInfoCollection.end()) {
      if (*itInfo != 0) {
	delete *itInfo;
      }
      ++itInfo;
    }
    mFeeAckInfoCollection.clear();
  }

  // delete FeeMsgInfo objects
  if (!mFeeMsgInfoCollection.empty()) {
    std::vector<FeeMsgInfo*>::iterator itInfo = mFeeMsgInfoCollection.begin();
    while (itInfo != mFeeMsgInfoCollection.end()) {
      if (*itInfo != 0) {
	delete *itInfo;
      }
      ++itInfo;
    }
    mFeeMsgInfoCollection.clear();
  }

}

//____________________________________________________________________
void 
FeeClientCore::infoHandler() 
{
  // get reference to currently handled DimInfo - incoming Service
  DimInfo* current = getInfo();
  char msg[255];

  // Try - catch block for dynamic cast
  // !! compiler must have enabled RTTI (option: /GR [on], /GR- [off, default])
  try {
    if (FeeInfo* pInfo = dynamic_cast<FeeInfo*> (current)) {
      if (!pInfo->retrieveServiceValue()) {

	// notification of no link to a given FeeServer
	msg[sprintf(msg, "FeeClientCore has NO LINK to %s.",
		    pInfo->getName())] = 0;
	mpLogger->createLogMessage(MSG_ERROR, SOURCE_FEE_CLIENT, msg);

#ifdef __DEBUG
	std::cerr << "No link on " << current->getName() << std::endl;
#endif

	return;
      }
      /*
	#ifdef __DEBUG // test of incoming data for the command line
	//get the name of the incoming Service
	cerr << "name of incoming service:  " << current->getName() << endl;
	//get the size of the Data of the incoming Service
	cerr << "size of incoming service:  " << current->getSize() << endl;
	cerr << "- - - - - - - - - - - - - - - - - - - - - " << endl;
	#endif
      */

    } else {
      msg[sprintf(msg, "FeeClient received unknown info-object (%s).",
		  current->getName())] = 0;
      mpLogger->createLogMessage(MSG_ERROR, SOURCE_FEE_CLIENT, msg);
    }

  } catch (std::exception) {
    // notify about cast exception
    msg[sprintf(msg,
                "In FeeClient occured an exception while receiving data from %s.",
                current->getName())] = 0;
    mpLogger->createLogMessage(MSG_ERROR, SOURCE_FEE_CLIENT, msg);
  }

}


//____________________________________________________________________
//-- subscribing to ACK - channels, message channels  and Items --
int 
FeeClientCore::subscribeTo(std::vector<char* >* servers,
			   std::vector<FeeServiceStruct* >* pServices) {

  if (mState == FEE_CLIENT_RUNNING_STATE) {
    // wrong state -> no sobscribing
    return 0;
  }

  // due to most compatibility the local collection are not used directly
  std::vector<FeeAckInfo*>* pAckCol = &mFeeAckInfoCollection; 
  //mpInterCom->getFeeAckInfoCollection();
  std::vector<FeeMsgInfo*>* pMsgCol = &mFeeMsgInfoCollection; 
  //mpInterCom->getFeeMsgInfoCollection();

  // iterator for list of servers
  std::vector<char* >::iterator itServer = servers->begin();

  // -- go through list of servers, subscribe to ACK & MSG, add them
  // to Collection -- 
  while (itServer != servers->end()) {
    int length = strlen(*itServer);
    char* ack = new char[length + 13];
    char* msg = new char[length + 9];
    char* serverName = new char[length + 1];
    char* noLinkMsg = new char[length + 100];
    FeeAckInfo* ackService;
    FeeMsgInfo* msgService;
    Timestamp date;

    // construting Fee - specific structure and objects
    noLinkMsg[sprintf(noLinkMsg,
		      "Subscribed to channel %s, but maybe FeeClientCore has no link?!",
		      *itServer)] = 0;

    MessageStruct noLinkStruct(MSG_WARNING, DETECTOR, SOURCE_FEE_CLIENT,
			       noLinkMsg, date.date);

    // subscribing to ack channel
    sprintf(ack, "%s_Acknowledge\0", *itServer);
    sprintf(serverName, "%s\0", *itServer);
    ackService = new FeeAckInfo(ack, (void*) FEE_ACK_NO_LINK,
				FEE_ACK_NO_LINK_SIZE, this, 
				serverName, mpInterface);
    ackService->initialize();
    pAckCol->push_back(ackService);

    // subscribing to msg channel
    sprintf(msg, "%s_Message\0", *itServer);
    msgService = new FeeMsgInfo(msg, (void*) &noLinkStruct,
				this, serverName);
    msgService->initialize();
    pMsgCol->push_back(msgService);

    delete[] serverName;
    delete[] ack;
    delete[] msg;
    delete[] noLinkMsg;
    ++itServer;
  }

  // subscribing to item channels,
  // due to most compatibility the local collection are not used directly
  std::vector<FeeItemInfo*>* pItemCol = &mFeeItemInfoCollection; 
  //mpInterCom->getFeeItemInfoCollection();

  // iterator for list of services
  std::vector<FeeServiceStruct* >::iterator itServ = pServices->begin();
  while (itServ != pServices->end()) {
    //when service unreachable --> -1000.0 (FEE_ITEM_NO_LINK)
    float nolink = FEE_ITEM_NO_LINK;

    // update at value change AND every 10 sec (FEE_ITEM_TIMEOUT)
    //!!!causes error on DCS-Board with many Services when using
    //Timeout function!!!  FeeItemInfo* service = new
    //FeeItemInfo(*itServ, FEE_ITEM_TIMEOUT, nolink, this); trying to
    //eliminate Errors when subscribing without Timeout

    FeeItemInfo* service = new FeeItemInfo(**itServ, nolink, this);
    service->initialize();
    pItemCol->push_back(service);
    ++itServ;
  }

  mState = FEE_CLIENT_RUNNING_STATE;
  //return amount of services
  return (pItemCol->size() + pAckCol->size() + pMsgCol->size());
}

//____________________________________________________________________
//
// EOF
//

