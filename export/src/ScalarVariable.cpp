// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file ScalarVariable.cpp

#include "export/include/ScalarVariable.h"


namespace ScalarVariableAttributes
{
	Variability::Variability getVariability( const std::string& str ) {
		if ( str == "continuous" ) return Variability::continuous; // FMI 1.0 & 2.0 (default)
		if ( str == "discrete" )   return Variability::discrete;   // FMI 1.0 & 2.0
		if ( str == "constant" )   return Variability::constant;   // FMI 1.0 & 2.0
		if ( str == "parameter" )  return Variability::parameter;  // FMI 1.0 only
		if ( str == "fixed" )      return Variability::fixed;      // FMI 2.0 only
		if ( str == "tunable" )    return Variability::tunable;    // FMI 2.0 only
		return Variability::continuous;
	}

	Variability::Variability defaultVariability() {
		return Variability::continuous;
	}

	Causality::Causality getCausality( const std::string& str ) {
		if ( str == "input" )       return Causality::input;       // FMI 1.0 & 2.0
		if ( str == "output" )      return Causality::output;      // FMI 1.0 & 2.0
		if ( str == "internal" )    return Causality::internal;    // FMI 1.0 only
		if ( str == "none" )        return Causality::none;        // FMI 1.0 only
		if ( str == "parameter" )   return Causality::parameter;   // FMI 2.0 only
		if ( str == "calculatedParameter" ) return Causality::calculatedParameter; // FMI 2.0 only
		if ( str == "local" )       return Causality::local;       // FMI 2.0 only (default)
		if ( str == "independent" ) return Causality::independent; // FMI 2.0 only
		return Causality::local;
	}
	
	Causality::Causality defaultCausality() {
		return Causality::local;
	}
}
