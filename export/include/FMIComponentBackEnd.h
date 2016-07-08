/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMICOMPONENTBACKEND_H
#define _FMIPP_FMICOMPONENTBACKEND_H

#include <string>
#include <vector>
#include <sstream>
#include <map>

#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/FMIPPConfig.h"

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
	fmiStatus startInitialization();

	///
	/// End initialization of the backend (connect/sync with master).
	///
	fmiStatus endInitialization();

	///
	/// Initialize real parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeRealParameters( const std::vector<std::string>& names );

	///
	/// Initialize real parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeRealParameters( const std::string* names, const size_t nNames );

	///
	/// Initialize integer parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeIntegerParameters( const std::vector<std::string>& names );

	///
	/// Initialize integer parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeIntegerParameters( const std::string* names, const size_t nNames );

	///
	/// Initialize boolean parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeBooleanParameters( const std::vector<std::string>& names );

	///
	/// Initialize boolean parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeBooleanParameters( const std::string* names, const size_t nNames );

	///
	/// Initialize string parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeStringParameters( const std::vector<std::string>& names );

	///
	/// Initialize string parameters.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeStringParameters( const std::string* names, const size_t nNames );

	///
	/// Initialize real variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeRealInputs( const std::vector<std::string>& names );

	///
	/// Initialize real variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeRealInputs( const std::string* names, const size_t nNames );

	///
	/// Initialize integer variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeIntegerInputs( const std::vector<std::string>& names );

	///
	/// Initialize integer variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeIntegerInputs( const std::string* names, const size_t nNames );

	///
	/// Initialize boolean variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeBooleanInputs( const std::vector<std::string>& names );

	///
	/// Initialize boolean variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeBooleanInputs( const std::string* names, const size_t nNames );

	///
	/// Initialize string variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeStringInputs( const std::vector<std::string>& names );

	///
	/// Initialize string variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeStringInputs( const std::string* names, const size_t nNames );

	///
	/// Initialize real variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeRealOutputs( const std::vector<std::string>& names );

	///
	/// Initialize real variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeRealOutputs( const std::string* names, const size_t nNames );

	///
	/// Initialize integer variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeIntegerOutputs( const std::vector<std::string>& names );

	///
	/// Initialize integer variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeIntegerOutputs( const std::string* names, const size_t nNames );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeBooleanOutputs( const std::vector<std::string>& names );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeBooleanOutputs( const std::string* names, const size_t nNames );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeStringOutputs( const std::vector<std::string>& names );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeStringOutputs( const std::string* names, const size_t nNames );

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
	fmiStatus getRealParameters( std::vector<fmiReal*>& parameters );

	///
	/// Read values from real parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeRealParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getRealParameters( fmiReal* parameters, size_t nParameters );

	///
	/// Read values from integer parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getIntegerParameters( std::vector<fmiInteger*>& parameters );

	///
	/// Read values from integer parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeIntegerParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getIntegerParameters( fmiInteger* parameters, size_t nParameters );

	///
	/// Read values from boolean parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getBooleanParameters( std::vector<fmiBoolean*>& parameters );

	///
	/// Read values from boolean parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getBooleanParameters( fmiBoolean* parameters, size_t nParameters );

	///
	/// Read values from string parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmiString!
	///
	fmiStatus getStringParameters( std::vector<std::string*>& parameters );

	///
	/// Read values from string parameters.
	/// Parameters are assumed to be in the same order as specified by #initializeBoolParameters.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmiString!
	///
	fmiStatus getStringParameters( std::string* parameters, size_t nParameters );

	///
	/// Read values from real inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getRealInputs( std::vector<fmiReal*>& inputs );

	///
	/// Read values from real inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getRealInputs( fmiReal* inputs, size_t nInputs );

	///
	/// Read values from integer inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getIntegerInputs( std::vector<fmiInteger*>& inputs );

	///
	/// Read values from integer inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getIntegerInputs( fmiInteger* inputs, size_t nInputs );

	///
	/// Read values from boolean inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getBooleanInputs( std::vector<fmiBoolean*>& inputs );

	///
	/// Read values from boolean inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus getBooleanInputs( fmiBoolean* inputs, size_t nInputs );

	///
	/// Read values from string inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmiString!
	///
	fmiStatus getStringInputs( std::vector<std::string*>& inputs );

	///
	/// Read values from string inputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBoolInputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmiString!
	///
	fmiStatus getStringInputs( std::string* inputs, size_t nInputs );

	///
	/// Write values to real outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus setRealOutputs( const std::vector<fmiReal*>& outputs );

	///
	/// Write values to real outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeRealOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus setRealOutputs( const fmiReal* outputs, size_t nOutputs );

	///
	/// Write values to integer outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus setIntegerOutputs( const std::vector<fmiInteger*>& outputs );

	///
	/// Write values to integer outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeIntegerOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus setIntegerOutputs( const fmiInteger* outputs, size_t nOutputs );

	///
	/// Write values to boolean outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBooleanOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus setBooleanOutputs( const std::vector<fmiBoolean*>& outputs );

	///
	/// Write values to boolean outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeBooleanOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	fmiStatus setBooleanOutputs( const fmiBoolean* outputs, size_t nOutputs );

	///
	/// Write values to string outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeStringOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmiString!
	///
	fmiStatus setStringOutputs( const std::vector<std::string*>& outputs );

	///
	/// Write values to string outputs.
	/// Inputs are assumed to be in the same order as specified by #initializeStringOutputs.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	/// Attention: Uses std::string instead of fmiString!
	///
	fmiStatus setStringOutputs( const std::string* outputs, size_t nOutputs );

	///
	/// Inform frontend what the next simulation time step will be.
	/// Call this method only before #endInitialization or between calls to #waitForMaster and #signalToMaster.
	///
	void enforceTimeStep( const fmiReal& delta );

	///
	/// Inform frontend that the simulation step has been rejected.
	/// Call this method only between calls to #waitForMaster and #signalToMaster.
	///
	void rejectStep();


	///
	/// Call the internal logger.
	///
	void logger( fmiStatus status, const std::string& category, const std::string& msg );

	///
	/// Get current communication point from the front end.
	/// Call this method only before #endInitialization or between calls to #waitForMaster and #signalToMaster.
	///
	const fmiReal& getCurrentCommunicationPoint() const;

	///
	/// Get next communication step size from the front end.
	/// Call this method only before #endInitialization or between calls to #waitForMaster and #signalToMaster.
	///
	const fmiReal& getCommunicationStepSize() const;

	///
	/// Get full path of log messages file.
	///
	std::string getLogFileName() const;


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
	/// Internal helper function for initialization of inputs/outputs.
	///
	template<typename Type>
	fmiStatus initializeVariables( std::vector<Type*>& variablePointers,
				       const std::string& scalarCollection,
				       const std::vector<std::string>& scalarNames,
				       const ScalarVariableAttributes::Causality causality );

	///
	/// Internal helper function for initialization of inputs/outputs.
	///
	template<typename Type>
	fmiStatus initializeVariables( std::vector<Type*>& variablePointers,
				       const std::string& scalarCollection,
				       const std::string* scalarNames,
					   const size_t nScalarNames,
				       const ScalarVariableAttributes::Causality causality );

	///
	/// Internal helper function for retrieving variable names.
	///
	template<typename Type>
	void getScalarNames( std::vector<std::string>& scalarNames,
			     const std::string& scalarCollection,
			     const ScalarVariableAttributes::Causality causality ) const;

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
	fmiReal* currentCommunicationPoint_;

	///
	/// Next simulation time step size (requested by the master or enforced by the slave).
	///
	fmiReal* communicationStepSize_;

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
	/// Flag for logging on/off.
	///
	bool* loggingOn_;
	
	///
	/// Internal pointers to real-valued parameters.
	///
	std::vector<fmiReal*> realParameters_;

	///
	/// Internal pointers to integer-valued parameters.
	///
	std::vector<fmiInteger*> integerParameters_;

	///
	/// Internal pointers to boolean-valued parameters.
	///
	std::vector<fmiBoolean*> booleanParameters_;

	///
	/// Internal pointers to string-valued parameters.
	/// Attention: Uses std::string instead of fmiString!
	///
	std::vector<std::string*> stringParameters_;

	///
	/// Internal pointers to real-valued inputs.
	///
	std::vector<fmiReal*> realInputs_;

	///
	/// Internal pointers to integer-valued inputs.
	///
	std::vector<fmiInteger*> integerInputs_;

	///
	/// Internal pointers to boolean-valued inputs.
	///
	std::vector<fmiBoolean*> booleanInputs_;

	///
	/// Internal pointers to string-valued inputs.
	/// Attention: Uses std::string instead of fmiString!
	///
	std::vector<std::string*> stringInputs_;

	///
	/// Internal pointers to real-valued outputs.
	///
	std::vector<fmiReal*> realOutputs_;

	///
	/// Internal pointers to integer-valued outputs.
	///
	std::vector<fmiInteger*> integerOutputs_;

	///
	/// Internal pointers to boolean-valued outputs.
	///
	std::vector<fmiBoolean*> booleanOutputs_;

	///
	/// Internal pointers to string-valued outputs.
	/// Attention: Uses std::string instead of fmiString!
	///
	std::vector<std::string*> stringOutputs_;
};



