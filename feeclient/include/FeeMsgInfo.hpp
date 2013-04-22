#ifndef DCS_FEE_FEE_MSG_INFO_HPP
#define DCS_FEE_FEE_MSG_INFO_HPP

#include "FeeInfo.hpp"
#include "MessageStruct.hpp"


namespace dcs { namespace fee {


/**
 * @class FeeMsgInfo
 * Class, to subscribe and get access to the FEE - MSG - service.
 * It inheritates from FeeInfo to get the DimInfo features, and to enable a
 * polymorph access to the service data. Create objects of FeeAckInfo to get
 * a connection to a given FEE - MSG - service.
 *
 * @see FeeInfo for more details
 *
 * @author Christian Kofler, Sebastian Bablok
 *
 * @date 2004-04-05 (changed for LIB 2005-10-05)
 *
 * @ingroup corelib
 */
class FeeMsgInfo : public FeeInfo {
	public:
		/**
		 * Constructor for the FeeMsgInfo.
		 * This constructor calls implictly the FeeInfo - constructor and with
		 * that the DimInfo constructor, so you subsrcibe to the inserted
		 * FEE - MSG- Service (name).
		 *
		 * @param name name of the service, to subscribe to
		 * @param noLink pointer to value to be shown, when no connection to
		 *			this service is available
		 * @param pHandler pointer to the DimInfoHandler
		 * @param serverName the name of the corresponding FeeServer
		 */
		FeeMsgInfo(char* name, void* noLink, DimInfoHandler* pHandler,
                char* serverName) : FeeInfo(name, noLink, sizeof(MessageStruct),
                pHandler), mpFeeMessage(NULL) {
			mpServerName = new char[strlen(serverName) + 1];
			strcpy(mpServerName, serverName);
		};


		/**
		 * Destructor for the FeeMsgInfo object
		 */
		virtual ~FeeMsgInfo();

		/**
		 * Function, to be executed when new data from the FEE - MSG - Service
		 * is available. It retrieves the data of the FeeServer from the DIM
		 * framework. It stores the value in the object itself.
		 *
		 * @return true, if service is available, false in case of "no link"
		 */
		virtual bool retrieveServiceValue();

		/**
		 * Function to set the initial state of the link for the MSG - Service.
		 * This should be called to determine, if the link is currently
		 * available and should be called once after constructing this object.
		 */
		virtual void initialize();


    private:
        /**
         * Stores the current message of a FeeServer in a Message Struct on the
         * FeeClient side.
         */
        MessageStruct* mpFeeMessage;

		/**
		 * Here is the name of the corresponding FeeServer stored.
		 */
		char* mpServerName;


}; // end class


} }// end namespace

#endif

