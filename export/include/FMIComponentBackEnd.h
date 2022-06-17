// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMICOMPONENTBACKEND_H
#define _FMIPP_FMICOMPONENTBACKEND_H

#include <vector>
#include <sstream>
#include <map>

#include "common/FMIPPConfig.h"

#include "export/include/ScalarVariable.h"
#include "export/include/IPCSlave.h"
#include "export/include/IPCSlaveLogger.h"
#include "export/include/IPCString.h"

/**
 * \file FMIComponentBackEnd.h
 * \class FMIComponentBackEnd FMIComponentBackEnd.h
 * The back end component functions as counterpart to the FMIComponentFrontEnd.
 *
 * It is intended to be incorporated within the slave application as part of a dedicated simulation
 * component, referred to as the FMI adapter. The back end interface is designed to make the connection
 * with the front end as simple as possible, focusing on synchronization and data exchange.
 */
class __FMI_DLL FMIComponentBackEnd
{

public:

	FMIComponentBackEnd();
	~FMIComponentBackEnd();

	///
	/// Start initialization of the backend (connect/sync with master).
	///
	fmippStatus startInitialization();

	///
	/// End initialization of the backend (connect/sync with master).
	///
	fmippStatus endInitialization();

	///
	/// Initialize real parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeRealParameters( const std::vector<fmippString>& names, std::vector<fmippReal*>& params );

	///
	/// Initialize real parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeRealParameters( const fmippString* names, fmippReal* params, const size_t n );

	///
	/// Initialize integer parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeIntegerParameters( const std::vector<fmippString>& names, std::vector<fmippInteger*>& params );

	///
	/// Initialize integer parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeIntegerParameters( const fmippString* names, fmippInteger* params, const size_t n );

	///
	/// Initialize boolean parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeBooleanParameters( const std::vector<fmippString>& names, std::vector<fmippBoolean*>& params );

	///
	/// Initialize boolean parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeBooleanParameters( const fmippString* names, fmippBoolean* params, const size_t n );

	///
	/// Initialize string parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeStringParameters( const std::vector<fmippString>& names, std::vector<fmippString*>& params );

	///
	/// Initialize string parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeStringParameters( const fmippString* names, fmippString* params, const size_t n );

	///
	/// Initialize real variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeRealInputs( const std::vector<fmippString>& names, std::vector<fmippReal*>& inputs );

	///
	/// Initialize real variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeRealInputs( const fmippString* names, fmippReal* inputs, const size_t n );

	///
	/// Initialize integer variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeIntegerInputs( const std::vector<fmippString>& names, std::vector<fmippInteger*>& inputs );

	///
	/// Initialize integer variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeIntegerInputs( const fmippString* names, fmippInteger* inputs, const size_t n );

	///
	/// Initialize boolean variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeBooleanInputs( const std::vector<fmippString>& names, std::vector<fmippBoolean*>& inputs );

	///
	/// Initialize boolean variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeBooleanInputs( const fmippString* names, fmippBoolean* inputs, const size_t n );

	///
	/// Initialize string variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeStringInputs( const std::vector<fmippString>& names, std::vector<fmippString*>& inputs );

	///
	/// Initialize string variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeStringInputs( const fmippString* names, fmippString* inputs, const size_t n );

	///
	/// Initialize real variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeRealOutputs( const std::vector<fmippString>& names, std::vector<fmippReal*>& outputs );

	///
	/// Initialize real variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeRealOutputs( const fmippString* names, fmippReal* outputs, const size_t n );

	///
	/// Initialize integer variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeIntegerOutputs( const std::vector<fmippString>& names, std::vector<fmippInteger*>& outputs );

