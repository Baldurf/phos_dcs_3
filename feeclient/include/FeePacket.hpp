#ifndef ZTT_DCS_FEE_PACKET_HPP
#define ZTT_DCS_FEE_PACKET_HPP

#include "feepacket_flags.h"

namespace dcs { namespace fee {

/**
 * Typedef for a two byte field for flag bits.
 *
 * @see feepacket_flags.h for more information
 */
typedef unsigned short FlagBits;

/**
 * Define for the size of ID field in header of FEE-Packet.
 */
#define FEE_PACKET_ID_SIZE          4
/**
 * Define for the length of the error code in header of FEE-Packet.
 */
#define FEE_PACKET_ERRORCODE_SIZE   2
/**
 * Define for the length of flag field in header of FEE-Packet.
 */
#define FEE_PACKET_FLAG_SIZE        2
/**
 * Define for the length of checksum field in header of FEE-Packet.
 */
#define FEE_PACKET_CHECKSUM_SIZE    4

/**
 * Define for the size of FeePacket header (12) of FEE-Packet.
 */
#define FEE_PACKET_HEADER_SIZE  (FEE_PACKET_ID_SIZE + FEE_PACKET_ERRORCODE_SIZE + FEE_PACKET_FLAG_SIZE + FEE_PACKET_CHECKSUM_SIZE)


/**
 * Define of the max ID in header of FEE-Packet.
 * Due to compatibility to different systems (2 byte).
 */
#define FEE_PACKET_MAX_ID           65530

/**
 * Define of the string for constructing packet with no error code.
 * Errorcode is represented as short.
 */
#define FEE_PACKET_NO_ERRORCODE     0


/**
 * Define for the Adler base; largest prime number smaller than 2^16 (65536).
 * Used by the checksum algorithm.
 */
#define ADLER_BASE 65521


/**
 * @class FeePacket
 * This class covers the work for marshalling and unmarshalling the command,
 * send down to the DIM-Server.
 *
 * @author Christian Kofler, Sebastian Bablok, Eric Conner
 *
 * @ingroup corelib
 */
class FeePacket {
	private:
		/** stores the errorCode, no error code is represented by 0 */
		short mErrorCode;

		/** stores the package id*/
		unsigned int mId;

		/** stores the flags */
		FlagBits mFlags;

		/** stores the checksum */
		unsigned int mChecksum;

		/** stores the payload, not NULL terminated ! */
		char* mPayload;

		/** stores the payload size in bytes */
		int mPayloadSize;

        /** indicates, if payload is valid (checksum correct). */
        bool mValid;

		/** Headersize, does NOT change */
		static const int HEADER_SIZE;

        /** Last transmitted ID for FeePackets */
        static unsigned int mLastID;

		/**
		 * Method to calculate the checksum of an input stream.
         * This function uses the Adler32 algorithm, which returns a 4 byte
         * long checksum and can check any length of input stream.
		 * Normally used to calculate the checksum of the payload.
         * Note: the checksum is always taken before huffman encoding and
         * after huffman decoding, so we check also the correctness of the
         * decoded data.
		 *
		 * @param input stream to calculate checksum of (NOTE: use of "unsigned
         *          char*" is important to be compatible with different
         *          plattforms; is mainly important, when checking binary data.)
         * @param size the size of the input stream
		 *
		 * @return 4 byte long checksum.
		 */
		unsigned int static calculateChecksum(unsigned char* input, int size);

        /**
		 * Constructor for a command. If the huffman-flag is set, the payload
		 * will be coded in huffman here in the constructor due to consistency
		 * of the payload size member. This means: the payload and payloadSize
		 * differs from the input in the constructor afterwards.
		 *
		 * @param id id of the package
		 * @param errorCode the errorCode in the header of the command
		 * @param payload the payload of the command to be send
         * @param legal indicates, if payload data is valid.
		 * @param payloadSize the size of the payload in bytes
         * @param checksum indicates, if checksum-flag is or will be set
		 *			(depends on marshall or unmarshall, by default this flag
         *          is set to false)
         * @param flags contains flag bits, indicating commands to feeserver
         *          (by default this flag is set to NO_FLAGS)
         * @param huffman indicates, if huffman-flag is or will be set
		 *			(depends on marshall or unmarshall, by default this flag is
         *          set to false), NOTE: If flag is set,the checksum - flag
         *          will be set automatically.
		 */
		FeePacket(unsigned int id, short errorCode, char* payload, bool legal,
                 int payloadSize, bool checksum = false,
                 FlagBits flags = NO_FLAGS, bool huffman = false);

        /**
         * Initializes a FeePacket.
         * This function is used by both construtors to initialize the packet
         * and covers the part, which is identic for both construction methods.
         *
         * @param id id of the package
		 * @param errorCode the errorCode in the header of the command
		 * @param payload the payload of the command to be send
		 * @param payloadSize the size of the payload in bytes
         * @param checksum indicates, if checksum-flag is or will be set
		 *			(depends on marshall or unmarshall, by default this flag
         *          is set to false)
         * @param flags flags contains flag bits, indicating commands to feeserver
         *          (by default this flag is set to NO_FLAGS)
         * @param huffman indicates, if huffman-flag is or will be set
		 *			(depends on marshall or unmarshall, by default this flag is
         *          set to false), NOTE: If flag is set,the checksum - flag
         *          will be set automatically.
         * @param legal indicates, if payload data is valid. (default = true).
		 */
        void initFeePacket(unsigned int id, short errorCode, char* payload,
                int payloadSize, bool checksum, FlagBits flags,
                bool huffman, bool legal = true);