template<typename Type>
fmiStatus FMIComponentBackEnd::initializeVariables( std::vector<Type*>& variablePointers,
						    const std::string& scalarCollection,
						    const std::string* scalarNames,
							const size_t nScalarNames,
						    const ScalarVariableAttributes::Causality causality )
{
	std::vector<std::string> vecScalarNames( scalarNames, scalarNames + nScalarNames );
	return initializeVariables( variablePointers, scalarCollection, vecScalarNames, causality );
}


template<typename Type>
fmiStatus FMIComponentBackEnd::initializeVariables( std::vector<Type*>& variablePointers,
						    const std::string& scalarCollection,
						    const std::vector<std::string>& scalarNames,
						    const ScalarVariableAttributes::Causality causality )
{
	fmiStatus result = fmiOK;

	// Clear the vector real inputs.
	if ( false == variablePointers.empty() ) {
		variablePointers.clear();
		ipcLogger_->logger( fmiWarning, "WARNING", "previous elements of input vector have been erased" );
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
			ipcLogger_->logger( fmiFatal, "ABORT", err.str() );
			result = fmiFatal;
			break;
		} else {
			if ( causality != itFind->second->causality_ ) {
				std::stringstream err;
				err << "scalar variable '" << *itName << "' has wrong causality: "
				    << itFind->second->causality_ << " instead of " << causality;
				ipcLogger_->logger( fmiFatal, "ABORT", err.str() );
				result = fmiWarning;
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
					  const ScalarVariableAttributes::Causality causality ) const
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
