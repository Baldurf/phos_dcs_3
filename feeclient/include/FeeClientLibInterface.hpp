#ifndef DCS_FEE_FEE_CLIENT_LIB_INTERFACE_HPP
#define DCS_FEE_FEE_CLIENT_LIB_INTERFACE_HPP

#include "MessageStruct.hpp"
#include "FeePacket.hpp"
#include "LockGuard.hpp"

#include <vector>
#include <map>
#include <pthread.h>

#include <stdio.h>

namespace dcs { namespace fee {

class FeeServiceStruct;
/**
 * Define for error code, if the FeeClientCore is in a wrong state for a
 * choosen interface call.
 */
#define FEE_CLIENT_WRONG_STATE -1

/**
 * Define for FeeClientCore has not been started yet.
 */
#define FEE_CLIENT_NOT_STARTED 0

// avoids to include FeeClientCore header file here, only needed in the cpp file
class FeeClientCore;

/**
 * @class FeeClientLibInterface 
 * This virtual class presentes the interface of the FeeClient - library.
 * It has to be derived in an child class and the pure virtual functions have
 * to be implemented in order to use the interface. In addition the user has
 * to provide a handler routine for each FeeService, he subscribes to. This
 * handler gives updated service values to the user. And the names of the
 * FeeServers have to be given to the library. After this registration the
 * user can start the FeeClient and send commands to a dedicated FeeServer.
 *
 * @see registerFeeServerName for more information about registering FeeServers
 * @see registerServiceName for more information about registering FeeServices
 * @see startFeeClient for more information about starting the FeeClient
 * @see messageHandler for more information on how to receive messages.
 * @see acknowledgeHandler for more information on how to receive ACK data.
 * @see sendCommand for more information about sending commands.
 * @see stopFeeClient for more information about stopping the FeeClient.
 * @see clearFeeClient for more information about cleaning up.
 *
 * @author Sebastian Bablok (Sebastian.Bablok@ift.uib.no)
 *
 * @date 2005-10-05
 *
 * @ingroup corelib
 */
class FeeClientLibInterface {
	public:
		/**
		 * Constructor for the client interface
		 */
		FeeClientLibInterface();

		/**
		 * Destructor for the client interface
		 */
		virtual ~FeeClientLibInterface();

		/**
		 * This handler routine informs about new acknowledge from a FeeServer
		 * and offers the received data. It has to be implemented in the
		 * derived classes to get the ACK. <br>
		 * NOTE: First the ackState has to be checked, it is possible, that
		 * some of the parameters are NULL, depending on the state.
		 * <br> <b>[pure virtual]</b>
		 * <br><br>
		 * <b>Structure of the FlageBits:</b>
		 * <pre>
		 * NO_FLAGS 0x0000                          // dec 0
		 * CHECKSUM_FLAG 0x0002                     // dec 2
		 * FEESERVER_UPDATE_FLAG 0x0004             // dec 4
		 * FEESERVER_RESTART_FLAG 0x0008            // dec 8
		 * FEESERVER_REBOOT_FLAG 0x0010             // dec 16
		 * FEESERVER_SHUTDOWN_FLAG 0x0020           // dec 32
		 * FEESERVER_EXIT_FLAG 0x0040               // dec 64
		 * FEESERVER_SET_DEADBAND_FLAG 0x0080       // dec 128
		 * FEESERVER_GET_DEADBAND_FLAG 0x0100       // dec 256
		 * FEESERVER_SET_ISSUE_TIMEOUT_FLAG 0x0200  // dec 512
		 * FEESERVER_GET_ISSUE_TIMEOUT_FLAG 0x0400  // dec 1024
		 * FEESERVER_SET_UPDATERATE_FLAG 0x0800     // dec 2048
		 * FEESERVER_GET_UPDATERATE_FLAG 0x1000     // dec 4096
		 * FEESERVER_SET_LOGLEVEL_FLAG 0x2000       // dec 8192
		 * FEESERVER_GET_CONFIGURATION_FLAG 0x4000  // dec 16384
		 * --------------------------------------------------------------------
		 * </pre><br>
		 * <b>Data structure in case of an ACK to a getter command:</b>
		 * <br>
		 * <i>FEESERVER_GET_DEADBAND_FLAG:</i><br>
		 * (gets deadband of a specified service value) <br>
		 * <pre>
		 * -> { deadband[sizeof(float))] | servicename of deadband[rest of data] }
		 * </pre>
		 *
		 * <i>FEESERVER_GET_ISSUE_TIMEOUT_FLAG:</i><br>
		 * (gets timeout for execution of commands inside the control engine in
		 * ms) <br>
		 * <pre>
		 * -> { issue timeout [sizeof(unsigned long)] }
		 * </pre>
		 *
		 * <i>FEESERVER_GET_UPDATERATE_FLAG: </i><br>
		 * (gets update rate of the deadband checker for service values in ms)
		 * <br>
		 * <pre>
		 * -> { update rate [sizeof(unsigned short)] }
		 * </pre>
		 *
		 * <i>FEESERVER_GET_CONFIGURATION_FLAG:</i><br>
		 * (this structure has not implemented yet, but it consists of a
		 * summary of the above menmtioned configuration settings.)
 		 * <br>
		 *
		 * @param serverName name of the Server, which has received this ack
		 * @param size the size of the received acknowledge data
		 * @param data pointer to the acknowledge data (pass by pointer)
		 * @param id the id of the corresponding command to this ack
		 * @param flags informs about the received flags of the ack data,
		 *			NOTE: these are not necessarily the same like the command
		 * @param errorCode passes a possible error code inside the FeePacket
		 *			header to the handler.
		 * @param ackState passes the state of the Acknowledge: <br>
		 *			FEE_OK 				 - Link available, FeePacket valid
		 *			<br>
		 *			FEE_ACK_CORRUPTED	 - Link available, FeePacket to small
		 *			-> their is NO DATA<br>
		 *			FEE_CHECKSUM_FAILED  - Link available, FeePacket checksum
		 *			failed, but data available <br>
		 * 			FEE_ACK_LINK_DOWN	 - no Link available, NO DATA <br>
		 *
		 * @see feepacket_flags.h for more information
		 * @see fee_errors.h for more information
		 */
		virtual void acknowledgeHandler(char* serverName, unsigned int size,
						char* data, unsigned int id, FlagBits flags,
						short errorCode, short ackState) = 0;

