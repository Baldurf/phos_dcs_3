#ifndef DCS_FEE_FEE_ITEM_INFO_HPP
#define DCS_FEE_FEE_ITEM_INFO_HPP

#include "FeeInfo.hpp"
#include "FeeServiceStruct.hpp"

namespace dcs { namespace fee {

/**
 * Value, when link to an Item-Service is not available.
 */
#define FEE_ITEM_NO_LINK -1000.0

/**
 * Define for timeout, when item is at least updated.
 * (in seconds, polled by client).
 */
#define FEE_ITEM_TIMEOUT 10


/**
 * @class FeeItemInfo
 * Class, to subscribe and get access to the FEE - Item - service.
 * It inheritates from FeeInfo to get the DimInfo features, and to enable a
 * polymorph access to the service data. Create objects of FeeItemInfo to get
 * a connection to a given FEE Item on the server. The value is also updated
 * after a certain timeout.
 *
 * @see FeeInfo for more details
 *
 * @author Christian Kofler, Sebastian Bablok
 *
 * @date 2003-08-27 (changed for LIB 2005-10-05)
 *
 * @ingroup corelib
 */
class FeeItemInfo : public FeeInfo {
	public:
		/**
		 * Constructor for the FeeItemInfo.
		 * This constructor calls implictly the FeeInfo - constructor and with
		 * that the DimInfo constructor, so you subsrcibe to the inserted
		 * FEE - Item - Service (name).
		 *
		 * @param name name of the service, to subscribe to
		 * @param time time interval, when to update service
		 * @param noLink value to be shown, when no connection to this service
		 *			is available
		 * @param pHandler pointer to the DimInfoHandler
		 */
		//!!!this constructor causes errors on DCS-Board when using DIM-Timer!!!
		/*FeeItemInfo(char* name, int time, float noLink, DimInfoHandler* pHandler)
				: mValue(FEE_ITEM_NO_LINK), mpFedService(NULL),
				FeeInfo(name, time, noLink, pHandler) {};*/

		/**
         * Constructor for the FeeItemInfo, without service request after time
         * intervals.
         * This constructor calls implictly the FeeInfo - constructor and with
         * that the DimInfo constructor, so you subsrcibe to the inserted
         * FEE - Item - Service (name).
         *
         * @param fss pointer to the FeeServiceStruct, containing the name of
         * 		the service to subscribe to and pointer to the handler routine
         * @param noLink value to be shown, when no connection to this service
         *       is available
         * @param pHandler pointer to the DimInfoHandler
         */
		FeeItemInfo(FeeServiceStruct& fss, float noLink, DimInfoHandler* pHandler)
               : FeeInfo(fss.name, noLink, pHandler), mValue(FEE_ITEM_NO_LINK),
               mServiceInfo(fss) {};

		/**
		 * Destructor for the FeeItemInfo objects
		 */
		virtual ~FeeItemInfo() {};

		/**
		 * Function, to be executed when new data from the FEE - Item - Service
		 * is available. It retrieves the data of the FeeServer from the DIM
		 * framework. It stores the value in the object itself. To get the value
		 * the the getValue function.
		 *
		 * @return true, if service is available, false in case of "no link"
		 */
		virtual bool retrieveServiceValue();

		/**
		 * Function to set the initial value of this ItemInfo. A possible
		 * "No link" at start up can be determined this way.
		 * This function should be called once after constructing this object.
		 */
		virtual void initialize();

		/**
		 * Getter for the reference to the service value.
		 *
		 * @return reference to the current value of this service
		 */
		float& getRefFloatValue();



	private:
		/**
		 * Here is the value of this Service stored.
		 */
		float mValue;

		/**
		 * Service info containing the handler/callback routine for the
		 * FeeClientLib interface of this FeeService and the service name.
		 */
		FeeServiceStruct mServiceInfo;


}; // end class

inline float& FeeItemInfo::getRefFloatValue() {
	return mValue;
}


} } // end namespace

#endif
