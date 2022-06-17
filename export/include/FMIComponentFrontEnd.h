// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMICOMPONENTFRONTEND_H
#define _FMIPP_FMICOMPONENTFRONTEND_H

#include <map>
#include <vector>

#include "export/include/FMIComponentFrontEndBase.h"
#include "export/include/ScalarVariable.h"
#include "export/include/IPCString.h"

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

	virtual fmippStatus setReal( const fmippValueReference& ref, const fmippReal& val );
	virtual fmippStatus setInteger( const fmippValueReference& ref, const fmippInteger& val );
	virtual fmippStatus setBoolean( const fmippValueReference& ref, const fmippBoolean& val );
	virtual fmippStatus setString( const fmippValueReference& ref, const fmippChar* val );

	virtual fmippStatus getReal( const fmippValueReference& ref, fmippReal& val );
	virtual fmippStatus getInteger( const fmippValueReference& ref, fmippInteger& val );
	virtual fmippStatus getBoolean( const fmippValueReference& ref, fmippBoolean& val );
	virtual fmippStatus getString( const fmippValueReference& ref, const fmippChar*& val );

	virtual fmippStatus getDirectionalDerivative( const fmippValueReference vUnknown_ref[],
		size_t nUnknown, const fmippValueReference vKnown_ref[], size_t nKnown,
		const fmippReal dvKnown[], fmippReal dvUnknown[] );

	//
	//  Functions specific for FMI for Co-simulation.
	//

	/// For FMI Version 1.x.
	virtual fmippStatus instantiateSlave( const std::string& instanceName, const std::string& fmuGUID,
		const std::string& fmuLocation, fmippReal timeout, fmippBoolean visible );

	/// For FMI Version 2.x.
	virtual fmippStatus instantiate( const std::string& instanceName, const std::string& fmuGUID,
		const std::string& fmuResourceLocation, fmippBoolean visible );

	virtual fmippStatus initializeSlave( fmippReal tStart, fmippBoolean stopTimeDefined, fmippReal tStop );
	virtual fmippStatus resetSlave();

	virtual fmippStatus setRealInputDerivatives( const fmippValueReference vr[], size_t nvr,
		const fmippInteger order[], const fmippReal value[]);
	virtual fmippStatus getRealOutputDerivatives( const fmippValueReference vr[], size_t nvr,
		const fmippInteger order[], fmippReal value[]);

	virtual fmippStatus doStep( fmippReal comPoint, fmippReal stepSize, fmippBoolean noSetFMUStatePriorToCurrentPoint );
	virtual fmippStatus cancelStep();

	virtual fmippStatus getStatus( const fmippStatusKind s, fmippStatus* value );
	virtual fmippStatus getRealStatus( const fmippStatusKind s, fmippReal* value );
	virtual fmippStatus getIntegerStatus( const fmippStatusKind s, fmippInteger* value );
	virtual fmippStatus getBooleanStatus( const fmippStatusKind s, fmippBoolean* value );
	virtual fmippStatus getStringStatus( const fmippStatusKind s, const fmippChar* value );

	//
	//  Optional functions for manipulating the FMU state.
	//

	virtual fmippStatus getFMUState( fmippFMUState* fmuState );
	virtual fmippStatus setFMUState( fmippFMUState fmuState );
	virtual fmippStatus freeFMUState( fmippFMUState* fmuState );
	virtual fmippStatus serializedFMUStateSize( fmippFMUState fmuState, size_t* size );
	virtual fmippStatus serializeFMUState( fmippFMUState fmuState, fmippByte serializedState[], size_t size );
	virtual fmippStatus deserializeFMUState( const fmippByte serializedState[], size_t size, fmippFMUState* fmuState );

	/// Send a message to FMU logger.
	virtual void logger( fmippStatus status, const std::string& category, const std::string& msg );

	/// Get MIME type (FMI 1.0 compatibility).
	virtual const std::string getMIMEType() const;

private:

	typedef ScalarVariable<fmippReal> RealScalar;
	typedef ScalarVariable<fmippInteger> IntegerScalar;
	typedef ScalarVariable<fmippBoolean> BooleanScalar;
	typedef ScalarVariable<fmippIPCString> StringScalar;

	typedef std::vector<RealScalar*> RealCollection;
	typedef std::vector<IntegerScalar*> IntegerCollection;
	typedef std::vector<BooleanScalar*> BooleanCollection;
	typedef std::vector<StringScalar*> StringCollection;

	typedef std::map<fmippValueReference, RealScalar*> RealMap;
	typedef std::map<fmippValueReference, IntegerScalar*> IntegerMap;
	typedef std::map<fmippValueReference, BooleanScalar*> BooleanMap;
	typedef std::map<fmippValueReference, StringScalar*> StringMap;

	typedef ScalarVariableAttributes::Causality::Causality Causality;
	typedef ScalarVariableAttributes::Variability::Variability Variability;

	RealMap realScalarMap_;
	IntegerMap integerScalarMap_;
	BooleanMap booleanScalarMap_;
	StringMap stringScalarMap_;

	IPCMaster* ipcMaster_;
	IPCLogger* ipcLogger_;

	fmippReal* currentCommunicationPoint_;
	fmippReal* communicationStepSize_;
	fmippReal* stopTime_;

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
	const fmippReal comPointPrecision_; // Will be set to 1e-9 in constructor.

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
