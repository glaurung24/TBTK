#ifndef COM_DAFER45_TBTK_OUTOFDEVICEMEMORY_EXCEPTION
#define COM_DAFER45_TBTK_OUTOFDEVICEMEMORY_EXCEPTION

#include "TBTK/Exception.h"

#include <string>

namespace TBTK{

class OutOfDeviceMemoryException : public Exception{
public:
	/** Constructor. */
	OutOfDeviceMemoryException();

	/** Constructor. */
	OutOfDeviceMemoryException(
		const std::string& function,
		const std::string& where,
		const std::string& message,
		const std::string& hint
	);

	/** Destructor. */
	virtual ~OutOfDeviceMemoryException();
private:
};

};	//End of namespace TBTK

#endif
