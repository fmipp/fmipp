/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _POWER_FACTORY_FRONT_END_H
#define _POWER_FACTORY_FRONT_END_H

#include <map>
#include <vector>
#include <string>

#include "export/include/FMIComponentFrontEndBase.h"

class ModelDescription;
class PowerFactory;
class PowerFactoryRealScalar;
class PowerFactoryTimeAdvance;
class PowerFactoryExtraOutput;


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

	PowerFactoryFrontEnd();

	virtual ~PowerFactoryFrontEnd();

	///
	//  Functions for data exchange.
	///
	virtual fmi2Status setReal( const fmi2ValueReference& ref, const fmi2Real& val );
	virtual fmi2Status setInteger( const fmi2ValueReference& ref, const fmi2Integer& val );
	virtual fmi2Status setBoolean( const fmi2ValueReference& ref, const fmi2Boolean& val );
	virtual fmi2Status setString( const fmi2ValueReference& ref, const fmi2String& val );

	virtual fmi2Status getReal( const fmi2ValueReference& ref, fmi2Real& val );
	virtual fmi2Status getInteger( const fmi2ValueReference& ref, fmi2Integer& val );
	virtual fmi2Status getBoolean( const fmi2ValueReference& ref, fmi2Boolean& val );
	virtual fmi2Status getString( const fmi2ValueReference& ref, fmi2String& val );

	virtual fmi2Status getDirectionalDerivative( const fmi2ValueReference vUnknown_ref[],
					size_t nUnknown, const fmi2ValueReference vKnown_ref[], size_t nKnown,
					const fmi2Real dvKnown[], fmi2Real dvUnknown[] );

	
	///
	//  Optional functions for manipulating the FMU state (only dummy implementation for PowerFactory).
	///

	virtual fmi2Status getFMUState( fmi2FMUstate* fmuState );
	virtual fmi2Status setFMUState( fmi2FMUstate fmuState );
	virtual fmi2Status freeFMUState( fmi2FMUstate* fmuState );
	virtual fmi2Status serializedFMUStateSize( fmi2FMUstate fmuState, size_t* size );
	virtual fmi2Status serializeFMUState( fmi2FMUstate fmuState, fmi2Byte serializedState[], size_t size );
	virtual fmi2Status deserializeFMUState( const fmi2Byte serializedState[], size_t size, fmi2FMUstate* fmuState );

	///
	//  Functions specific for FMI for Co-simulation.
	///

	virtual fmi2Status instantiateSlave( const std::string& instanceName, const std::string& fmuGUID,
					const std::string& fmuLocation, fmi2Real timeout, fmi2Boolean visible );
	virtual fmi2Status initializeSlave( fmi2Real tStart, fmi2Boolean stopTimeDefined, fmi2Real tStop );
	virtual fmi2Status resetSlave();

	virtual fmi2Status setRealInputDerivatives( const fmi2ValueReference vr[], size_t nvr,
					const fmi2Integer order[], const fmi2Real value[]);
	virtual fmi2Status getRealOutputDerivatives( const fmi2ValueReference vr[], size_t nvr,
					const fmi2Integer order[], fmi2Real value[]);

	virtual fmi2Status doStep( fmi2Real comPoint, fmi2Real stepSize, fmi2Boolean newStep );
	virtual fmi2Status cancelStep();

	virtual fmi2Status getStatus( const fmi2StatusKind s, fmi2Status* value );
	virtual fmi2Status getRealStatus( const fmi2StatusKind s, fmi2Real* value );
	virtual fmi2Status getIntegerStatus( const fmi2StatusKind s, fmi2Integer* value );
	virtual fmi2Status getBooleanStatus( const fmi2StatusKind s, fmi2Boolean* value );
	virtual fmi2Status getStringStatus( const fmi2StatusKind s, fmi2String* value );

	/// Send a message to FMU logger.
	virtual void logger( fmi2Status status, const std::string& category, const std::string& msg );

private:

	typedef std::map<fmi2ValueReference, const PowerFactoryRealScalar*> RealMap;

	/// Map with all the internal representations of the model variables, indexed by value reference.
	RealMap realScalarMap_;

	/// Pointer to high-level PowerFactory API instance.
	PowerFactory* pf_;

	/// Handle for advancing time in simulation.
	PowerFactoryTimeAdvance* time_;

	/// Handle for dealing with extra outputs.
	PowerFactoryExtraOutput* extraOutput_;

	/// PowerFactory target.
	std::string target_;

	/// PowerFactory target.
	std::string projectName_;

	/// FMU instance name.
	std::string instanceName_;

	/// Instantiate time advance mechanism.
	bool instantiateTimeAdvanceMechanism( const ModelDescription* modelDescription );

	/// Initialize internal representation of model variables.
	bool initializeVariables( const ModelDescription* modelDescription );

	/// Extract and parse PowerFactory target.
	bool parseTarget( const ModelDescription* modelDescription );

};



#endif // _POWER_FACTORY_FRONT_END_H
