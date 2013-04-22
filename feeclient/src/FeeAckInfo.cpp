//#include "FeeAckInfo.hpp"
#include "../include/FeeAckInfo.hpp"
//#include "FeePacket.hpp"
#include "../include/FeePacket.hpp"
//#include "FeeClientLogger.hpp"
#include "../include/FeeClientLogger.hpp"
//#include "FeeClientLibInterface.hpp"
#include "../include/FeeClientLibInterface.hpp"
//#include "fee_errors.h"
#include "../include/fee_errors.h"

//#include "../dim/dim/dim.hxx"
//#include "../dim/

#include <stdio.h>
#include <iostream>

using namespace std;
using namespace dcs::fee;

FeeAckInfo::~FeeAckInfo() {
    if (mpServerName != 0) {
		delete[] mpServerName;
	}
}

bool FeeAckInfo::retrieveServiceValue() {
	short ackState = FEE_OK;

    if (getSize() < FEE_PACKET_HEADER_SIZE) {
		// to short -> service value cannot contain own protocol definition
		// test for "No link"
		char testData[FEE_ACK_NO_LINK_SIZE + 1];
		memcpy((void*) testData, (void*) getData(), FEE_ACK_NO_LINK_SIZE);
		testData[FEE_ACK_NO_LINK_SIZE] = 0;

		// compare to "No Link",
		if (strcmp(testData, FEE_ACK_NO_LINK) == 0) {
            // notify FedServer of lost link !!!

			ackState = FEE_ACK_LOST_LINK;
			// signal no link to service to interface
			if (mpInterface == 0) {
				const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
				char msg[200];
				msg[sprintf(msg, "No FeeClientLib-interface registered for ACKs of %s",
								getName())] = 0;
				aLogger->createLogMessage(MSG_ERROR, FEE_ROOT_CLIENT, msg);
				return true;
			}
			mpInterface->acknowledgeHandler(mpServerName, 0, 0, 0, 0, 0,
					FEE_ACK_LINK_DOWN);


			return false;
		}

        const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
        char msg[200];
        msg[sprintf(msg, "Received strange acknowledge data from %s: %s.",
                getName(), getString())] = 0;
        aLogger->createLogMessage(MSG_WARNING, FEE_ROOT_CLIENT, msg);

		ackState = FEE_ACK_STRANGE_DATA;
		// signal strange data to interface
		if (mpInterface == 0) {
			const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
			char msg[200];
			msg[sprintf(msg, "No FeeClientLib-interface registered for ACKs of %s",
							getName())] = 0;
			aLogger->createLogMessage(MSG_ERROR, FEE_ROOT_CLIENT, msg);
			return true;
		}
		mpInterface->acknowledgeHandler(mpServerName, 0, 0, 0, 0, 0,
					FEE_ACK_CORRUPTED);

		return true;
	}

	// received data seems to be OK -> extracting to FeePacket
	ackState = FEE_ACK_LINK_OK;
    FeePacket* pResult = FeePacket::unmarshall((char*) getData(), getSize());

    if (!pResult->isValid()) {
		const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
		char msg[200];
		msg[sprintf(msg, "Received data with invalid checksum from %s",
		                getName())] = 0;
        aLogger->createLogMessage(MSG_WARNING, FEE_ROOT_CLIENT, msg);

        // signal about invalid checksum, but give data despite of that as well
        ackState = FEE_CHECKSUM_FAILED;
    }

    // 	pass ACK via callback/handler routine to interface
	if (mpInterface == 0) {
		const FeeClientLogger* aLogger = FeeClientLogger::getLogger();
		char msg[200];
		msg[sprintf(msg, "No FeeClientLib-interface registered for ACKs of %s",
						getName())] = 0;
		aLogger->createLogMessage(MSG_ERROR, FEE_ROOT_CLIENT, msg);
		return true;
	}

	if (!mpInterface->checkIDinList(pResult->getId())) {
		// ACK is not expected -> discard it
		return true;
	}

	mpInterface->acknowledgeHandler(mpServerName, pResult->getPayloadSize(),
						pResult->getPayload(), pResult->getId(),
						pResult->getFlags(), pResult->getErrorCode(), ackState);

/*
#ifdef __DEBUG // debug output to check received ACK on command line
    int size = pResult->getPayloadSize();
    char* data = new char[size + 1];
	memcpy(data, pResult->getPayload(), size);
	data[size] = 0;
	cerr << "\n im gSV (\"getServiceValue\"): incoming data (char*): " <<
                data << endl;
	cerr << endl;
	delete[] data;
#endif
*/

	return true;
}


void FeeAckInfo::initialize() {
	char testData[FEE_ACK_NO_LINK_SIZE + 1];
	// MaRi 22.12.2005 crash after update from DIM version 12.04 to 15.20
	// suddenly the function getData might return a NULL Pointer which causes the
	// memcpy to crash
	void* pData=getData();
	if (pData) {
	  memcpy((void*) testData, (void*) getData(), FEE_ACK_NO_LINK_SIZE);
	  testData[FEE_ACK_NO_LINK_SIZE] = 0;

	  // compare to "No Link",
	  if (strcmp(testData, FEE_ACK_NO_LINK) == 0) {
	    // no link to service
	    ackState = FEE_ACK_LOST_LINK;
	  } else {
	    // MaRi 22.12.2005 the else was missing and that looked to me as a bug
	    ackState = FEE_ACK_LINK_OK;
	  }
	} else {
	  ackState = FEE_ACK_LOST_LINK;
	}
	// now comes the original code
	/*
	memcpy((void*) testData, (void*) getData(), FEE_ACK_NO_LINK_SIZE);
	testData[FEE_ACK_NO_LINK_SIZE] = 0;

	// compare to "No Link",
	if (strcmp(testData, FEE_ACK_NO_LINK) == 0) {
		// no link to service
		ackState = FEE_ACK_LOST_LINK;
	}
	ackState = FEE_ACK_LINK_OK;
	*/

#ifdef __DEBUG
	cerr << "Init - Stream: " << testData << endl;
#endif

}

