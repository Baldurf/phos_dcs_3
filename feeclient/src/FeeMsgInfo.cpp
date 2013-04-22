//#include "FeeMsgInfo.hpp"
#include "feeclient/include/FeeMsgInfo.hpp"

//#include "FeeClientLogger.hpp"
#include "feeclient/include/FeeClientLogger.hpp"

#include <stdio.h>

#ifdef __DEBUG
#include <iostream>
using namespace std;
#endif

using namespace dcs::fee;


FeeMsgInfo::~FeeMsgInfo() {
    if (mpServerName != 0) {
		delete[] mpServerName;
	}

}

bool FeeMsgInfo::retrieveServiceValue() {
    void* pData;

    if (getSize() != sizeof(MessageStruct)) {
		// this means an error or no Link, so message struct cannot be filled
		const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
		char msg[200];
		msg[sprintf(msg, "No Link or strange data on message channel of %s.",
				getName())] = 0;
        aLogger->createLogMessage(MSG_WARNING, FEE_ROOT_CLIENT, msg);
        return false;
    }

    pData = getData();
    mpFeeMessage = (static_cast<MessageStruct*> (pData));

    FeeClientLogger::getLogger()->relayLogEntry(mpFeeMessage);

    return true;
}


void FeeMsgInfo::initialize() {
    void* pData;

    if (getSize() != sizeof(MessageStruct)) {
        // this means an error or no Link, so message struct cannot be filled
        const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
		char msg[200];
		msg[sprintf(msg, "No Link to message channel of %s.",
		        getName())] = 0;
        aLogger->createLogMessage(MSG_WARNING, FEE_ROOT_CLIENT, msg);
#       ifdef __DEBUG
        cerr << "Message channel from FeeServer " << mpServerName <<
                    " has no link!" << endl;
#       endif
        return;
    }

    pData = getData();
    mpFeeMessage = (static_cast<MessageStruct*> (pData));
}

