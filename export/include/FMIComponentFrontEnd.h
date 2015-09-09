/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMICOMPONENTFRONTEND_H
#define _FMIPP_FMICOMPONENTFRONTEND_H

#include <map>
#include <vector>

#include "export/include/FMIComponentFrontEndBase.h"

class IPCMaster;
class IPCLogger;
template<class T> class ScalarVariable;
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

	virtual fmiStatus setReal( const fmiValueReference& ref, const fmiReal& val );
	virtual fmiStatus setInteger( const fmiValueReference& ref, const fmiInteger& val );
	virtual fmiStatus setBoolean( const fmiValueReference& ref, const fmiBoolean& val );
	virtual fmiStatus setString( const fmiValueReference& ref, const fmiString& val );

	virtual fmiStatus getReal( const fmiValueReference& ref, fmiReal& val );
	virtual fmiStatus getInteger( const fmiValueReference& ref, fmiInteger& val );
	virtual fmiStatus getBoolean( const fmiValueReference& ref, fmiBoolean& val );
	virtual fmiStatus getString( const fmiValueReference& ref, fmiString& val );


	//
	//  Functions specific for FMI for Co-simulation.
	//

	virtual fmiStatus instantiateSlave( const std::string& instanceName, const std::string& fmuGUID,
					    const std::string& fmuLocation, const std::string& mimeType,
					    fmiReal timeout, fmiBoolean visible );
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

	/// Send a message to FMU logger.
	virtual void logger( fmiStatus status, const std::string& category, const std::string& msg );
	
private:

	typedef ScalarVariable<fmiReal> RealScalar;
	typedef ScalarVariable<fmiInteger> IntegerScalar;
	typedef ScalarVariable<fmiBoolean> BooleanScalar;
	typedef ScalarVariable<std::string> StringScalar; // Attention: We do not use fmiString here!!!

	typedef std::vector<RealScalar*> RealCollection;
	typedef std::vector<IntegerScalar*> IntegerCollection;
	typedef std::vector<BooleanScalar*> BooleanCollection;
	typedef std::vector<StringScalar*> StringCollection;

	typedef std::map<fmiValueReference, RealScalar*> RealMap;
	typedef std::map<fmiValueReference, IntegerScalar*> IntegerMap;
	typedef std::map<fmiValueReference, BooleanScalar*> BooleanMap;
	typedef std::map<fmiValueReference, StringScalar*> StringMap;

	RealMap realScalarMap_;
	IntegerMap integerScalarMap_;
	BooleanMap booleanScalarMap_;
	StringMap stringScalarMap_;

	IPCMaster* ipcMaster_;
	IPCLogger* ipcLogger_;

	fmiReal* currentCommunicationPoint_;
	fmiReal* communicationStepSize_;

	bool* enforceTimeStep_;
	bool* rejectStep_;

	bool* slaveHasTerminated_;

	std::string instanceName_;

	/// Process ID of backend application.
#ifdef WIN32
	int pid_;
#else
	pid_t pid_;
#endif

	/// Start external simulator application in a separate thread.
	bool startApplication( const ModelDescription* modelDescription,
			       const std::string& mimeType,
			       const std::string& fmuLocation );


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
