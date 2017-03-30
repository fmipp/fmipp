#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "PowerFactoryRealScalar.h"


bool
PowerFactoryRealScalar::parseFMIVariableName( const std::string& name,
					      std::string& className,
					      std::string& objectName,
					      std::string& parameterName,
					      bool& isRMSEvent )
{
	using namespace boost;

	// Split string, use "."-character as delimiter.
	std::vector<std::string> strs;
	split( strs, name, is_any_of(".") );

	if ( 3 == strs.size() )	{ 	// Require either 3 resulting strings (class name, object name, parameter name) ...
		className = algorithm::trim_copy( strs[0] );
		objectName = algorithm::trim_copy( strs[1] );
		parameterName = algorithm::trim_copy( strs[2] );
		isRMSEvent = false;
		return true;
	} else if ( ( 2 == strs.size() ) && ( std::string::npos != strs[0].find( "FMIEvent" ) ) ) { // ... or 2 strings (RMS input event).
		className = std::string();
		objectName = std::string();
		parameterName = algorithm::trim_copy( strs[1] );
		isRMSEvent = true;
		return true;
	}

	return false;
}