	///
	/// Initialize integer variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeIntegerOutputs( const fmippString* names, fmippInteger* outputs, const size_t n );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeBooleanOutputs( const std::vector<fmippString>& names, std::vector<fmippBoolean*>& outputs );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeBooleanOutputs( const fmippString* names, fmippBoolean* outputs, const size_t n );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeStringOutputs( const std::vector<fmippString>& names, std::vector<fmippString*>& outputs );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmippStatus initializeStringOutputs( const fmippString* names, fmippString* outputs, const size_t n );

	///
	/// Wait for signal from master to resume execution.
	/// Blocks until signal from master is received.
	///
	void waitForMaster() const;

	///
	/// Send signal to master to proceed with execution.
	/// Do not read/write shared data until #waitForMaster unblocks.
	///
	void signalToMaster() const;

	///
	/// Read values from real parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeRealParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getRealParameters( std::vector<fmippReal*>& parameters );

	///
	/// Read values from real parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeRealParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getRealParameters( fmippReal* parameters, size_t nParameters );

	///
	/// Read values from integer parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getIntegerParameters( std::vector<fmippInteger*>& parameters );

	///
	/// Read values from integer parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getIntegerParameters( fmippInteger* parameters, size_t nParameters );

	///
	/// Read values from boolean parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getBooleanParameters( std::vector<fmippBoolean*>& parameters );

	///
	/// Read values from boolean parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getBooleanParameters( fmippBoolean* parameters, size_t nParameters );

	///
	/// Read values from string parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getStringParameters( std::vector<fmippString*>& parameters );

	///
	/// Read values from string parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getStringParameters( fmippString* parameters, size_t nParameters );

	///
	/// Specify values of real parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeRealParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmippStatus setRealParameters( const std::vector<fmippReal*>& parameters );

	///
	/// Specify values of real parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeRealParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmippStatus setRealParameters( const fmippReal* parameters, size_t nParameters );

	///
	/// Specify values of integer parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmippStatus setIntegerParameters( const std::vector<fmippInteger*>& parameters );

	///
	/// Specify values of integer parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmippStatus setIntegerParameters( const fmippInteger* parameters, size_t nParameters );

	///
	/// Specify values of boolean parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeBooleanParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmippStatus setBooleanParameters( const std::vector<fmippBoolean*>& parameters );

	///
	/// Specify values of boolean parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeBooelanParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmippStatus setBooleanParameters( const fmippBoolean* parameters, size_t nParameters );

	///
	/// Specify values of string parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeStringParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmippStatus setStringParameters( const std::vector<fmippString*>& parameters );

	///
	/// Specify values of string parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeStringParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmippStatus setStringParameters( const fmippString* parameters, size_t nParameters );

	///
	/// Read values from real inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getRealInputs( std::vector<fmippReal*>& inputs );

	///
	/// Read values from real inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getRealInputs( fmippReal* inputs, size_t nInputs );

	///
	/// Read values from integer inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getIntegerInputs( std::vector<fmippInteger*>& inputs );

	///
	/// Read values from integer inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getIntegerInputs( fmippInteger* inputs, size_t nInputs );

	///
	/// Read values from boolean inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getBooleanInputs( std::vector<fmippBoolean*>& inputs );

	///
	/// Read values from boolean inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getBooleanInputs( fmippBoolean* inputs, size_t nInputs );

	///
	/// Read values from string inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getStringInputs( std::vector<fmippString*>& inputs );

	///
	/// Read values from string inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus getStringInputs( fmippString* inputs, size_t nInputs );

	///
	/// Reset values of real inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus resetRealInputs( std::vector<fmippReal*>& inputs );

	///
	/// Reset values of real inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus resetRealInputs( fmippReal* inputs, size_t nInputs );

	///
	/// Reset values of integer inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus resetIntegerInputs( std::vector<fmippInteger*>& inputs );

	///
	/// Reset values of integer inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus resetIntegerInputs( fmippInteger* inputs, size_t nInputs );

	///
	/// Reset values of boolean inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus resetBooleanInputs( std::vector<fmippBoolean*>& inputs );

