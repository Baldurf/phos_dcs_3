#ifndef DCS_FEE_MESSAGE_STRUCT_HPP
#define DCS_FEE_MESSAGE_STRUCT_HPP

//#include "Timestamp.hpp"
#include "../include/Timestamp.hpp"

//#include "fee_loglevels.h"
#include "../include/fee_loglevels.h"

#include <string>
#include <cstring>
#include <string.h>

namespace dcs { namespace fee {

/**
 * Define for the currently using detector
 */
#define DETECTOR "TPC"


/**
 * @class MessageStruct
 * This struct represents the composition of a message.
 * The size of this struct depends on how many gaps the compiler inserts to
 * fill to architecture borders. (On our first test, there seems to be gaps,
 * who have in sum 3 bytes; total amount 620 (payload 617) - border size seems
 * to be 4 byte).
 *
 * 05-04-2004: -->>  Size (and so borders too) has changed ! Total size is now
 * 620 bytes. (-> there should be no gap now.)
 *
 * @ingroup corelib
 */
typedef struct MessageStruct {

    /**
     * The type of event (info, warning, error, ...).
     */
    unsigned int eventType;

    /**
     * The detector type (TRD, TPC).
     */
    char detector[MSG_DETECTOR_SIZE];

    /**
     * The origin of this message.
     */
    char source[MSG_SOURCE_SIZE];

    /**
     * The message itself.
     */
    char description[MSG_DESCRIPTION_SIZE];

    /**
     * The date of the message ("YYYY-MM-DD hh:mm:ss\0").
     */
    char date[MSG_DATE_SIZE];

    /**
     * Initial constructor for MessageStruct initializes all struct members.
     *
     * @param detectorName three character long string representing name of
     *          detector.
     */
    MessageStruct(char* detectorName) {
        Timestamp now;

        eventType = MSG_INFO;
        memcpy(detector, detectorName, MSG_DETECTOR_SIZE);
        strcpy(source, "FedServer");
        strcpy(description, "FedServer started");

        // set the proper date with Timestamp struct
        strcpy(date, now.date);
    }

    /**
     * Constructor for MessageStruct which allows to set all members.
     * This Contructor will be mainly used by the logger, to prepare a message
     * for the FedMessenger.
     *
     * @param evType the type of the event.
     * @param detectorName the name of the detector (size 4 byte).
     * @param origin the source, where message originally comes from
     *      (size 256 byte).
     * @param descript the description of the event (size 256 byte).
     * @param eventDate the date, when event occured (size 20 byte).
     */
    MessageStruct(unsigned int evType, char* detectorName, char* origin, char* descript,
            char* eventDate) {
        eventType = evType;
        memcpy(detector, detectorName, MSG_DETECTOR_SIZE);
        strcpy(source, origin);
        strcpy(description, descript);
        strcpy(date, eventDate);
    }

    /**
     * Copy constructor for a MessageStruct.
     *
     * @param msg the message struct to produce a copy from.
     */
    MessageStruct(const MessageStruct& msg) {
		eventType = msg.eventType;
		memcpy(detector, msg.detector, MSG_DETECTOR_SIZE);
		memcpy(source, msg.source, MSG_SOURCE_SIZE);
		memcpy(description, msg.description, MSG_DESCRIPTION_SIZE);
        memcpy(date, msg.date, MSG_DATE_SIZE);
	}

    /**
     * Assignment operator for MessageStructs.
     * All values are copied.
     *
     * @param rhs the struct, which values should be copied to this MessageStruct
     *
     * @return the updated MessageStruct
     */
    MessageStruct& operator=(const MessageStruct& rhs) {
        if (this == &rhs) {
            return *this;
        }
        eventType = rhs.eventType;
        memcpy(detector, rhs.detector, MSG_DETECTOR_SIZE);
        memcpy(source, rhs.source, MSG_SOURCE_SIZE);
        memcpy(description, rhs.description, MSG_DESCRIPTION_SIZE);
        memcpy(date, rhs.date, MSG_DATE_SIZE);
        return *this;
    }

} MessageStruct;


} } // end of namespace

#endif