		/**
		 * This handler routine informs about new received log message (remote
		 * and local). It has to be implemented in the derived classes, the
		 * message can be retrieved via the parameter msg.
		 * <br> <b>[pure virtual]</b>
		 *
		 * @param msg this message struct contains the current log message for
		 *			this handler routine. (pass by value)
		 */
		virtual void messageHandler(const MessageStruct msg) = 0;

		/**
		 * This function registers a FeeServer name to which the
		 * FeeClient should connect to. This is only possible during init
		 * state, registering afterwards will return an error. If the name
		 * already exists in the local vector, no additional copy is included.
		 *
		 * @param serverName FeeServer name, for the FeeClient to connect to.
		 *
		 * @return false, if the FeeClient has already been connected to the
		 *			FeeServers (or if serverName is NULL).
		 */
		bool registerFeeServerName(const char* serverName);

		/**
		 * This function registers a FeeService to which the FeeClient should
		 * connect to. It needs the name of the desired service and has to
		 * register the corresponding handler function. This is only possible
		 * during init state, registering afterwards will return an error. If
		 * the name already exists in the local vector, no additional copy is
		 * included.
		 *
		 * @param serviceName FeeServer name, for the FeeClient to connect to.
		 * @param serviceHandler() function pointer to the corresponding
		 * 			handler, the float value, it accepts, is the new value of
		 * 			an updated service.<br><b>NOTE:</b> This function has to be
		 *			implemented by the user of the FeeClient - library and
		 *			its pointer passed to registerServiceName(...). Each time
		 *			the value of this service is updated, the new value is
		 *			passed to the user via this function.
		 *
		 * @return false, if the FeeClient has already been connected to the
		 *			FeeServers (or if serviceName or function pointer is NULL).
		 */
		bool registerServiceName(const char* serviceName,
						void (*serviceHandler)(float fVal));

		/**
		 * Starts the FeeClient.
		 * It subscibes to all registered FeeServers and their registered
		 * service. The amount of subscribed services is returned.<br>
		 * NOTE: The amount contains an ACK and a message channel per FeeSever
		 * + registered services.<br>
		 * NOTE: A negative number (<b>FEE_CLIENT_WRONG_STATE</b>) is returned,
		 * if the FeeClient has already been started.
		 *
		 * @return the amount of services, the FeeClient has subscribed to.
		 *			A negative number (<b>FEE_CLIENT_WRONG_STATE</b>) is
		 *			returned, if the FeeClient is in the wrong state.
		 */
		int startFeeClient();