	///
	/// Reset values of boolean inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus resetBooleanInputs( fmippBoolean* inputs, size_t nInputs );

	///
	/// Reset values of string inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus resetStringInputs( std::vector<fmippString*>& inputs );

	///
	/// Reset values of string inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus resetStringInputs( fmippString* inputs, size_t nInputs );

	///
	/// Write values to real outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus setRealOutputs( const std::vector<fmippReal*>& outputs );

	///
	/// Write values to real outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus setRealOutputs( const fmippReal* outputs, size_t nOutputs );

	///
	/// Write values to integer outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus setIntegerOutputs( const std::vector<fmippInteger*>& outputs );

	///
	/// Write values to integer outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus setIntegerOutputs( const fmippInteger* outputs, size_t nOutputs );

	///
	/// Write values to boolean outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBooleanOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus setBooleanOutputs( const std::vector<fmippBoolean*>& outputs );

	///
	/// Write values to boolean outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBooleanOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus setBooleanOutputs( const fmippBoolean* outputs, size_t nOutputs );

	///
	/// Write values to string outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeStringOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus setStringOutputs( const std::vector<fmippString*>& outputs );

	///
	/// Write values to string outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeStringOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmippStatus setStringOutputs( const fmippString* outputs, size_t nOutputs );

	///
	/// Inform frontend what the next simulation time step will be.
	/// Call this method only before #endInitialization or between calls to #waitForMaster and #signalToMaster.
	///
	void enforceTimeStep( const fmippReal& delta );

	///
	/// Inform frontend that the simulation step has been rejected.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	void rejectStep();

	///
	/// Call the internal logger.
	///
	void logger( fmippStatus status, const fmippString& category, const fmippString& msg );

	///
	/// Get current communication point from the front end.
	/// Call this method only before #endInitialization or between calls to #waitForMaster and #signalToMaster.
	///
	const fmippReal& getCurrentCommunicationPoint() const;

	///
	/// Get next communication step size from the front end.
	/// Call this method only before #endInitialization or between calls to #waitForMaster and #signalToMaster.
	///
	const fmippReal& getCommunicationStepSize() const;

	///
	/// Get simulation stop time.
	///
	const fmippReal& getStopTime() const;

	///
	/// Get flag indicating if simulation stop time has been defined.
	///
	const bool& getStopTimeDefined() const;

	///
	/// Get full path of log messages file.
	///
	fmippString getLogFileName() const;

	///
	/// Check if logging has been activated.
	///
	bool loggingOn() const;

	///
	/// Get names of all real inputs initialized by the front end.
	///
	void getRealInputNames( std::vector<fmippString>& names ) const;

	///
	/// Get names of all integer inputs initialized by the front end.
	///
	void getIntegerInputNames( std::vector<fmippString>& names ) const;

	///
	/// Get names of all boolean inputs initialized by the front end.
	///
	void getBooleanInputNames( std::vector<fmippString>& names ) const;

	///
	/// Get names of all string inputs initialized by the front end.
	///
	void getStringInputNames( std::vector<fmippString>& names ) const;


	///
	/// Get names of all real outputs initialized by the front end.
	///
	void getRealOutputNames( std::vector<fmippString>& names ) const;

	///
	/// Get names of all integer outputs initialized by the front end.
	///
	void getIntegerOutputNames( std::vector<fmippString>& names ) const;

	///
	/// Get names of all boolean outputs initialized by the front end.
	///
	void getBooleanOutputNames( std::vector<fmippString>& names ) const;

	///
	/// Get names of all string outputs initialized by the front end.
	///
	void getStringOutputNames( std::vector<fmippString>& names ) const;

private:

	///
	/// Internal helper function to get the process ID (or the
	/// parent process in case flag BACKEND_USE_PARENT_PID is
	/// set at compilation time).
	///
	const fmippString getProcessID() const;

