//#include "FeeClientLibInterface.hpp"
#include "../include/FeeClientLibInterface.hpp"
//#include "FeeClientLogger.hpp"
#include "../include/FeeClientLogger.hpp"
//#include "FeeClientCore.hpp"
#include "../include/FeeClientCore.hpp"

using namespace std;
using namespace dcs::fee;


FeeClientLibInterface::FeeClientLibInterface() {
	mpFeeClient = new FeeClientCore(this);
}


FeeClientLibInterface::~FeeClientLibInterface() {
	clearFeeServerVector();
	clearServiceVector();
	mIDList.clear();
	delete mpFeeClient;
}


bool FeeClientLibInterface::registerFeeServerName(const char* serverName) {
	// check state
	if ((mpFeeClient->getState() != FEE_CLIENT_INIT_STATE) || (serverName == 0)) {
		return false;
	}

	// fill in local FeeServer vector, if not already existing
	if (isNewFeeServerName(serverName)) {
		char* name = new char[strlen(serverName) + 1];
		strcpy(name, serverName);
		mServerNames.push_back(name);
	}
	return true;
}

bool FeeClientLibInterface::isNewFeeServerName(const char* feeServerName) {
    vector<char* >::iterator it;
    for (it = mServerNames.begin(); it != mServerNames.end(); ++it) {
        if (strcmp(feeServerName, (*it)) == 0) {
			return false;
        }
    }
    return true;
}

bool FeeClientLibInterface::registerServiceName(const char* serviceName,
				void (*serviceHandler)(float fVal)) {
	// check state
	if ((mpFeeClient->getState() != FEE_CLIENT_INIT_STATE) || (serviceName == 0)
					|| (serviceHandler == 0)) {
		return false;
	}

	// fill in local Service vector, if not already existing
	if (isNewServiceName(serviceName)) {
		char* name = new char[strlen(serviceName) + 1];
		strcpy(name, serviceName);
		FeeServiceStruct* fss = new FeeServiceStruct();
		fss->name = name;
		fss->handlerRoutine = serviceHandler;
		mServiceNames.push_back(fss);
	}
	return true;
}

bool FeeClientLibInterface::isNewServiceName(const char* feeServiceName) {
    vector<FeeServiceStruct* >::iterator it;
    for (it = mServiceNames.begin(); it != mServiceNames.end(); ++it) {
        if (strcmp(feeServiceName, (*it)->name) == 0) {
			return false;
        }
    }
    return true;
}

int FeeClientLibInterface::startFeeClient() {
	// check state
	if (mpFeeClient->getState() != FEE_CLIENT_INIT_STATE) {
		return FEE_CLIENT_WRONG_STATE;
	}
	return mpFeeClient->subscribeTo(&mServerNames, &mServiceNames);
}


unsigned int FeeClientLibInterface::sendCommand(char* serverName,
				unsigned int size, char* data, FlagBits flags) {
	char* channelName;
	// check state
	if ((mpFeeClient->getState() != FEE_CLIENT_RUNNING_STATE) ||
					(serverName == 0)) {
		return FEE_CLIENT_NOT_STARTED;
	}

	channelName = new char[strlen(serverName) + 10];
	channelName[sprintf(channelName, "%s_Command", serverName)] = 0;

	FeePacket command(FEE_PACKET_NO_ERRORCODE, data, size, false, flags);
	mpFeeClient->sendCommandNB(channelName, (void*) command.marshall(),
		command.getFeePacketSize());

	delete[] channelName;
	insertIDtoList(command.getId());
	return command.getId();
}

bool FeeClientLibInterface::stopFeeClient() {
	// check state
	if (mpFeeClient->getState() != FEE_CLIENT_RUNNING_STATE) {
			return false;
	}

	// do the stopping here
	delete mpFeeClient;
	mpFeeClient = new FeeClientCore(this);

	return true;
}


int FeeClientLibInterface::clearFeeClient() {
	int amount = 0;

	if (mpFeeClient->getState() != FEE_CLIENT_INIT_STATE) {
			return FEE_CLIENT_WRONG_STATE;
	}

	amount = mServerNames.size() + mServiceNames.size();
	clearFeeServerVector();
	clearServiceVector();

	return amount;
}


