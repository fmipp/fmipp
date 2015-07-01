/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _POWER_FACRORY_FRONT_END_H
#define _POWER_FACRORY_FRONT_END_H

#include <map>
#include <vector>
#include <utility>
#include <string>
#include <fstream>

#include "export/include/FMIComponentFrontEndBase.h"
#include "import/base/include/ModelDescription.h"

class PowerFactory;
class PowerFactoryRealScalar;
class PowerFactoryTimeAdvance;
namespace api { class DataObject; }


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
	virtual fmiStatus instantiateSlave(  const std::string& instanceName, const std::string& fmuGUID,
					     const std::string& fmuLocation, const std::string& mimeType,
					     fmiReal timeout, fmiBoolean visible );
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

	virtual fmiStatus initializeSlave( fmiReal tStart, fmiBoolean stopTimeDefined, fmiReal tStop );
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

	typedef std::map<fmiValueReference, const PowerFactoryRealScalar*> RealMap;

	typedef std::vector<const PowerFactoryRealScalar*> ExtraOutputSet;
	typedef std::pair<std::ofstream*, ExtraOutputSet*> ExtraOutput;
	typedef std::vector<ExtraOutput> ExtraOutputList;

	/// Map with all the internal representations of the model variables, indexed by value reference.
	RealMap realScalarMap_;

	/// Pointer to high-level PowerFactory API instance.
	PowerFactory* pf_;

	/// Handle for advancing time in simulation.
	PowerFactoryTimeAdvance* time_;

	/// PowerFactory target.
	std::string target_;

	/// PowerFactory target.
	std::string projectName_;

	/// FMU instance name.
	std::string instanceName_;

	/// Map with output streams and scalar variables for saving extra simulation results at each step.
	ExtraOutputList extraOutputs_;

	/// Instantiate time advance mechanism.
	bool instantiateTimeAdvanceMechanism( const ModelDescription& modelDescription );

	/// Initialize internal representation of model variables.
	bool initializeVariables( const ModelDescription& modelDescription );

	/// Extract and store information for a model variable from XML model description.
	bool initializeScalar( PowerFactoryRealScalar* scalar,
			       const ModelDescription::Properties& description );

	/// Initialize outputs streams and lists of scalar variables for extra output.
	bool initializeExtraOutput();

	/// Initialize outputs streams and lists of scalar variables for extra output.
	bool writeExtraOutput( const fmiReal currentSyncPoint );

	/// Extract and parse PowerFactory target.
	bool parseTarget( const ModelDescription& modelDescription );

	/** Extract and parse information abaout PowerFactory variables. Variable names
	 *  are supposed to be of the form "<class-name>.<object-name>.<parameter-name>".
	 */
	bool parseFMIVariableName( const std::string& name,
				   std::string& className,
				   std::string& objectName,
				   std::string& parameterName );
};



#endif // _POWER_FACRORY_FRONT_END_H
