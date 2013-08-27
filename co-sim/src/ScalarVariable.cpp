#include "ScalarVariable.h"

namespace ScalarVariableAttributes
{
	Variability getVariability( const std::string& str ) {
		if ( str == "continuous" ) return continuous;
		if ( str == "discrete" ) return discrete;
		return constant;
	}

	Causality getCausality( const std::string& str ) {
		if ( str == "input" ) return input;
		if ( str == "output" ) return output;
		if ( str == "internal" ) return internal;
		return none;
	}
}
