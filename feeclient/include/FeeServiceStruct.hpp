#ifndef DCS_FEE_FEESERVICE_STRUCT_HPP
#define DCS_FEE_FEESERVICE_STRUCT_HPP

#include <string>

namespace dcs { namespace fee {

/**
 * @class FeeServiceStruct
 * FeeService structs contain the name of a FeeService and the pointer to the
 * corresponding handler function.
 *
 * @author Sebastian Bablok
 *
 * @date 2005-10-06
 *
 * @ingroup corelib
 */
struct FeeServiceStruct {
	/**
	 * Name of the FeeService
	 */
	char* name;

	/**
	 * Pointer to the corresponding handler function
	 *
	 * @param floatVal the value of the updated service
	 */
	void (*handlerRoutine)(float floatVal);

	/**
	 * Constructor for a FeeServiceStruct
	 */
	FeeServiceStruct() {
		name = 0;
	}

	/**
	 * Copy constructor for a FeeService struct
	 *
	 * @param fss the FeeService struct from which the copy shall be made.
	 */
	FeeServiceStruct(const FeeServiceStruct& fss) {
		name = new char[strlen(fss.name) + 1];
		strcpy(name, fss.name);
		handlerRoutine = fss.handlerRoutine;
	}

	/**
	 * Assignment operator for a FeeService struct
	 *
	 * @param rhs the FeeService struct which shall be assigned from.
	 */
	FeeServiceStruct& operator=(const FeeServiceStruct& rhs) {
		if (this == &rhs) {
			return *this;
		}
		if (name != 0) {
			delete[] name;
		}
		name = new char[strlen(rhs.name) + 1];
		strcpy(name, rhs.name);
		handlerRoutine = rhs.handlerRoutine;
		return *this;
    }

    /**
	 * Destructor for a FeeServiceStruct
	 */
	~FeeServiceStruct() {
		if (name != 0) {
			delete[] name;
		}
	}

};


} }

#endif
