#ifndef DCS_FEE_FEE_ACK_INFO_HPP
#define DCS_FEE_FEE_ACK_INFO_HPP

#include "FeeInfo.hpp"

namespace dcs { namespace fee {

/**
 * Value, when link to an ACK-Service is not available.
 */
#define FEE_ACK_NO_LINK "No Link"
/**
 * Size of "No Link" - value for ACK-Service, NOT inclusive '\\0' .
 */
#define FEE_ACK_NO_LINK_SIZE 7


/*
 * The area -70 to -79 is especially reserved for FeeAckInfos.
 * So use these numbers here. For OK use always '0'.
 */

/**
 * Define for the FEE - acknowledge info, when link is ok.
 */
#define FEE_ACK_LINK_OK 0
/**
 * Define for the FEE - acknowledge info, when link is lost.
 */
#define FEE_ACK_LOST_LINK -71
/**
 * Define for the FEE - acknowledge info, when strange data has been received.
 */
#define FEE_ACK_STRANGE_DATA -72


class FeeClientLibInterface;

/**
 * @class FeeAckInfo
 * Class, to subscribe and get access to the FEE - ACK - service.
 * It inheritates from FeeInfo to get the DimInfo features, and to enable a
 * polymorph access to the service data. Create objects of FeeAckInfo to get
 * a connection to a given FEE - ACK - service.
 *
 * @see FeeInfo for more details
 *
 * @author Christian Kofler, Sebastian Bablok
 *
 * @date 2003-08-27 (changed for LIB 2005-10-05 (SB))
 *
 * @ingroup corelib
 */
class FeeAckInfo : public FeeInfo {
	public:
		/**
		 * Constructor for the FeeAckInfo.
		 * This constructor calls implictly the FeeInfo - constructor and with
		 * that the DimInfo constructor, so you subscribe to the inserted
		 * FEE - ACK- Service (name).
		 *
		 * @param name name of the service, to subscribe to
		 * @param noLink pointer to value to be shown, when no connection to
		 *			this service is available
		 * @param noLinkSize size of the noLink - value
		 * @param pHandler pointer to the DimInfoHandler
		 * @param serverName the name of the corresponding FeeServer
		 * @param interface pointer to the interface to call the handler routine
		 */
		FeeAckInfo(char* name, void* noLink, int noLinkSize,
				DimInfoHandler* pHandler, char* serverName,
				FeeClientLibInterface* interface)
				: FeeInfo(name, noLink, noLinkSize, pHandler),
                ackState(FEE_ACK_LOST_LINK), mpInterface(interface) {
			mpServerName = new char[strlen(serverName) + 1];
			strcpy(mpServerName, serverName);
		};

		/**
		 * Destructor for the FeeAckInfo objects
		 */
		virtual ~FeeAckInfo();

		/**
		 * Function, to be executed when new data from the FEE - ACK - Service
		 * is available. It retrieves the data of the FeeServer from the DIM
		 * framework. It stores the value in the object itself.
		 *
		 * @return true, if service is available, false in case of "no link"
		 */
		virtual bool retrieveServiceValue();

		/**
		 * Function to set the initial state of the link for the ACK-Service.
		 * This should be called to determine, if the link is currently
		 * available and should be called once after constructing this object.
		 */
		virtual void initialize();

        /**
         * Gets a pointer to the server name of the corresponding FeeServer for
         * this DimInfo object.
         *
         * @return the corresponding server name
         */
         const char* getCorrespondingServerName() const;

         /**
          * Indicates if the FeeClient has "link" to the server corresponding
          * to this FeeAckInfo object.
          *
          * @return false if no-link, true otherwise
          */
         bool hasLink();


	private:
		/**
		 * Here is the ACK value stored.
		 */
		int ackState;

		/**
		 * Here is the name of the corresponding FeeServer stored.
		 */
		char* mpServerName;

		/**
		 * Pointer to the FeeClientLibInterface to call the handler routine.
		 */
		FeeClientLibInterface* mpInterface;


}; // end class

inline const char* FeeAckInfo::getCorrespondingServerName() const {
    return mpServerName;
}

inline bool FeeAckInfo::hasLink() {
    return (ackState != FEE_ACK_LOST_LINK) ? true : false;
}

} }// end namespace

#endif

