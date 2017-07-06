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
	using namespace std;

	// Split string, use "."-character as delimiter.
	std::vector<std::string> strs;
	split( strs, name, is_any_of(".") );

	if ( 3 != strs.size() ) return false;

	// Check if this is an event (used for RMS simulations).
	isRMSEvent = ( string::npos != strs[0].find( "EvtParam" ) );

	className = algorithm::trim_copy( strs[0] );
	objectName = algorithm::trim_copy( strs[1] );
	parameterName = algorithm::trim_copy( strs[2] );

	return true;
}
