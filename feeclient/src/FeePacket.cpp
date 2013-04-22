//#include "FeePacket.hpp"
#include "feeclient/include/FeePacket.hpp"
#include <memory>
#include <cstdlib>

#include <string.h>

using namespace dcs::fee;


const int FeePacket::HEADER_SIZE = FEE_PACKET_HEADER_SIZE;

unsigned int FeePacket::mLastID = 0;

FeePacket::FeePacket(short errorCode, char* payload,
            int payloadSize, bool checksum, FlagBits flags, bool huffman) {


	// reset mLastID, when it is larger than FEE_PACKET_MAX_ID
	if (mLastID >= FEE_PACKET_MAX_ID) {
		mLastID = 0;
	}
    initFeePacket((++mLastID), errorCode, payload, payloadSize, checksum, flags,
            huffman);
}

FeePacket::FeePacket(unsigned int id, short errorCode, char* payload,
            bool legal, int payloadSize, bool checksum, FlagBits flags,
            bool huffman) {
    initFeePacket(id, errorCode, payload, payloadSize, checksum, flags,
            huffman, legal);
}

void FeePacket::initFeePacket(unsigned int id, short errorCode, char* payload,
                int payloadSize, bool checksum, FlagBits flags,
                bool huffman, bool legal) {
	mId = id;
    // take over given flags
	mFlags = flags;
    mPayload = new char[payloadSize];
    memcpy(mPayload, payload, payloadSize);
	mPayloadSize = payloadSize;
	mErrorCode = errorCode;

    // initialize checksum with 0
    mChecksum = 0;

	if (huffman) {
		//set huffman flag via binary OR
		mFlags |= HUFFMAN_FLAG;
		checksum = true;
	}

	if (checksum) {
		//set checksum flag via binary OR
		mFlags |= CHECKSUM_FLAG;
	}

    mValid = legal;
}



FeePacket::~FeePacket() {

	if (mPayload != NULL ) {
		delete[] mPayload;
	}
}


unsigned int FeePacket::calculateChecksum(unsigned char* input, int size) {
    unsigned int checks = 0;
    unsigned long adler = 1L;
    unsigned long part1 = adler & 0xffff;
    unsigned long part2 = (adler >> 16) & 0xffff;

    // calculates the checksum with the Adler32 algorithm
    for (int n = 0; n < size; n++) {
        part1 = (part1 + input[n]) % ADLER_BASE;
        part2 = (part2 + part1) % ADLER_BASE;
    }
    checks = (unsigned int) ((part2 << 16) + part1);

	return checks;
}

char* FeePacket::marshall() {
    // do checksum
	if (mFlags & CHECKSUM_FLAG) {
        mChecksum = calculateChecksum((unsigned char*) mPayload, mPayloadSize);
	}

    // do Huffman, IMPORTANT Do huffmann always after checksum
    // so we can check, if huffman decoding has been correct !
    if (mFlags & HUFFMAN_FLAG) {

    	// do Huffman with payload and
		// store new payloadsize
    }

	int size = mPayloadSize + HEADER_SIZE;
	char* marshalledFeePacket = new char[size];

	memcpy(marshalledFeePacket, &mId, FEE_PACKET_ID_SIZE);
	memcpy((marshalledFeePacket + FEE_PACKET_ID_SIZE), &mErrorCode,
            FEE_PACKET_ERRORCODE_SIZE);
	memcpy((marshalledFeePacket + FEE_PACKET_ID_SIZE +
            FEE_PACKET_ERRORCODE_SIZE), &mFlags, FEE_PACKET_FLAG_SIZE);
	memcpy((marshalledFeePacket + FEE_PACKET_ID_SIZE +
            FEE_PACKET_ERRORCODE_SIZE + FEE_PACKET_FLAG_SIZE), &mChecksum,
            FEE_PACKET_CHECKSUM_SIZE);

	int pos = HEADER_SIZE;

	for (int i = 0; i < mPayloadSize; i++) {
		marshalledFeePacket[pos++] = mPayload[i];
	}

	// do big-endian, little-endian ???
	return marshalledFeePacket;
}


FeePacket* FeePacket::unmarshall(char* marshalledFeePacket, int size) {
	unsigned int id;
	short errorCode;
	FlagBits flags;
	unsigned int checksum;
    bool legal = true;

	int payloadSize = size - HEADER_SIZE;

	memcpy(&id, marshalledFeePacket, FEE_PACKET_ID_SIZE);
	memcpy(&errorCode, (marshalledFeePacket + FEE_PACKET_ID_SIZE),
            FEE_PACKET_ERRORCODE_SIZE);
	memcpy(&flags, (marshalledFeePacket + FEE_PACKET_ID_SIZE +
            FEE_PACKET_ERRORCODE_SIZE), FEE_PACKET_FLAG_SIZE);
	memcpy(&checksum, (marshalledFeePacket + FEE_PACKET_ID_SIZE +
            FEE_PACKET_ERRORCODE_SIZE + FEE_PACKET_FLAG_SIZE),
            FEE_PACKET_CHECKSUM_SIZE);

	// do big-endian, little-endian ??? // this is done by the DIM framework

    char* payload  = marshalledFeePacket + HEADER_SIZE;

	if (flags & HUFFMAN_FLAG) {
		//decode payload -> do DeHuffman !!!
		// and store new payloadSize !!!
	}

	if (flags & CHECKSUM_FLAG) {
        if (calculateChecksum((unsigned char*) payload, payloadSize) !=
                checksum) {
			// payload invalid -> do something !!!
            legal = false;
		}
	}

	return new FeePacket(id, errorCode,	payload, legal, payloadSize,
            ((flags & CHECKSUM_FLAG) > NO_FLAGS) ? true : false,
            flags, // just take over flags
            ((flags & HUFFMAN_FLAG) > NO_FLAGS) ? true : false);
}