	public:
		/**
		 * Constructor for a command. If the huffman-flag is set, the payload
		 * will be coded in huffman here in the constructor due to consistency
		 * of the payload size member. This means: the payload and payloadSize
		 * differs from the input in the constructor afterwards. The ID for the
         * FeePacket is set inside this constructor.
		 *
		 * @param errorCode the errorCode in the header of the command
		 * @param payload the payload of the command to be send
		 * @param payloadSize the size of the payload in bytes
         * @param checksum indicates, if checksum-flag is or will be set
		 *			(depends on marshall or unmarshall, by default this flag
         *          is set to false)
         * @param flags flags contains flag bits, indicating commands to feeserver
         *          (by default this flag is set to NO_FLAGS)
         * @param huffman indicates, if huffman-flag is or will be set
		 *			(depends on marshall or unmarshall, by default this flag is
         *          set to false), NOTE: If flag is set,the checksum - flag
         *          will be set automatically.
		 */
		FeePacket(short errorCode, char* payload, int payloadSize,
                 bool checksum = false, FlagBits flags = NO_FLAGS,
                 bool huffman = false);

		/**
		 * Destructor for a FeePacket object.
		 */
		~FeePacket();

		/**
		 * Gives back package ID of this command
		 *
		 * @return package ID
		 */
		unsigned int getId() const;

		/**
		 * Gives back the error Code, if it is the result data from the
		 * FeeServer.
		 * For more details see FeeServer: fee_errors.h
		 *
		 * @return error code
		 */
		short getErrorCode() const;

		/**
		 * Gives back the payload size
		 *
		 * @return payload size in bytes
		 */
		int getPayloadSize() const;

		/**
		 * Gives back the size of the command in bytes
		 *
		 * @return the commandsize in bytes
		 */
		int getFeePacketSize() const;

		/**
		 * Gives back the byte, representing the set flags.<pre>
		 * 0x0000 -> no flag is set
		 * 0x0002 -> only checksum flag is set
         * 0x0003 -> huffman and checksum flag are set
		 * </pre><br>
         * NOTE :The huffman flag cannot be set alone!
         * All other flags indicate a specific command that has to be
         * executed by the feeserver itself. For details:
         * @see feepacket_flags.h
		 *
		 * @return the flag bits (2 byte)
		 */
		FlagBits getFlags() const;

		/**
		 * Tells, if the huffman-flag is indicated
		 *
		 * @return true, if huffman-flag is or will be set
		 */
		bool isHuffmanSet() const;

		/**
		 * Tells, if the checksum-flag is indicated
		 *
		 * @return true, if checksum-flag is or will be set
		 */
		bool isChecksumSet() const;

        /**
         * Tells, if payload is valid (checksum has been correct).
         *
         * @return true, if checksum has been correct.
         */
        bool isValid() const;


		/**
		 * Gives back the payload of this command, note that the payload is
         * not NULL terminated.
		 *
		 * @return payload
		 */
		char* getPayload() const;

		/**
		 * Marshalls the command, so it can be send as byte-stream (char-stream)
		 * down to the FeeServer. The management of this memory is in the duty
         * of the caller !
		 *
		 * @return the marshalled command as charstream
		 */
		char* marshall();

		/**
		 * Constructs a FeePacket out of a received byte-stream (char-stream).
         * The management of the memory of the returned object is in the duty
         * of the caller.
		 *
		 * @param marshalledFeePacket the byte-stream from which the command will
		 *			be constructed
		 * @param size the size of the byte-stream in bytes
		 *			(this is incl. headersize).
		 *
		 * @return the newly constructed FeePacket from the byte-stream
		 */
		static FeePacket* unmarshall(char* marshalledFeePacket, int size);

}; // end class

inline unsigned int FeePacket::getId() const {
	return mId;
}

inline short FeePacket::getErrorCode() const {
	return mErrorCode;
}

inline char* FeePacket::getPayload() const {
	return mPayload;
}

inline int FeePacket::getPayloadSize() const {
	return mPayloadSize;
}

inline int FeePacket::getFeePacketSize() const {
	return (mPayloadSize + HEADER_SIZE);
}

inline FlagBits FeePacket::getFlags() const {
	return mFlags;
}

inline bool FeePacket::isHuffmanSet() const {
	// > 0 means true
	return ((mFlags & HUFFMAN_FLAG) > NO_FLAGS) ? true : false;
}

inline bool FeePacket::isChecksumSet() const {
	// > 0 means true
	return ((mFlags & CHECKSUM_FLAG) > NO_FLAGS) ? true : false;
}

inline bool FeePacket::isValid() const {
    return mValid;
}

} } //end of namespace ztt::dcs

#endif
