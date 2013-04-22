#ifndef DCS_FEE_FEE_CLIENT_LOGGER_HPP
#define DCS_FEE_FEE_CLIENT_LOGGER_HPP

#include "MessageStruct.hpp"

#include "fee_loglevels.h"

namespace dcs { namespace fee {

/**
 * Define for default log level (MSG_ALARM should be always included).
 */
#define DEFAULT_LOGLEVEL (MSG_INFO+MSG_ERROR+MSG_WARNING+MSG_ALARM)

/**
 * Define for the FeeRootClient as source. After renaming this is now the
 * FeeClientCore.
 */
#define FEE_ROOT_CLIENT "FeeClientCore"

class FeeClientLibInterface;

/**
 * @class FeeClientLogger
 * This class represents the logger for the FeeClient-library.
 *
 * @author Sebastian Bablok
 *
 * @date 2005-10-05
 *
 * @ingroup corelib
 */
class FeeClientLogger {
	public:
		/**
		 * Method to create/get the single Logger - object (Singleton).
		 * This method takes care, that only one Logger - object is created
		 * at one time. If this object already exists, it returns the pointer
		 * to this object (see also "Design Patterns, GoF, - Singleton).
		 * This function must be static, because it is called without having an
		 * Logger - object.
		 *
		 * @return pointer to the one and only Logger - object.
		 */
    	static FeeClientLogger* getLogger();

		/**
		 * Destructor for the Logger.
		 */
		virtual ~FeeClientLogger();

		/**
		 * Prepares a logging message for sending it to a registered handler
		 * routine.
		 *
		 * @param type the event type of this message
		 * @param origin the source of this message
		 * @param description the message body
		 */
    	void createLogMessage(unsigned int type, char* origin, char* description) const;

		/**
		 * Relays a log entry, which is received from a FeeServer.
		 * The messages is checked against the log level delivered further
		 * in case of pass.
		 *
		 * @param msgStruct pointer to the message struct storing the received
		 *          log entry from the FeeServer.
		 */
   		void relayLogEntry(MessageStruct* msgStruct) const;

   		/**
   		 * This function registers the interface to the logger. If an interface
   		 * has already been registered, the new one is discarded.
   		 *
   		 * @param interface pointer the FeeClientLibInterface
   		 *
   		 * @return false if an interface has already been registered
   		 */
   		bool registerInterface(FeeClientLibInterface* interface);

	private:
		/**
		 * Constructor for the Logger.
		 */
		FeeClientLogger() : mLogLevel(DEFAULT_LOGLEVEL), mpInterface(0) {};

		/**
		 * This is the one and only Logger - object.
		 * (see Singleton, GoF, Desing Patterns).
		 */
    	static FeeClientLogger* pInstance;

		/**
		 * Checks the event - type to the current LogLevel for local messages.
		 * If event - type is inside current LogLevel, the event will be announced,
		 * if not, no message will be send or logged (NO log file entry).
		 *
		 * @param event type of the event.
		 *
		 * @return true, if event - type is inside current LogLevel, else false.
		 */
		bool checkLogLevel(unsigned int event) const;

		/**
		 * Current LogLevel (default level is "INFO|WARNING|ERROR|ALARM").
		 *
		 * @see setLogLevel for more information about valid LogLevels.
		 */
		unsigned int mLogLevel;

		/**
		 * Pointer to the FeeClientLibInterface to call the handler routine.
		 * This interface has to be registered by the appropriated function.
		 */
		FeeClientLibInterface* mpInterface;


};  // end of class

// inline for checking LogLevel for local events
inline bool FeeClientLogger::checkLogLevel(unsigned int event) const {
    // typically Eric construct !
    return (event & mLogLevel) ? true : false;
}

} } // end of namespace

#endif
