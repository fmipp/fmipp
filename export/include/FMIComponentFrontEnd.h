// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMICOMPONENTFRONTEND_H
#define _FMIPP_FMICOMPONENTFRONTEND_H

#include <map>
#include <vector>

#include "common/FMUType.h"
#include "export/include/FMIComponentFrontEndBase.h"
#include "export/include/ScalarVariable.h"

class IPCMaster;
class IPCLogger;
class ModelDescription;


/**
 * \file FMIComponentFrontEnd.h
 * \class FMIComponentFrontEnd FMIComponentFrontEnd.h
 * The front end component is the actual gateway for a master algorithm to communicate and exchange
 * data with an external simulation application.
 *
 * Its interface is designed such that it can be easily used as an FMI component (FMI model type
 * fmiComponent), implementing functionalities close to the requirements of the FMI specification,
 * e.g., functions initializeSlave(...), doStep(...) or setReal(...).
 */


class __FMI_DLL FMIComponentFrontEnd : public FMIComponentFrontEndBase
{

public:

	/// Constructor.
	FMIComponentFrontEnd();

	/// Destructor.
	virtual ~FMIComponentFrontEnd();

	//
	//  Functions for data exchange.
	//

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

	//
	//  Functions specific for FMI for Co-simulation.
	//

	/// For FMI Version 1.x.
	virtual fmi2Status instantiateSlave( const std::string& instanceName, const std::string& fmuGUID,
		const std::string& fmuLocation, fmi2Real timeout, fmi2Boolean visible );

	/// For FMI Version 2.x.
	virtual fmi2Status instantiate( const std::string& instanceName, const std::string& fmuGUID,
		const std::string& fmuResourceLocation, fmi2Boolean visible );

	virtual fmi2Status initializeSlave( fmi2Real tStart, fmi2Boolean stopTimeDefined, fmi2Real tStop );
	virtual fmi2Status resetSlave();

	virtual fmi2Status setRealInputDerivatives( const fmi2ValueReference vr[], size_t nvr,
						   const fmi2Integer order[], const fmi2Real value[]);
	virtual fmi2Status getRealOutputDerivatives( const fmi2ValueReference vr[], size_t nvr,
						    const fmi2Integer order[], fmi2Real value[]);

	virtual fmi2Status doStep( fmi2Real comPoint, fmi2Real stepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint );
	virtual fmi2Status cancelStep();

	virtual fmi2Status getStatus( const fmi2StatusKind s, fmi2Status* value );
	virtual fmi2Status getRealStatus( const fmi2StatusKind s, fmi2Real* value );
	virtual fmi2Status getIntegerStatus( const fmi2StatusKind s, fmi2Integer* value );
	virtual fmi2Status getBooleanStatus( const fmi2StatusKind s, fmi2Boolean* value );
	virtual fmi2Status getStringStatus( const fmi2StatusKind s, fmi2String* value );

	//
	//  Optional functions for manipulating the FMU state.
	//

	virtual fmi2Status getFMUState( fmi2FMUstate* fmuState );
	virtual fmi2Status setFMUState( fmi2FMUstate fmuState );
	virtual fmi2Status freeFMUState( fmi2FMUstate* fmuState );
	virtual fmi2Status serializedFMUStateSize( fmi2FMUstate fmuState, size_t* size );
	virtual fmi2Status serializeFMUState( fmi2FMUstate fmuState, fmi2Byte serializedState[], size_t size );
	virtual fmi2Status deserializeFMUState( const fmi2Byte serializedState[], size_t size, fmi2FMUstate* fmuState );

	/// Send a message to FMU logger.
	virtual void logger( fmi2Status status, const std::string& category, const std::string& msg );

	/// Get MIME type (FMI 1.0 compatibility).
	virtual const std::string getMIMEType() const;

private:

	typedef ScalarVariable<fmi2Real> RealScalar;
	typedef ScalarVariable<fmi2Integer> IntegerScalar;
	typedef ScalarVariable<fmi2Boolean> BooleanScalar;
	typedef ScalarVariable<std::string> StringScalar; // Attention: We do not use fmi2String here!!!

	typedef std::vector<RealScalar*> RealCollection;
	typedef std::vector<IntegerScalar*> IntegerCollection;
	typedef std::vector<BooleanScalar*> BooleanCollection;
	typedef std::vector<StringScalar*> StringCollection;

	typedef std::map<fmi2ValueReference, RealScalar*> RealMap;
	typedef std::map<fmi2ValueReference, IntegerScalar*> IntegerMap;
	typedef std::map<fmi2ValueReference, BooleanScalar*> BooleanMap;
	typedef std::map<fmi2ValueReference, StringScalar*> StringMap;
	
	typedef ScalarVariableAttributes::Causality::Causality Causality;
	typedef ScalarVariableAttributes::Variability::Variability Variability;

	RealMap realScalarMap_;
	IntegerMap integerScalarMap_;
	BooleanMap booleanScalarMap_;
	StringMap stringScalarMap_;

	IPCMaster* ipcMaster_;
	IPCLogger* ipcLogger_;

	fmi2Real* currentCommunicationPoint_;
	fmi2Real* communicationStepSize_;
	fmi2Real* stopTime_;

	bool* stopTimeDefined_;
	bool* enforceTimeStep_;
	bool* rejectStep_;

	bool* slaveHasTerminated_;

	int* fmuType_;

	std::string instanceName_;
	std::string mimeType_;

	/// Process ID of backend application.
#ifdef WIN32
	int pid_;
	HANDLE job_;
#else
	pid_t pid_;
#endif

	/// This is the precision for matching the internal time with the communication point in function doStep(...).
	const fmi2Real comPointPrecision_; // Will be set to 1e-9 in constructor.

	/// Start external simulator application in a separate thread.
	bool startApplication( const ModelDescription* modelDescription, const std::string& fmuLocation );


	/// Kill external simulator application.
	void killApplication();

	/// Initialize internal variables in shared memory
	void initializeVariables( const ModelDescription* modelDescription,
		RealCollection& realScalars,
		IntegerCollection& integerScalars,
		BooleanCollection& booleanScalars,
		StringCollection& stringScalars );

};



#endif // _FMIPP_FMICOMPONENTFRONTEND_H