	///
	/// Internal helper function for initialization of inputs/outputs.
	///
	template<typename Type>
	fmippStatus initializeVariables( std::vector<Type*>& variablePointers,
				       const fmippString& scalarCollection,
				       const std::vector<fmippString>& scalarNames,
				       const ScalarVariableAttributes::Causality::Causality causality );

	///
	/// Internal helper function for initialization of inputs/outputs.
	///
	template<typename Type>
	fmippStatus initializeVariables( std::vector<Type*>& variablePointers,
				       const fmippString& scalarCollection,
				       const fmippString* scalarNames,
					   const size_t nScalarNames,
				       const ScalarVariableAttributes::Causality::Causality causality );

	///
	/// Internal helper function for retrieving variable names.
	///
	template<typename Type>
	void getScalarNames( std::vector<fmippString>& scalarNames,
			     const fmippString& scalarCollection,
			     const ScalarVariableAttributes::Causality::Causality causality ) const;

	///
	/// Internal helper function for retrieving the FMI version.
	///
	FMUType getFMUType() const { return static_cast< FMUType >( *fmuType_ ); }

	///
	/// Interface for inter-process communication.
	///
	IPCSlave* ipcSlave_;

	///
	/// Logger.
	///
	IPCSlaveLogger* ipcLogger_;

	///
	/// Simulation time as requested by the master.
	///
	fmippReal* currentCommunicationPoint_;

	///
	/// Next simulation time step size (requested by the master or enforced by the slave).
	///
	fmippReal* communicationStepSize_;

	///
	/// Simulation stop time.
	///
	fmippReal* stopTime_;

	///
	/// Flag indicating if stop time has been defined.
	///
	bool* stopTimeDefined_;

	///
	/// Flag for enforcing simulation time step size.
	///
	bool* enforceTimeStep_;

	///
	/// Flag for rejecting a simulation step.
	///
	bool* rejectStep_;

	///
	/// Flag to indicate to the frontend that the slave has terminated.
	///
	bool* slaveHasTerminated_;

	///
	/// Flag to indicate to the FMI version.
	///
	int* fmuType_;

	///
	/// Flag for logging on/off.
	///
	bool* loggingOn_;

	///
	/// Internal pointers to real-valued parameters.
	///
	std::vector<fmippReal*> realParameters_;

	///
	/// Internal pointers to integer-valued parameters.
	///
	std::vector<fmippInteger*> integerParameters_;

	///
	/// Internal pointers to boolean-valued parameters.
	///
	std::vector<fmippBoolean*> booleanParameters_;

	///
	/// Internal pointers to string-valued parameters.
	///
	std::vector<fmippIPCString*> stringParameters_;

	///
	/// Internal pointers to real-valued inputs.
	///
	std::vector<fmippReal*> realInputs_;

	///
	/// Internal pointers to integer-valued inputs.
	///
	std::vector<fmippInteger*> integerInputs_;

	///
	/// Internal pointers to boolean-valued inputs.
	///
	std::vector<fmippBoolean*> booleanInputs_;

	///
	/// Internal pointers to string-valued inputs.
	///
	std::vector<fmippIPCString*> stringInputs_;

	///
	/// Internal pointers to real-valued outputs.
	///
	std::vector<fmippReal*> realOutputs_;

	///
	/// Internal pointers to integer-valued outputs.
	///
	std::vector<fmippInteger*> integerOutputs_;

	///
	/// Internal pointers to boolean-valued outputs.
	///
	std::vector<fmippBoolean*> booleanOutputs_;

	///
	/// Internal pointers to string-valued outputs.
	///
	std::vector<fmippIPCString*> stringOutputs_;
};

template<typename Type>
fmippStatus FMIComponentBackEnd::initializeVariables( std::vector<Type*>& variablePointers,
	const fmippString& scalarCollection,
	const fmippString* scalarNames,
	const size_t nScalarNames,
	const ScalarVariableAttributes::Causality::Causality causality )
{
	std::vector<fmippString> vecScalarNames( scalarNames, scalarNames + nScalarNames );
	return initializeVariables( variablePointers, scalarCollection, vecScalarNames, causality );
}

