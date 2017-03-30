/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _POWER_FACTORY_REAL_SCALAR_H
#define _POWER_FACTORY_REAL_SCALAR_H


#include "export/include/ScalarVariable.h"

namespace api {
	namespace v1 {
		class DataObject;
	}
}


/**
 * \file PowerFactoryRealScalar.h
 *
 * \class PowerFactorsRealScalar PowerFactorsRealScalar.h
 * Class for storing information about PowerFactory model variables according to FMI specification.
 *
 * Includes information about PF class name, PF object name, PF parameter name, and
 * FMI-related information, such as value reference, causality and variability. Also
 * used to store information needed for sending events to RMS simulations.
 */


class PowerFactoryRealScalar
{

public:

	// Information related to FMI:

	fmiValueReference valueReference_; ///< FMI value reference.

	ScalarVariableAttributes::Causality causality_; ///< FMI variable causality.
	ScalarVariableAttributes::Variability variability_; ///< FMI variable causality.


	// Information related to PF:

	std::string className_; ///< Class name of the associated PF object.
	std::string objectName_; ///< Object name of the associated PF object.
	std::string parameterName_; ///< Parameter name of the associated PF object.

	/// This flag indicates whether this model variable is related to an input event (RMS simulation).
	bool isRMSEvent_;
	
	api::v1::DataObject* apiDataObject_; ///< Link to PF object.

	/** 
	 * Extract and parse information abaout PowerFactory variables. Variable names 
	 * are supposed to be of the form "<class-name>.<object-name>.<parameter-name>"
	 * or "FMIEvent.<parameter-name>"
	 */
	static bool parseFMIVariableName( const std::string& name,
					  std::string& className,
					  std::string& objectName,
					  std::string& parameterName,
					  bool& isRMSEvent );

};



#endif // _POWER_FACTORY_REAL_SCALAR_H
