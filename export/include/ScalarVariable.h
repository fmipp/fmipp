// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file ScalarVariable.h

#ifndef _FMIPP_SCALARVARIABLE_H
#define _FMIPP_SCALARVARIABLE_H

#include <cstring>

#include "common/FMIPPConfig.h"

#define SCALAR_VARIABLE_MAX_NAME_LENGTH 128

/// Contains helper functions to handle struct ScalarVariable.
namespace ScalarVariableAttributes
{
	namespace Variability {

		/// Enumeration of possible values for variability of scalar variables.
		enum Variability {
			constant,   // FMI 1.0 & 2.0
			discrete,   // FMI 1.0 & 2.0
			continuous, // FMI 1.0 & 2.0 (default)
			parameter,  // FMI 1.0 only
			fixed,      // FMI 2.0 only
			tunable     // FMI 2.0 only
		};
	
	}

	namespace Causality {

		/// Enumeration of possible values for causality of scalar variables.
		enum Causality {
			input,          // FMI 1.0 & 2.0
			output,         // FMI 1.0 & 2.0 
			internal,       // FMI 1.0 only
			none,           // FMI 1.0 only
			parameter,      // FMI 2.0 only
			calculatedParameter, // FMI 2.0 only
			local,          // FMI 2.0 only (default)
			independent     // FMI 2.0 only
		};
	}
		
	/// Parse string and retrieve enum value for variability.
	Variability::Variability getVariability( const std::string& str );

	/// Get default value for variability (according to FMI 2.0).
	Variability::Variability defaultVariability();

	/// Parse string and retrieve enum value for causality.
	Causality::Causality getCausality( const std::string& str );

	/// Get default value for causality (according to FMI 2.0).
	Causality::Causality defaultCausality();
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

	template<typename Alloc>
	ScalarVariable( Alloc a ) : value_( T( a ) ) {}

	ScalarVariable() : value_( T() ) {}

	fmippChar name_[SCALAR_VARIABLE_MAX_NAME_LENGTH];

	T value_;

	fmippValueReference valueReference_;

	ScalarVariableAttributes::Causality::Causality causality_;
	ScalarVariableAttributes::Variability::Variability variability_;

	bool setName( const std::string& name ) {
		return setName( name.c_str(), name.size() + 1 );
	}

	bool setName( const char* name, size_t length ) {
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

#endif // _FMIPP_SCALARVARIABLE_H
