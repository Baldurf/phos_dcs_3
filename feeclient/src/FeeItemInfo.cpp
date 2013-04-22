//#include "FeeItemInfo.hpp"
#include "feeclient/include/FeeItemInfo.hpp"


/*
#ifdef __DEBUG
#include <iostream>
using namespace std;
#endif
*/

using namespace dcs::fee;


bool FeeItemInfo::retrieveServiceValue() {
	// save value from current->getFloat()
	mValue = getFloat();

	// trigger update in LIB callback/handler routine
	mServiceInfo.handlerRoutine(mValue);

/*
#   ifdef __DEBUG
	cerr << "incoming data (float): " << getFloat() << endl;
	cerr << endl;
#   endif
*/

	if (mValue == FEE_ITEM_NO_LINK) {
		return false;
	}
	return true;
}

void FeeItemInfo::initialize() {
	mValue = getFloat();
}


