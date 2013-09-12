/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _SCALAR_VARAIABLE_H
#define _SCALAR_VARAIABLE_H

#include <cstring>
#include <string>

#include "fmiPlatformTypes.h"

#define SCALAR_VARIABLE_MAX_NAME_LENGTH 128

namespace ScalarVariableAttributes
{
	enum Variability { constant, discrete, continuous };
	enum Causality { input, output, internal, none };

	Variability getVariability( const std::string& str );
	Causality getCausality( const std::string& str );
}


template<class T>
class ScalarVariable
{

public:

	char name_[SCALAR_VARIABLE_MAX_NAME_LENGTH];

	T value_;
	//double value_;

	fmiValueReference valueReference_;

	ScalarVariableAttributes::Causality causality_;
	ScalarVariableAttributes::Variability variability_;

	bool setName( const std::string& name ) {
		return setName( name.c_str(), name.size() );
	}

	bool setName( const char* name, unsigned int length ) {
		bool result = false;
		if ( length < SCALAR_VARIABLE_MAX_NAME_LENGTH ) {
			strcpy( &name_[0], name );
			result = true;
		}
		return result;
	}

};



#endif // _SCALAR_VARAIABLE_H
