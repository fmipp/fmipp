#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "PowerFactoryRealScalar.h"


bool
PowerFactoryRealScalar::parseFMIVariableName( const std::string& name,
					      std::string& className,
					      std::string& objectName,
					      std::string& parameterName )
{
	using namespace boost;

	// Split string, use "."-character as delimiter.
	std::vector<std::string> strs;
	split( strs, name, is_any_of(".") );

	// Require three resulting strings (class name, object name, parameter name).
	if ( 3 == strs.size() )
	{
		className = algorithm::trim_copy( strs[0] );
		objectName = algorithm::trim_copy( strs[1] );
		parameterName = algorithm::trim_copy( strs[2] );
		return true;
	}

	return false;
}
