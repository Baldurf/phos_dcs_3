//#include "FeeClientLogger.hpp"
#include "feeclient/include/FeeClientLogger.hpp"
//#include "FeeClientLibInterface.hpp"
#include "feeclient/include/FeeClientLibInterface.hpp"
//#include "Timestamp.hpp"
#include "feeclient/include/Timestamp.hpp"



using namespace dcs::fee;

FeeClientLogger* FeeClientLogger::pInstance = 0;

FeeClientLogger::~FeeClientLogger() {
    if (pInstance == 0) {
        return;
    }

    // set pInstance to 0
    pInstance = 0;
    mpInterface = 0;
}

bool FeeClientLogger::registerInterface(FeeClientLibInterface* interface) {
	if (mpInterface == 0) {
		mpInterface = interface;
		return true;
	}
	return false;
}

FeeClientLogger* FeeClientLogger::getLogger() {
    if (pInstance == 0) {
        pInstance = new FeeClientLogger();
    }
    return pInstance;
}

void FeeClientLogger::createLogMessage(unsigned int type, char* origin,
										char* description) const {
    // check LogLevel
    if (!checkLogLevel(type)) {
        return;
    }

	Timestamp now;

	MessageStruct msg(type, DETECTOR, origin, description, now.date);
	relayLogEntry(&msg);
}

void FeeClientLogger::relayLogEntry(MessageStruct* msgStruct) const {
	// check LogLevel
	if (!checkLogLevel(msgStruct->eventType)) {
	        return;
    }

	if (mpInterface != 0) {
	// call of the corresponding handler routine of the interface
    	mpInterface->messageHandler(*msgStruct);
	}
}
