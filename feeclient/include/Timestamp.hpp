#ifndef DCS_FEE_TIMESTAMP_HPP
#define DCS_FEE_TIMESTAMP_HPP

#include <time.h>
#include "fee_loglevels.h"

namespace dcs { namespace fee {
/**
 * @class Timestamp
 * Represents a timestamp for log entry.
 * The format of the date should be unique for all log entries.
 * So the creation of the log entry should be done with this struct.
 *
 * @author Christian Kofler, Sebastian Bablok
 *
 * @date 2004-04-06
 *
 * @ingroup corelib
 */
struct Timestamp {
    /**
     * Constructs the Timestamp with the current date.
     * The format should be adapted here.
     */
    Timestamp() {
  	    time_t timeVal;
		struct tm* now;

		time(&timeVal);
		now = localtime(&timeVal);

		int length = strftime(date, MSG_DATE_SIZE, "%Y-%m-%d %H:%M:%S", now);
        // terminate with '\0'
        date[length] = 0;
    }

    /**
     * Destructs the timestamp
     */
    ~Timestamp() {
    }

    /**
     * Stores the timestamp in a char string
     */
    char date[MSG_DATE_SIZE];

};


} }

#endif
