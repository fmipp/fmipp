// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMICOMPONENTBACKEND_H
#define _FMIPP_FMICOMPONENTBACKEND_H

#include <string>
#include <vector>
#include <sstream>
#include <map>

#include "common/fmi_v2.0/fmi2ModelTypes.h"
#include "common/FMIPPConfig.h"
#include "common/FMUType.h"

#include "export/include/ScalarVariable.h"
#include "export/include/IPCSlave.h"
#include "export/include/IPCSlaveLogger.h"


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
	fmi2Status startInitialization();

	///
	/// End initialization of the backend (connect/sync with master).
	///
	fmi2Status endInitialization();

	///
	/// Initialize real parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeRealParameters( const std::vector<std::string>& names, std::vector<fmi2Real*>& params );

	///
	/// Initialize real parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeRealParameters( const std::string* names, fmi2Real* params, const size_t n );

	///
	/// Initialize integer parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeIntegerParameters( const std::vector<std::string>& names, std::vector<fmi2Integer*>& params );

	///
	/// Initialize integer parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeIntegerParameters( const std::string* names, fmi2Integer* params, const size_t n );

	///
	/// Initialize boolean parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeBooleanParameters( const std::vector<std::string>& names, std::vector<fmi2Boolean*>& params );

	///
	/// Initialize boolean parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeBooleanParameters( const std::string* names, fmi2Boolean* params, const size_t n );

	///
	/// Initialize string parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeStringParameters( const std::vector<std::string>& names, std::vector<std::string*>& params );

	///
	/// Initialize string parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeStringParameters( const std::string* names, std::string* params, const size_t n );

	///
	/// Initialize real variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeRealInputs( const std::vector<std::string>& names, std::vector<fmi2Real*>& inputs );

	///
	/// Initialize real variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeRealInputs( const std::string* names, fmi2Real* inputs, const size_t n );

	///
	/// Initialize integer variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeIntegerInputs( const std::vector<std::string>& names, std::vector<fmi2Integer*>& inputs );

	///
	/// Initialize integer variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeIntegerInputs( const std::string* names, fmi2Integer* inputs, const size_t n );

	///
	/// Initialize boolean variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeBooleanInputs( const std::vector<std::string>& names, std::vector<fmi2Boolean*>& inputs );

	///
	/// Initialize boolean variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeBooleanInputs( const std::string* names, fmi2Boolean* inputs, const size_t n );

	///
	/// Initialize string variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeStringInputs( const std::vector<std::string>& names, std::vector<std::string*>& inputs );

	///
	/// Initialize string variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeStringInputs( const std::string* names, std::string* inputs, const size_t n );

	///
	/// Initialize real variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeRealOutputs( const std::vector<std::string>& names, std::vector<fmi2Real*>& outputs );

	///
	/// Initialize real variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeRealOutputs( const std::string* names, fmi2Real* outputs, const size_t n );

	///
	/// Initialize integer variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeIntegerOutputs( const std::vector<std::string>& names, std::vector<fmi2Integer*>& outputs );

	///
	/// Initialize integer variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeIntegerOutputs( const std::string* names, fmi2Integer* outputs, const size_t n );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeBooleanOutputs( const std::vector<std::string>& names, std::vector<fmi2Boolean*>& outputs );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeBooleanOutputs( const std::string* names, fmi2Boolean* outputs, const size_t n );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeStringOutputs( const std::vector<std::string>& names, std::vector<std::string*>& outputs );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmi2Status initializeStringOutputs( const std::string* names, std::string* outputs, const size_t n );

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
	fmi2Status getRealParameters( std::vector<fmi2Real*>& parameters );

	///
	/// Read values from real parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeRealParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getRealParameters( fmi2Real* parameters, size_t nParameters );

	///
	/// Read values from integer parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getIntegerParameters( std::vector<fmi2Integer*>& parameters );

	///
	/// Read values from integer parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getIntegerParameters( fmi2Integer* parameters, size_t nParameters );

	///
	/// Read values from boolean parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getBooleanParameters( std::vector<fmi2Boolean*>& parameters );

	///
	/// Read values from boolean parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getBooleanParameters( fmi2Boolean* parameters, size_t nParameters );

	///
	/// Read values from string parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status getStringParameters( std::vector<std::string*>& parameters );

	///
	/// Read values from string parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status getStringParameters( std::string* parameters, size_t nParameters );


	///
	/// Specify values of real parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeRealParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmi2Status setRealParameters( const std::vector<fmi2Real*>& parameters );

	///
	/// Specify values of real parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeRealParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmi2Status setRealParameters( const fmi2Real* parameters, size_t nParameters );

	///
	/// Specify values of integer parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmi2Status setIntegerParameters( const std::vector<fmi2Integer*>& parameters );

	///
	/// Specify values of integer parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmi2Status setIntegerParameters( const fmi2Integer* parameters, size_t nParameters );

	///
	/// Specify values of boolean parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeBooleanParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmi2Status setBooleanParameters( const std::vector<fmi2Boolean*>& parameters );

	///
	/// Specify values of boolean parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeBooelanParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	///
	fmi2Status setBooleanParameters( const fmi2Boolean* parameters, size_t nParameters );

	///
	/// Specify values of string parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeStringParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status setStringParameters( const std::vector<std::string*>& parameters );

	///
	/// Specify values of string parameters.
	/// Inputs are assumed to be in the same order as specified by #initializeStringParameters.
	/// This method is supposed to be called only during the initializatiob phase.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status setStringParameters( const std::string* parameters, size_t nParameters );


	///
	/// Read values from real inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getRealInputs( std::vector<fmi2Real*>& inputs );

	///
	/// Read values from real inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getRealInputs( fmi2Real* inputs, size_t nInputs );

	///
	/// Read values from integer inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getIntegerInputs( std::vector<fmi2Integer*>& inputs );

	///
	/// Read values from integer inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getIntegerInputs( fmi2Integer* inputs, size_t nInputs );

	///
	/// Read values from boolean inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getBooleanInputs( std::vector<fmi2Boolean*>& inputs );

	///
	/// Read values from boolean inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status getBooleanInputs( fmi2Boolean* inputs, size_t nInputs );

	///
	/// Read values from string inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status getStringInputs( std::vector<std::string*>& inputs );

	///
	/// Read values from string inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status getStringInputs( std::string* inputs, size_t nInputs );

	///
	/// Reset values of real inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status resetRealInputs( std::vector<fmi2Real*>& inputs );

	///
	/// Reset values of real inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status resetRealInputs( fmi2Real* inputs, size_t nInputs );

	///
	/// Reset values of integer inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status resetIntegerInputs( std::vector<fmi2Integer*>& inputs );

	///
	/// Reset values of integer inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status resetIntegerInputs( fmi2Integer* inputs, size_t nInputs );

	///
	/// Reset values of boolean inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status resetBooleanInputs( std::vector<fmi2Boolean*>& inputs );

	///
	/// Reset values of boolean inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status resetBooleanInputs( fmi2Boolean* inputs, size_t nInputs );

	///
	/// Reset values of string inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status resetStringInputs( std::vector<std::string*>& inputs );

	///
	/// Reset values of string inputs (i.e., overwrite values of input variables in the
	/// frontend with values provided by the backend).
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status resetStringInputs( std::string* inputs, size_t nInputs );

	///
	/// Write values to real outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status setRealOutputs( const std::vector<fmi2Real*>& outputs );

	///
	/// Write values to real outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status setRealOutputs( const fmi2Real* outputs, size_t nOutputs );

	///
	/// Write values to integer outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status setIntegerOutputs( const std::vector<fmi2Integer*>& outputs );

	///
	/// Write values to integer outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status setIntegerOutputs( const fmi2Integer* outputs, size_t nOutputs );

	///
	/// Write values to boolean outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBooleanOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status setBooleanOutputs( const std::vector<fmi2Boolean*>& outputs );

	///
	/// Write values to boolean outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBooleanOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmi2Status setBooleanOutputs( const fmi2Boolean* outputs, size_t nOutputs );

	///
	/// Write values to string outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeStringOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status setStringOutputs( const std::vector<std::string*>& outputs );

	///
	/// Write values to string outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeStringOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmi2String!
	///
	fmi2Status setStringOutputs( const std::string* outputs, size_t nOutputs );

	///
	/// Inform frontend what the next simulation time step will be.
	/// Call this method only before #endInitialization or between calls to #waitForMaster and #signalToMaster.
	///
	void enforceTimeStep( const fmi2Real& delta );

	///
	/// Inform frontend that the simulation step has been rejected.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	void rejectStep();


	///
	/// Call the internal logger.
	///
	void logger( fmi2Status status, const std::string& category, const std::string& msg );

	///
	/// Get current communication point from the front end.
	/// Call this method only before #endInitialization or between calls to #waitForMaster and #signalToMaster.
	///
	const fmi2Real& getCurrentCommunicationPoint() const;

	///
	/// Get next communication step size from the front end.
	/// Call this method only before #endInitialization or between calls to #waitForMaster and #signalToMaster.
	///
	const fmi2Real& getCommunicationStepSize() const;

	///
	/// Get simulation stop time.
	///
	const fmi2Real& getStopTime() const;

	///
	/// Get flag indicating if simulation stop time has been defined.
	///
	const bool& getStopTimeDefined() const;

	///
	/// Get full path of log messages file.
	///
	std::string getLogFileName() const;

	///
	/// Check if logging has been activated.
	///
	bool loggingOn() const;


	///
	/// Get names of all real inputs initialized by the front end.
	///
	void getRealInputNames( std::vector<std::string>& names ) const;

	///
	/// Get names of all integer inputs initialized by the front end.
	///
	void getIntegerInputNames( std::vector<std::string>& names ) const;

	///
	/// Get names of all boolean inputs initialized by the front end.
	///
	void getBooleanInputNames( std::vector<std::string>& names ) const;

	///
	/// Get names of all string inputs initialized by the front end.
	///
	void getStringInputNames( std::vector<std::string>& names ) const;


	///
	/// Get names of all real outputs initialized by the front end.
	///
	void getRealOutputNames( std::vector<std::string>& names ) const;

	///
	/// Get names of all integer outputs initialized by the front end.
	///
	void getIntegerOutputNames( std::vector<std::string>& names ) const;

	///
	/// Get names of all boolean outputs initialized by the front end.
	///
	void getBooleanOutputNames( std::vector<std::string>& names ) const;

	///
	/// Get names of all string outputs initialized by the front end.
	///
	void getStringOutputNames( std::vector<std::string>& names ) const;


