/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _POWER_FACRORY_FRONT_END_H
#define _POWER_FACRORY_FRONT_END_H

#include <map>
#include <vector>

#include "export/include/FMIComponentFrontEndBase.h"
#include "import/base/include/ModelDescription.h"

class PowerFactory;
class PowerFactoryRealScalar;


/**
 * \file PowerFactoryFrontEnd.h
 * \class PowerFactoryFrontEnd PowerFactoryFrontEnd.h
 * This class provides a wrapper around the simulation API for PowerFactory.
 *
 * Its interface is designed such that it can be easily used as an FMI component (FMI model type
 * fmiComponent), implementing functionalities close to the requirements of the FMI specification,
 * e.g., functions initializeSlave(...), doStep(...) or setReal(...).
 */ 


class __FMI_DLL PowerFactoryFrontEnd : public FMIComponentFrontEndBase
{

public:

	PowerFactoryFrontEnd( const std::string& instanceName, const std::string& fmuGUID,
			      const std::string& fmuLocation, const std::string& mimeType,
			      fmiReal timeout, fmiBoolean visible );

	virtual ~PowerFactoryFrontEnd();

	///
	//  Functions for data exchange.
	///

	virtual fmiStatus setReal( const fmiValueReference& ref, const fmiReal& val );
	virtual fmiStatus setInteger( const fmiValueReference& ref, const fmiInteger& val );
	virtual fmiStatus setBoolean( const fmiValueReference& ref, const fmiBoolean& val );
	virtual fmiStatus setString( const fmiValueReference& ref, const fmiString& val );

	virtual fmiStatus getReal( const fmiValueReference& ref, fmiReal& val );
	virtual fmiStatus getInteger( const fmiValueReference& ref, fmiInteger& val );
	virtual fmiStatus getBoolean( const fmiValueReference& ref, fmiBoolean& val );
	virtual fmiStatus getString( const fmiValueReference& ref, fmiString& val );


	///
	//  Functions specific for FMI for Co-simulation.
	///

	virtual fmiStatus initializeSlave( fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop );
	//virtual fmiStatus terminateSlave(); // NOT NEEDED HERE? -> fmiFunctions.cpp
	virtual fmiStatus resetSlave();
	//virtual fmiStatus freeSlaveInstance(); // NOT NEEDED HERE? -> fmiFunctions.cpp

	virtual fmiStatus setRealInputDerivatives( const fmiValueReference vr[], size_t nvr,
						   const fmiInteger order[], const fmiReal value[]);
	virtual fmiStatus getRealOutputDerivatives( const fmiValueReference vr[], size_t nvr,
						    const fmiInteger order[], fmiReal value[]);

	virtual fmiStatus doStep( fmiReal comPoint, fmiReal stepSize, fmiBoolean newStep );
	virtual fmiStatus cancelStep();

	virtual fmiStatus getStatus( const fmiStatusKind s, fmiStatus* value );
	virtual fmiStatus getRealStatus( const fmiStatusKind s, fmiReal* value );
	virtual fmiStatus getIntegerStatus( const fmiStatusKind s, fmiInteger* value );
	virtual fmiStatus getBooleanStatus( const fmiStatusKind s, fmiBoolean* value );
	virtual fmiStatus getStringStatus( const fmiStatusKind s, fmiString* value );


private:

	typedef std::vector<PowerFactoryRealScalar*> RealCollection;
	typedef std::map<fmiValueReference, const PowerFactoryRealScalar*> RealMap;

	RealMap realScalarMap_;

	PowerFactory* pf_;

	void initializeVariables( const ModelDescription& modelDescription );

	void initializeScalar( PowerFactoryRealScalar* scalar,
			       const ModelDescription::Properties& description ) const;

	bool parseFMIVariableName( const std::string& name,
				   std::string& className,
				   std::string& objectName,
				   std::string& parameterName ) const;
};



#endif // _FMI_COMPONENT_FRONTEND_H
