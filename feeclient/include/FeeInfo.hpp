#ifndef DCS_FEE_FEE_INFO_HPP
#define DCS_FEE_FEE_INFO_HPP

#include "dim/dic.hxx"

namespace dcs { namespace fee {


/**
 * @class FeeInfo
 * Class, that wrapps the DimInfo - objects and offers a polymorph call
 * to retrieve the service value from the DIM framework. It inheritates the DIM
 * features from DimInfo. To get the polymorph access, you need to instanciate
 * objects from inheritated classes from FeeInfo.
 *
 * @author Christian Kofler, Sebastian Bablok
 *
 * @date 2003-08-27 (changed for LIB 2005-10-05)
 *
 * @ingroup corelib
 */
class FeeInfo : public DimInfo {
	public:
		/**
		 * Constructor for the FeeInfo, when you want access to the Fee - ACK -
		 * Service. Normally, you will never call this constructor, use instead
		 * the FeeAckInfo, where this constructor is called implicitly. This
		 * constructor calls the DimInfo constructor where you automatically
		 * subscribe to inserted service (name).
		 *
		 * @param name name of the service, to subscribe to
		 * @param noLink pointer to value to be shown, when no connection to this
		 *			service is available
		 * @param noLinkSize size of the noLink - value
		 * @param pHandler pointer to the DimInfoHandler
		 */
		FeeInfo(char* name, void* noLink, int noLinkSize, DimInfoHandler* pHandler)
				: DimInfo(name, noLink, noLinkSize, pHandler) {};

		/**
		 * Constructor for the FeeInfo, when you want access to the Fee - Item -
		 * Service. Normally, you will never call this constructor, use instead
		 * the FeeItemInfo, where this constructor is called implicitly. This
		 * constructor calls the DimInfo constructor where you automatically
		 * subscribe to inserted service (name).
		 *
		 * @param name name of the service, to subscribe to
		 * @param time time interval, when to update service
		 * @param noLink value to be shown, when no connection to this service
		 *			is available
		 * @param pHandler pointer to the DimInfoHandler
		 */
		FeeInfo(char* name, int time, float noLink, DimInfoHandler* pHandler)
				: DimInfo(name, time, noLink, pHandler) {};

		/**
        * Constructor for the FeeInfo, when you want access to the Fee - Item -
        * Service, here without regular service request after certain time
        * intervals.
        * Normally, you will never call this constructor, use instead
        * the FeeItemInfo, where this constructor is called implicitly. This
        * constructor calls the DimInfo constructor where you automatically
        * subscribe to inserted service (name).
        *
        * @param name name of the service, to subscribe to
        * @param noLink value to be shown, when no connection to this service
        *          is available
        * @param pHandler pointer to the DimInfoHandler
        */
       FeeInfo(char* name, float noLink, DimInfoHandler* pHandler)
               : DimInfo(name, noLink, pHandler) {};

		/**
		 * Destructor for the FeeInfo objects.
		 * It is virtual, so the destructor of the derived class is also called,
		 * if the object is of this type (see Effective C++, page 60).
		 */
		virtual ~FeeInfo();

		/**
		 * Function, that executes polymorph the action, that should be performed
		 * when new data is retrieved from the FeeServer.
		 * This function is pure virtual, because it should only be called from
		 * objects of the inherited classes. It stores the value in the object
		 * itself.
		 *
		 * @return true, if service is available, false in case of "no link"
		 */
		virtual bool retrieveServiceValue() = 0;

		/**
		 * Function to initialze the value of the info - object.
		 * This function is virtual, because the implementation depends on the
		 * kind of DimInfo - object.
		 */
		virtual void initialize() = 0;


	private:



}; // end class



} } // end namespace
#endif
