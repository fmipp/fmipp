/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMICOMPONENTBACKEND_H
#define _FMIPP_FMICOMPONENTBACKEND_H

#include <string>
#include <vector>
#include <map>

#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/FMIPPConfig.h"

#include "export/include/ScalarVariable.h"
#include "export/include/IPCSlave.h"


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
	/// Initialize real variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeRealInputs( const std::vector<std::string>& names );

	///
	/// Initialize integer variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeIntegerInputs( const std::vector<std::string>& names );

	///
	/// Initialize boolean variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeBooleanInputs( const std::vector<std::string>& names );

	///
	/// Initialize string variables for input.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeStringInputs( const std::vector<std::string>& names );

	///
	/// Initialize real variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeRealOutputs( const std::vector<std::string>& names );

	///
	/// Initialize integer variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeIntegerOutputs( const std::vector<std::string>& names );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeBooleanOutputs( const std::vector<std::string>& names );

	///
	/// Initialize boolean variables for output.
	/// Intended to be called after #startInitialization and before #endInitialization.
	///
	fmiStatus initializeStringOutputs( const std::vector<std::string>& names );

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
	fmiStatus getRealInputs( fmiReal*& inputs, size_t nInputs );

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
	fmiStatus getIntegerInputs( fmiInteger*& inputs, size_t nInputs );

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
	fmiStatus getBooleanInputs( fmiBoolean*& inputs, size_t nInputs );

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
	fmiStatus getStringInputs( std::string*& inputs, size_t nInputs );

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
	///
	void enforceTimeStep( const fmiReal& delta );

	///
	/// Inform frontend that the simulation step has been rejected.
	///
	void rejectStep();


	const fmiReal& getMasterTime() const;
	const fmiReal& getNextStepSize() const;


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
	/// Interface for inter-process communication.
	///
	IPCSlave* ipcSlave_;

	///
	/// Simulation time as requested by the master.
	///
	fmiReal* masterTime_;

	///
	/// Next simulation time step size (requested by the master or enforced by the slave).
	///
	fmiReal* nextStepSize_;

	///
	/// Flag for enforcing simulation time step size.
	///
	bool* enforceTimeStep_;

	///
	/// Flag for rejecting a simulation step.
	///
	bool* rejectStep_;

	bool* slaveHasTerminated_;

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
						    const std::vector<std::string>& scalarNames,
						    const ScalarVariableAttributes::Causality causality )
{
	fmiStatus result = fmiOK;

	// Clear the vector real inputs.
	/// \FIXME Clear only in case it is not empty and issue a warning (via function logger).
	variablePointers.clear();

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
	for ( ; itName != itNamesEnd; ++ itName )
	{
		// Search for name in map.
		itFind = scalarMap.find( *itName );

		// Check if scalar according to the name exists.
		if ( itFind == itFindEnd )
		{
			/// \FIXME Call function logger.
			result = fmiFatal;
			break;
		} else {
			if ( causality != itFind->second->causality_ ) {
				/// \FIXME Call function logger.
				result = fmiWarning;
			}

			/// \FIXME What about variability of scalar variable?

			// Get value.
			variablePointers.push_back( &itFind->second->value_ );
		}
	}

	return result;
}

#endif // _FMIPP_FMICOMPONENTBACKEND_H
