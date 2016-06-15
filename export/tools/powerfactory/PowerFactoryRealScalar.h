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
 * FMI-related information, such as value reference, causality and variability.
 */


class PowerFactoryRealScalar
{

public:

	// Information related to FMI:

	fmiValueReference valueReference_;

	ScalarVariableAttributes::Causality causality_;
	ScalarVariableAttributes::Variability variability_;


	// Information related to PF:

	std::string className_;
	std::string objectName_;
	std::string parameterName_;

	api::v1::DataObject* apiDataObject_;


	/** Extract and parse information abaout PowerFactory
	 *  variables. Variable names are supposed to be of the
	 *  form "<class-name>.<object-name>.<parameter-name>".
	 */
	static bool parseFMIVariableName( const std::string& name,
					  std::string& className,
					  std::string& objectName,
					  std::string& parameterName );

};



#endif // _POWER_FACTORY_REAL_SCALAR_H