private:

	///
	/// Internal helper function to get the process ID (or the
	/// parent process in case flag BACKEND_USE_PARENT_PID is
	/// set at compilation time).
	///
	const std::string getProcessID() const;

	///
	/// Internal helper function for initialization of inputs/outputs.
	///
	template<typename Type>
	fmi2Status initializeVariables( std::vector<Type*>& variablePointers,
				       const std::string& scalarCollection,
				       const std::vector<std::string>& scalarNames,
				       const ScalarVariableAttributes::Causality::Causality causality );

	///
	/// Internal helper function for initialization of inputs/outputs.
	///
	template<typename Type>
	fmi2Status initializeVariables( std::vector<Type*>& variablePointers,
				       const std::string& scalarCollection,
				       const std::string* scalarNames,
					   const size_t nScalarNames,
				       const ScalarVariableAttributes::Causality::Causality causality );

	///
	/// Internal helper function for retrieving variable names.
	///
	template<typename Type>
	void getScalarNames( std::vector<std::string>& scalarNames,
			     const std::string& scalarCollection,
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
	fmi2Real* currentCommunicationPoint_;

	///
	/// Next simulation time step size (requested by the master or enforced by the slave).
	///
	fmi2Real* communicationStepSize_;

	///
	/// Simulation stop time.
	///
	fmi2Real* stopTime_;

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
	std::vector<fmi2Real*> realParameters_;

	///
	/// Internal pointers to integer-valued parameters.
	///
	std::vector<fmi2Integer*> integerParameters_;

	///
	/// Internal pointers to boolean-valued parameters.
	///
	std::vector<fmi2Boolean*> booleanParameters_;

	///
	/// Internal pointers to string-valued parameters.
	/// Attention: Uses std::string instead of fmi2String!
	///
	std::vector<std::string*> stringParameters_;

	///
	/// Internal pointers to real-valued inputs.
	///
	std::vector<fmi2Real*> realInputs_;

	///
	/// Internal pointers to integer-valued inputs.
	///
	std::vector<fmi2Integer*> integerInputs_;

	///
	/// Internal pointers to boolean-valued inputs.
	///
	std::vector<fmi2Boolean*> booleanInputs_;

	///
	/// Internal pointers to string-valued inputs.
	/// Attention: Uses std::string instead of fmi2String!
	///
	std::vector<std::string*> stringInputs_;

	///
	/// Internal pointers to real-valued outputs.
	///
	std::vector<fmi2Real*> realOutputs_;

	///
	/// Internal pointers to integer-valued outputs.
	///
	std::vector<fmi2Integer*> integerOutputs_;

	///
	/// Internal pointers to boolean-valued outputs.
	///
	std::vector<fmi2Boolean*> booleanOutputs_;

	///
	/// Internal pointers to string-valued outputs.
	/// Attention: Uses std::string instead of fmi2String!
	///
	std::vector<std::string*> stringOutputs_;
};