template<typename Type>
fmippStatus FMIComponentBackEnd::initializeVariables( std::vector<Type*>& variablePointers,
	const fmippString& scalarCollection,
	const std::vector<fmippString>& scalarNames,
	const ScalarVariableAttributes::Causality::Causality causality )
{
	fmippStatus result = fmippOK;

	// Clear the vector real inputs.
	if ( false == variablePointers.empty() ) {
		variablePointers.clear();
		ipcLogger_->logger( fmippWarning, "WARNING", "previous elements of input vector have been erased" );
	}

	if ( true == scalarNames.empty() ) return result;

	// Reserve correct number of elements.
	variablePointers.reserve( scalarNames.size() );

	// Retrieve scalars from master.
	std::vector< ScalarVariable<Type>* > scalars;
	ipcSlave_->retrieveScalars( scalarCollection, scalars );

	// Fill map between scalar names and instance pointers
	std::map< fmippString, ScalarVariable<Type>* > scalarMap;
	typename std::vector< ScalarVariable<Type>* >::iterator itScalar = scalars.begin();
	typename std::vector< ScalarVariable<Type>* >::iterator endScalars = scalars.end();
	for ( ; itScalar != endScalars; ++itScalar ) {
		scalarMap[(*itScalar)->name_] = *itScalar;
	}

	// Iterators needed for searching the map.
	typename std::map< fmippString, ScalarVariable<Type>* >::const_iterator itFind;
	typename std::map< fmippString, ScalarVariable<Type>* >::const_iterator itFindEnd = scalarMap.end();

	// Loop through the input names, chack their causality and store pointer.
	typename std::vector<fmippString>::const_iterator itName = scalarNames.begin();
	typename std::vector<fmippString>::const_iterator itNamesEnd = scalarNames.end();
	//Type** currentVariablePointer = variablePointers;
	for ( ; itName != itNamesEnd; ++ itName )
	{
		// Search for name in map.
		itFind = scalarMap.find( *itName );

		// Check if scalar according to the name exists.
		if ( itFind == itFindEnd )
		{
			std::stringstream err;
			err << "scalar variable not found: " << *itName;
			ipcLogger_->logger( fmippFatal, "ABORT", err.str() );
			result = fmippFatal;
			break;
		} else {
			if ( causality != itFind->second->causality_ ) {
				std::stringstream err;
				err << "scalar variable '" << *itName << "' has wrong causality: "
				    << itFind->second->causality_ << " instead of " << causality;
				ipcLogger_->logger( fmippFatal, "ABORT", err.str() );
				result = fmippWarning;
			}

			/// \FIXME What about variability of scalar variable?

			// Get value.
			variablePointers.push_back( &itFind->second->value_ );
			//*currentVariablePointer = &itFind->second->value_;
			//++currentVariablePointer;
		}
	}

	return result;
}

template<typename Type>
void FMIComponentBackEnd::getScalarNames( std::vector<fmippString>& scalarNames,
					  const fmippString& scalarCollection,
					  const ScalarVariableAttributes::Causality::Causality causality ) const
{
	scalarNames.clear();

	// Retrieve scalars from master.
	std::vector< ScalarVariable<Type>* > scalars;
	ipcSlave_->retrieveScalars( scalarCollection, scalars );

	// Fill vector with scalar names.
	typename std::vector< ScalarVariable<Type>* >::iterator itScalar = scalars.begin();
	typename std::vector< ScalarVariable<Type>* >::iterator endScalars = scalars.end();
	for ( ; itScalar != endScalars; ++itScalar ) {
		if ( causality == (*itScalar)->causality_ )
			scalarNames.push_back( (*itScalar)->name_ );
	}
}

#endif // _FMIPP_FMICOMPONENTBACKEND_H
