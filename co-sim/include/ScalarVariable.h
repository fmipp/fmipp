/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file ScalarVariable.h


#ifndef _SCALAR_VARAIABLE_H
#define _SCALAR_VARAIABLE_H

#include <cstring>
#include <string>

#include "fmiPlatformTypes.h"

#define SCALAR_VARIABLE_MAX_NAME_LENGTH 128


/// Contains helper functions to handle struct ScalarVariable.
namespace ScalarVariableAttributes
{
	enum Variability { constant, discrete, continuous };
	enum Causality { input, output, internal, none };

	Variability getVariability( const std::string& str );
	Causality getCausality( const std::string& str );
}


/**
 * \class ScalarVariable ScalarVariable.h
 * Structure for storing information about FMI model variables.
 *
 * Includes information about name, type, value reference, causality and variability.
 */
template<class T>
class ScalarVariable
{

public:

	char name_[SCALAR_VARIABLE_MAX_NAME_LENGTH];

	T value_;

	fmiValueReference valueReference_;

	ScalarVariableAttributes::Causality causality_;
	ScalarVariableAttributes::Variability variability_;

	bool setName( const std::string& name ) {
		return setName( name.c_str(), name.size() + 1 );
	}

	bool setName( const char* name, unsigned int length ) {
		bool result = false;
		if ( length < SCALAR_VARIABLE_MAX_NAME_LENGTH ) {
#ifdef _MSC_VER
			strcpy_s( &name_[0], length, name );
#else
			strcpy( &name_[0], name );
#endif
			result = true;
		}
		return result;
	}

};



#endif // _SCALAR_VARAIABLE_H