		/**
		 * Function to send a command to a specified FeeServer.
		 * If no FlagBits for FeeServer (core) commands are set, this data is
		 * delivered to the ControlEngine and executed there. Commands to the
		 * FeeServer core (see FlagBits) never reach the ControlEngine and
		 * have a defined structure (see below).
		 * <br><br>
		 * <b>Structure of the FlageBits:</b>
		 * <pre>
		 * NO_FLAGS 0x0000                          // dec 0
		 * CHECKSUM_FLAG 0x0002                     // dec 2
		 * FEESERVER_UPDATE_FLAG 0x0004             // dec 4   (experts only!)
		 * FEESERVER_RESTART_FLAG 0x0008            // dec 8
		 * FEESERVER_REBOOT_FLAG 0x0010             // dec 16
		 * FEESERVER_SHUTDOWN_FLAG 0x0020           // dec 32
		 * FEESERVER_EXIT_FLAG 0x0040               // dec 64
		 * FEESERVER_SET_DEADBAND_FLAG 0x0080       // dec 128
		 * FEESERVER_GET_DEADBAND_FLAG 0x0100       // dec 256
		 * FEESERVER_SET_ISSUE_TIMEOUT_FLAG 0x0200  // dec 512
		 * FEESERVER_GET_ISSUE_TIMEOUT_FLAG 0x0400  // dec 1024
		 * FEESERVER_SET_UPDATERATE_FLAG 0x0800     // dec 2048
		 * FEESERVER_GET_UPDATERATE_FLAG 0x1000     // dec 4096
		 * FEESERVER_SET_LOGLEVEL_FLAG 0x2000       // dec 8192
		 * FEESERVER_GET_CONFIGURATION_FLAG 0x4000  // dec 16384
		 * --------------------------------------------------------------------
		 * </pre><br>
		 * <b>Structure of the FlageBits:</b>
		 * <br>
		 * <i>FEESERVER_UPDATE_FLAG:</i><br>
		 * (this command should be executed only by experts, it updates the
		 * whole FeeServer with a new binary) <br>
		 * <pre>
		 * -> { binary of the new FeeServer [sizeof(data)] }
		 * </pre>
		 *
		 * <i>FEESERVER_SET_DEADBAND_FLAG:</i><br>
		 * (sets deadband of a specified service value) <br>
		 * <pre>
		 * -> { deadband[sizeof(float))] | servicename of deadband[rest of data] }
		 * </pre>
		 *
		 * <i>FEESERVER_GET_DEADBAND_FLAG:</i><br>
		 * (requests deadband of a specified service value) <br>
		 * <pre>
		 * -> { servicename of deadband [sizeof(data)] }
		 * </pre>
		 *
		 * <i>FEESERVER_SET_ISSUE_TIMEOUT_FLAG:</i><br>
		 * (sets timeout for execution of commands inside the control engine in
		 * ms) <br>
		 * <pre>
		 * -> { issue timeout [sizeof(unsigned long)] }
		 * </pre>
		 *
		 * <i>FEESERVER_SET_UPDATERATE_FLAG:</i><br>
		 * (sets update rate of the deadband checker for service values in ms)
		 * <br>
		 * <pre>
		 * -> { update rate [sizeof(unsigned short)] }
		 * </pre>
		 *
		 * <i>FEESERVER_SET_LOGLEVEL_FLAG:</i><br>
		 * (sets log level in the specified FeeServer (MSG_ALARM is always on),
		 * see LogLevels for further details) <br>
		 * <pre>
		 * -> { log level [sizeof(unsigned int)] }
		 * </pre>
		 * (all other flags do not contain any command data; there ack may,
		 * depending on the command).<br>
		 * <b>NOTE:</b> A "0" (<b>FEE_CLIENT_NOT_STARTED</b>) is returned,
		 * if the FeeRootclient has already been started.
		 *
		 * @param serverName name of the Server, to which the command shall be
		 *			sent
		 * @param size the size of the to sent command data
		 * @param data pointer to the to sent command data
		 * @param flags which should be set for this command packet
		 *
		 * @return the id of the command is returned to the interface to
		 *			identify the corresponding acknowledge afterwards.
		 *			A "0" (<b>FEE_CLIENT_NOT_STARTED</b>) is returned,
		 *			if the FeeClient has not been started yet.<br> NOTE:
		 *			<b>FEE_CLIENT_NOT_STARTED</b> is also returned, if the
		 *			given server name is NULL.
		 *
		 * @see feepacket_flags.h for more information
		 * @see fee_loglevels.h for detailed description of log levels
		 */
		unsigned int sendCommand(char* serverName, unsigned int size, char* data,
						FlagBits flags);