template<typename Type>
fmi2Status FMIComponentBackEnd::initializeVariables( std::vector<Type*>& variablePointers,
	const std::string& scalarCollection,
	const std::string* scalarNames,
	const size_t nScalarNames,
	const ScalarVariableAttributes::Causality::Causality causality )
{
	std::vector<std::string> vecScalarNames( scalarNames, scalarNames + nScalarNames );
	return initializeVariables( variablePointers, scalarCollection, vecScalarNames, causality );
}


template<typename Type>
fmi2Status FMIComponentBackEnd::initializeVariables( std::vector<Type*>& variablePointers,
	const std::string& scalarCollection,
	const std::vector<std::string>& scalarNames,
	const ScalarVariableAttributes::Causality::Causality causality )
{
	fmi2Status result = fmi2OK;

	// Clear the vector real inputs.
	if ( false == variablePointers.empty() ) {
		variablePointers.clear();
		ipcLogger_->logger( fmi2Warning, "WARNING", "previous elements of input vector have been erased" );
	}

	if ( true == scalarNames.empty() ) return result;

	// Reserve correct number of elements.
	variablePointers.reserve( scalarNames.size() );

	// Retrieve scalars from master.
	std::vector< ScalarVariable<Type>* > scalars;
	ipcSlave_->retrieveScalars( scalarCollection, scalars );

	// Fill map between scalar names and instance pointers
	std::map< std::string, ScalarVariable<Type>* > scalarMap;
	typename std::vector< ScalarVariable<Type>* >::iterator itScalar = scalars.begin();
	typename std::vector< ScalarVariable<Type>* >::iterator endScalars = scalars.end();
	for ( ; itScalar != endScalars; ++itScalar ) {
		scalarMap[(*itScalar)->name_] = *itScalar;
	}

	// Iterators needed for searching the map.
	typename std::map< std::string, ScalarVariable<Type>* >::const_iterator itFind;
	typename std::map< std::string, ScalarVariable<Type>* >::const_iterator itFindEnd = scalarMap.end();

	// Loop through the input names, chack their causality and store pointer.
	typename std::vector<std::string>::const_iterator itName = scalarNames.begin();
	typename std::vector<std::string>::const_iterator itNamesEnd = scalarNames.end();
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
			ipcLogger_->logger( fmi2Fatal, "ABORT", err.str() );
			result = fmi2Fatal;
			break;
		} else {
			if ( causality != itFind->second->causality_ ) {
				std::stringstream err;
				err << "scalar variable '" << *itName << "' has wrong causality: "
				    << itFind->second->causality_ << " instead of " << causality;
				ipcLogger_->logger( fmi2Fatal, "ABORT", err.str() );
				result = fmi2Warning;
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
void FMIComponentBackEnd::getScalarNames( std::vector<std::string>& scalarNames,
					  const std::string& scalarCollection,
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
