/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMI_COMPONENT_FRONTEND_H
#define _FMI_COMPONENT_FRONTEND_H

#include <map>
#include <vector>
#include <string>

#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/FMIPPConfig.h"

#include "import/base/include/ModelDescription.h"

class IPCMaster;
template<class T> class ScalarVariable;



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
class __FMI_DLL FMIComponentFrontEnd
{

public:

	FMIComponentFrontEnd( const std::string& instanceName, const std::string& fmuGUID,
			      const std::string& fmuLocation, const std::string& mimeType,
			      fmiReal timeout, fmiBoolean visible );

	~FMIComponentFrontEnd();

	///
	//  Functions for data exchange.
	///

	fmiStatus setReal( const fmiValueReference& ref, const fmiReal& val );
	fmiStatus setInteger( const fmiValueReference& ref, const fmiInteger& val );
	fmiStatus setBoolean( const fmiValueReference& ref, const fmiBoolean& val );
	fmiStatus setString( const fmiValueReference& ref, const fmiString& val );

	fmiStatus getReal( const fmiValueReference& ref, fmiReal& val ) const;
	fmiStatus getInteger( const fmiValueReference& ref, fmiInteger& val );
	fmiStatus getBoolean( const fmiValueReference& ref, fmiBoolean& val );
	fmiStatus getString( const fmiValueReference& ref, fmiString& val );


	///
	//  Functions specific for FMI for Co-simulation.
	///

	fmiStatus initializeSlave( fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop );
	//fmiStatus terminateSlave(...); /// NOT NEEDED HERE? -> fmiFunctions.cpp
	//fmiStatus resetSlave(...);
	//fmiStatus freeSlaveInstance(...);

	//fmiStatus setRealInputDerivatives(...);
	//fmiStatus getRealOutputDerivatives(...);

	fmiStatus doStep( fmiReal comPoint, fmiReal stepSize, fmiBoolean newStep );
	//fmiStatus cancelStep(...);

	//fmiStatus getStatus(...);
	//fmiStatus getRealStatus(...);
	//fmiStatus getIntegerStatus(...);
	//fmiStatus getBooleanStatus(...);
	//fmiStatus getStringStatus(...);


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

	fmiReal* masterTime_;
	fmiReal* nextStepSize_;

	bool* enforceTimeStep_;
	bool* rejectStep_;

	bool* slaveHasTerminated_;

	/// Process ID of backend application.
#ifdef WIN32
	int pid_;
#else
	pid_t pid_;
#endif

	void startApplication( const std::string& applicationName,
			       const std::string& inputFileUrl );

	void killApplication() const;

	void initializeVariables( const ModelDescription& modelDescription,
				  RealCollection& realScalars,
				  IntegerCollection& integerScalars,
				  BooleanCollection& booleanScalars,
				  StringCollection& stringScalars );

	template<typename T>
	void initializeScalar( ScalarVariable<T>* scalar,
			       const ModelDescription::Properties& description,
			       const std::string& xmlTypeTag ) const;
};



#endif // _FMI_COMPONENT_FRONTEND_H