		/**
		 * Function to stop the FeeClient. Only the subscribed channels
		 * are deregistered, but the corresponding container are <b> NOT </b>
		 * cleared. A new call of startFeeClient() will result in the same
		 * connections. It is possible to register additional FeeServices and
		 * FeeServer before restarting the FeeClient. <br> To empty the
		 * corresponding containers (clear all subscribed FeeServices and
		 * FeeServers) call clearFeeClient() after stopping the
		 * FeeClient. It is only possible to stop the FeeClient, if it
		 * is in running state.
		 *
		 * @return false, if FeeClient is in the wrong state
		 */
		bool stopFeeClient();

		/**
		 * Function to clear all registered FeeServers and FeeServices. This
		 * can only to executed, if the FeeClient is not in the running
		 * state.
		 *
		 * @return the amount of services + FeeServers, which are deleted.
		 *			A negative number (<b>FEE_CLIENT_WRONG_STATE</b>) is
		 *			returned, if the FeeClient is in the wrong state.
		 */
		int clearFeeClient();

		/**
		 * Sets a pending ACK ID to false -> timeout, ACK no longer expected.
		 * <br>
		 * This function belonmgs as well to the interface and should not be
		 * called by the Lib itself, only by the implementation. The Lib does
		 * not include a timeout mechsnism for now.
		 *
		 * @param id the ID to deactivate
		 *
		 * @return true, if ID is in list and its value true; false, if ID is
		 * 			not in map or its value is already false.
		 */
		bool deactivateIDinList(int id);

		/**
		 * Removes an ACK ID from the pending ID map.
		 * If its value is false, the ACK is already regarded as lost. In
		 * that case only a message will be written to the message channel
		 * and the ACK shall be discarded.
		 * <br><b>
		 * This function is not part of the interface and should only be called
		 * by an FeeAckInfo object.</b>
		 *
		 * @param id the ID, which shall be checked and removed from the
		 *			map
		 *
		 * @return true, if the ID is expected, false, if the ID is not in
		 * 			the map or its value is false (ACK already regarded
		 *			as lost -> discard ACK then).
		 */
		bool checkIDinList(int id);


	private:
		/**
		 * Checks, if a given FeeServer name is already in the local vector.
		 *
		 * @param feeServerName the new name, which shall be checked
		 *
		 * @return false, if the name already exists in the local vector.
		 */
		bool isNewFeeServerName(const char* feeServerName);

		/**
		 * Checks, if a given FeeService name is already in the local vector.
		 *
		 * @param feeServiceName the new name, which shall be checked
		 *
		 * @return false, if the name already exists in the local vector.
		 */
		bool isNewServiceName(const char* feeServiceName);

		/**
		 * Function to insert an ID in the map of pending ACK IDs.
		 * If this ID already exists, its value is set to true, which indicates
		 * that it will be accepted when an ACK with the corresponding ID
		 * arrives. If the value has been false before, the ACK has been
		 * regarded as lost; if the value has been true before, a message will
		 * be written to the message channel (possible reasons: the
		 * FeeClientLib has not been notified about a timeout for this ID, or
		 * a collision might occur, when the mssing ACK arrives in between).
		 * If an ACK arrives with an ID corresponding to a false value, a
		 * message will be send to the message channel as well.
		 *
		 * @param id the ID to insert into map
		 */
		void insertIDtoList(int id);

		/**
		 * Function to lock the ID list mutex.
		 */
		void lockListMutex();

		/**
		 * Function to unlock the ID list mutex.
		 */
		void unlockListMutex();

		/**
		 * Clears the local vector of FeeServer names.
		 */
		void clearFeeServerVector();

		/**
		 * Clears the local vector of FeeService structs.
		 */
		void clearServiceVector();

		/**
		 * The pointer to the FeeClientCore.
		 */
		FeeClientCore* mpFeeClient;

		/**
		 * Vector of FeeServer names for subscribing to at the end of init
		 * process.
		 */
		std::vector<char* > mServerNames;

		/**
		 * Vector of FeeServiceStructs for subscribing to at the end of init
		 * process. The FeeServiceStruct contains the FeeService name and the
		 * pointer to its handler function.
		 */
		std::vector<FeeServiceStruct* > mServiceNames;

		/**
		 * List of pending packet IDs.
		 * This container contains a list of IDs for which an ACK is expected.
		 */
		std::map<int, bool> mIDList;

		/**
		 * Mutex variable to protect the map from concurrent access.
		 */
                Mutex fMutex;


};  // end of class

} } // end of namespace
#endif