void FeeClientLibInterface::clearFeeServerVector() {
	// check if not empty, the clear vector
	if (!mServerNames.empty()) {
		vector<char* >::iterator itServ = mServerNames.begin();
		while (itServ != mServerNames.end()) {
			if (*itServ != 0) {
				delete[] *itServ;
			}
			++itServ;
		}
		mServerNames.clear();
	}
}

void FeeClientLibInterface::clearServiceVector() {
	// check if not empty, the clear vector
	if (!mServiceNames.empty()) {
		vector<FeeServiceStruct* >::iterator it = mServiceNames.begin();
		while (it != mServiceNames.end()) {
			if ((*it) != 0) {
				delete (*it);
			}
			++it;
		}
		mServiceNames.clear();
	}
}

bool FeeClientLibInterface::deactivateIDinList(int id) {
	map<int, bool>::iterator listIterator;
	bool bRet = false;

	// lock mutex to protect handling of map
	lockListMutex();

	listIterator = mIDList.find(id);
	if (listIterator == mIDList.end()) {
		// the list does not contain this ID -> ACK not expected
		bRet = false;
	} else {
		// ID exists -> now check value
		if (listIterator->second) {
			// ID in list and active -> deactivate
			listIterator->second = false;
			bRet = true;
		} else {
			// ID has already been deactivated
			bRet = false;
		}
	}

	// free mutex
	unlockListMutex();
	return bRet;
}

bool FeeClientLibInterface::checkIDinList(int id) {
	map<int, bool>::iterator listIterator;
	bool bRet = false;

	// lock mutex to protect handling of map
	lockListMutex();

	listIterator = mIDList.find(id);
	if (listIterator == mIDList.end()) {
		// the list does not contain this ID -> ACK not expected
		bRet = false;
		// message
		const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
		char msg[200];
		msg[sprintf(msg,
				"Received ACK with ID %d, but this ACK is not expected",
				id)] = 0;
		aLogger->createLogMessage(MSG_INFO, FEE_ROOT_CLIENT, msg);
	} else {
		// ID exists -> now check value
		if (listIterator->second) {
			// ID in list and active
			bRet = true;
		} else {
			// ID found but ACK already regarded as lost -> discard ACK
			bRet = false;
			// message
			const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
			char msg[200];
			msg[sprintf(msg,
					"Received ACK with ID %d, but this ACK has already a timeout",
					id)] = 0;
			aLogger->createLogMessage(MSG_INFO, FEE_ROOT_CLIENT, msg);
		}
		// remove ID from list
		mIDList.erase(listIterator);
	}

	// free mutex
	unlockListMutex();
	return bRet;
}

void FeeClientLibInterface::insertIDtoList(int id) {
	map<int, bool>::iterator listIterator;

	// lock mutex to protect handling of map
	lockListMutex();

	listIterator = mIDList.find(id);
	if (listIterator == mIDList.end()) {
		// the list does not contain this ID -> insert it
		typedef pair<int, bool> IDPair;
		mIDList.insert(IDPair(id, true));

	} else {
		// ID already exists -> now check value
		if (listIterator->second) {
			// former ACK with same ID is still expected
			// write message
			const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
			char msg[200];
			msg[sprintf(msg,
					"Sending command with ID %d, which is still active ... collision may occur",
					id)] = 0;
			aLogger->createLogMessage(MSG_WARNING, FEE_ROOT_CLIENT, msg);
		} else {
			// ACK for ID is regarded as lost reactivate ID
			listIterator->second = true;
		}
	}

	// free mutex
	unlockListMutex();
}

void FeeClientLibInterface::lockListMutex() {
	int status = 0;
	// lock mutex for ID list
	status = fMutex.Lock();

	if (status != 0) {
		const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
		char msg[100];
		msg[sprintf(msg, "Unable to lock mutex for pending ID list")] = 0;
		aLogger->createLogMessage(MSG_ERROR, FEE_ROOT_CLIENT, msg);
	}
}

void FeeClientLibInterface::unlockListMutex() {
	int status = 0;
	// lock mutex for ID list
	status = fMutex.Unlock();

	if (status != 0) {
		const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
		char msg[100];
		msg[sprintf(msg, "Unable to unlock mutex for pending ID list")] = 0;
		aLogger->createLogMessage(MSG_ERROR, FEE_ROOT_CLIENT, msg);
	}
}


