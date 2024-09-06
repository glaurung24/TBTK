#include "TBTK/OutOfDeviceMemoryException.h"

namespace TBTK{

OutOfDeviceMemoryException::OutOfDeviceMemoryException(){
}

OutOfDeviceMemoryException::OutOfDeviceMemoryException(
	const std::string& function,
	const std::string& where,
	const std::string& message,
	const std::string& hint
) : Exception(function, where, message, hint){
}

OutOfDeviceMemoryException::~OutOfDeviceMemoryException(){
}

};	//End of namespace TBTK
