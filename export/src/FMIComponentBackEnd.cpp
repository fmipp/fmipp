// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file FMIComponentBackEnd.cpp

#ifdef WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif

// #include <iostream>
// #include <fstream>

#include <boost/lexical_cast.hpp>

#include "export/include/FMIComponentBackEnd.h"
#include "export/include/ScalarVariable.h"
#include "export/include/SHMSlave.h"
#include "export/include/IPCSlaveLogger.h"

using namespace std;

template<>
fmippStatus FMIComponentBackEnd::initializeVariables( std::vector<IPCString*>& variablePointers,
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
	std::vector< ScalarVariable<fmippIPCString>* > scalars;
	ipcSlave_->retrieveScalars( scalarCollection, scalars );

	// Fill map between scalar names and instance pointers
	std::map< fmippString, ScalarVariable<fmippIPCString>* > scalarMap;
	typename std::vector< ScalarVariable<fmippIPCString>* >::iterator itScalar = scalars.begin();
	typename std::vector< ScalarVariable<fmippIPCString>* >::iterator endScalars = scalars.end();
	for ( ; itScalar != endScalars; ++itScalar ) {
		scalarMap[(*itScalar)->name_] = *itScalar;
	}

	// Iterators needed for searching the map.
	typename std::map< fmippString, ScalarVariable<fmippIPCString>* >::const_iterator itFind;
	typename std::map< fmippString, ScalarVariable<fmippIPCString>* >::const_iterator itFindEnd = scalarMap.end();

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

FMIComponentBackEnd::FMIComponentBackEnd() :
	ipcSlave_( 0 ),
	ipcLogger_( 0 ),
	currentCommunicationPoint_( 0 ),
	communicationStepSize_( 0 ),
	stopTime_( 0 ),
	stopTimeDefined_( 0 ),
	enforceTimeStep_( 0 ),
	rejectStep_( 0 ),
	slaveHasTerminated_( 0 ),
	loggingOn_( 0 )
{}

FMIComponentBackEnd::~FMIComponentBackEnd()
{
	if ( 0 != ipcSlave_ ) {
		// Notify frontend that the backend has been terminated.
		*slaveHasTerminated_ = true;

		ipcSlave_->signalToMaster();
		delete ipcSlave_;
	}

	if ( 0 != ipcLogger_ ) delete ipcLogger_;
}

///
/// Start initialization of the backend (connect/sync with master).
///
fmippStatus
FMIComponentBackEnd::startInitialization()
{
	string pid = getProcessID();

#ifdef SHM_SEGMENT_NAME
	// If this flag is set, the name specified along with it is
	// used for the name of the shared memory segment.
	string shmSegmentName = string( SHM_SEGMENT_NAME );
#else
	// Otherwise, use the process ID of the application (or the
	// parent process in case flag BACKEND_USE_PARENT_PID) to
	// generate the shared memory segment name.
	string shmSegmentName = string( "FMI_SEGMENT_PID" ) + pid;
#endif

	string loggerFileName = string( "fmibackend_pid" ) + pid + string( ".log" );

	ipcLogger_ = new IPCSlaveLogger( loggerFileName );
	ipcSlave_ = IPCSlaveFactory::createIPCSlave< SHMSlave >( shmSegmentName, ipcLogger_ );

	while ( false == ipcSlave_->isOperational() ) {
		ipcLogger_->logger( fmippWarning, "WARNING", "IPC interface not operational" );
		ipcSlave_->sleep( 3000 ); /// \FIXME waiting time should be configurable
		ipcLogger_->logger( fmippWarning, "WARNING", "retry to initialize IPC interface" );
		ipcSlave_->reinitialize();
	}

	ipcSlave_->waitForMaster();

	if ( false == ipcSlave_->retrieveVariable( "current_comm_point", currentCommunicationPoint_ ) ) {
		ipcLogger_->logger( fmippFatal, "ABORT", "unable to create internal variable 'current_comm_point'" );
		return fmippFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "comm_step_size", communicationStepSize_ ) ) {
		ipcLogger_->logger( fmippFatal, "ABORT", "unable to create internal variable 'comm_step_size'" );
		return fmippFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "stop_time", stopTime_ ) ) {
		ipcLogger_->logger( fmippFatal, "ABORT", "unable to create internal variable 'stop_time'" );
		return fmippFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "stop_time_defined", stopTimeDefined_ ) ) {
		ipcLogger_->logger( fmippFatal, "ABORT", "unable to create internal variable 'stop_time_defined'" );
		return fmippFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "enforce_step", enforceTimeStep_ ) ) {
		ipcLogger_->logger( fmippFatal, "ABORT", "unable to create internal variable 'enforce_step'" );
		return fmippFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "reject_step", rejectStep_ ) ) {
		ipcLogger_->logger( fmippFatal, "ABORT", "unable to create internal variable 'reject_step'" );
		return fmippFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "slave_has_terminated", slaveHasTerminated_ ) ) {
		ipcLogger_->logger( fmippFatal, "ABORT", "unable to create internal variable 'slave_has_terminated'" );
		return fmippFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "fmu_type", fmuType_ ) ) {
		ipcLogger_->logger( fmippFatal, "ABORT", "unable to create internal variable 'fmu_type'" );
		return fmippFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "logging_on", loggingOn_ ) ) {
		ipcLogger_->logger( fmippFatal, "ABORT", "unable to create internal variable 'logging_on'" );
		return fmippFatal;
	}

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "FMI component backend initialized successfully." );

	return fmippOK;
}

///
/// End initialization of the backend (connect/sync with master).
///
fmippStatus
FMIComponentBackEnd::endInitialization()
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function endInitialization" );

	ipcSlave_->signalToMaster(); /// \FIXME is there a way to check whether everthing went fine?

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "endInitialization done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeRealParameters( const std::vector<fmippString>& names, std::vector<fmippReal*>& params )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeRealParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "Invalid FMU type." );
			return fmippFatal;
	}

	fmippStatus status = initializeVariables( realParameters_, "real_scalars", names, causality );

	if ( fmippOK != status ) return fmippFatal;

	if ( params.size() != realParameters_.size() ) return fmippFatal;

	vector<fmippReal*>::const_iterator itParameter = params.begin();
	vector<fmippReal*>::iterator itCopy = realParameters_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeRealParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeRealParameters( const fmippString* names, fmippReal* params, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeRealParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "Invalid FMU type." );
			return fmippFatal;
	}

	fmippStatus status = initializeVariables( realParameters_, "real_scalars", names, n, causality );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != realParameters_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itCopy = realParameters_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++params ) *params = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeRealParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeIntegerParameters( const std::vector<fmippString>& names, std::vector<fmippInteger*>& params )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeIntegerParameters" );

		ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "Invalid FMU type." );
			return fmippFatal;
	}

	fmippStatus status = initializeVariables( integerParameters_, "integer_scalars", names, causality );

	if ( fmippOK != status ) return fmippFatal;

	if ( params.size() != integerParameters_.size() ) return fmippFatal;

	vector<fmippInteger*>::const_iterator itParameter = params.begin();
	vector<fmippInteger*>::iterator itCopy = integerParameters_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeIntegerParameters done" );

	return fmippOK;

}

fmippStatus
FMIComponentBackEnd::initializeIntegerParameters( const fmippString* names, fmippInteger* params, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeIntegerParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "Invalid FMU type." );
			return fmippFatal;
	}

	fmippStatus status = initializeVariables( integerParameters_, "integer_scalars", names, n, causality );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != integerParameters_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itCopy = integerParameters_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++params ) *params = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeIntegerParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeBooleanParameters( const std::vector<fmippString>& names, std::vector<fmippBoolean*>& params )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeBooleanParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "Invalid FMU type." );
			return fmippFatal;
	}

	fmippStatus status = initializeVariables( booleanParameters_, "boolean_scalars", names, causality );

	if ( fmippOK != status ) return fmippFatal;

	if ( params.size() != booleanParameters_.size() ) return fmippFatal;

	vector<fmippBoolean*>::const_iterator itParameter = params.begin();
	vector<fmippBoolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeBooleanParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeBooleanParameters( const fmippString* names, fmippBoolean* params, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeBooleanParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "Invalid FMU type." );
			return fmippFatal;
	}

	fmippStatus status = initializeVariables( booleanParameters_, "boolean_scalars", names, n, causality );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != booleanParameters_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++params ) *params = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeBooleanParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeStringParameters( const std::vector<fmippString>& names, std::vector<fmippString*>& params )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeStringParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "Invalid FMU type." );
			return fmippFatal;
	}

	fmippStatus status = initializeVariables( stringParameters_, "string_scalars", names, causality );

	if ( fmippOK != status ) return fmippFatal;

	if ( params.size() != stringParameters_.size() ) return fmippFatal;

	vector<fmippString*>::const_iterator itParameter = params.begin();
	vector<fmippIPCString*>::iterator itCopy = stringParameters_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeStringParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeStringParameters( const fmippString* names, fmippString* params, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeStringParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "Invalid FMU type." );
			return fmippFatal;
	}

	fmippStatus status = initializeVariables( stringParameters_, "string_scalars", names, n, causality );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != stringParameters_.size() ) return fmippFatal;

	vector<fmippIPCString*>::iterator itCopy = stringParameters_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++params ) *params = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeStringParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeRealInputs( const vector<fmippString>& names, std::vector<fmippReal*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeRealInputs" );

	fmippStatus status = initializeVariables( realInputs_, "real_scalars", names, ScalarVariableAttributes::Causality::input );

	if ( fmippOK != status ) return fmippFatal;

	if ( inputs.size() != realInputs_.size() ) return fmippFatal;

	vector<fmippReal*>::const_iterator itInput = inputs.begin();
	vector<fmippReal*>::iterator itCopy = realInputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeRealInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeRealInputs( const fmippString* names, fmippReal* inputs, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeRealInputs" );

	fmippStatus status = initializeVariables( realInputs_, "real_scalars", names, n, ScalarVariableAttributes::Causality::input );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != realInputs_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itCopy = realInputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeRealInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeIntegerInputs( const vector<fmippString>& names, std::vector<fmippInteger*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeIntegerInputs" );

	fmippStatus status = initializeVariables( integerInputs_, "integer_scalars", names, ScalarVariableAttributes::Causality::input );

	if ( fmippOK != status ) return fmippFatal;

	if ( inputs.size() != integerInputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::const_iterator itInput = inputs.begin();
	vector<fmippInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeIntegerInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeIntegerInputs( const fmippString* names, fmippInteger* inputs, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeIntegerInputs" );

	fmippStatus status = initializeVariables( integerInputs_, "integer_scalars", names, n, ScalarVariableAttributes::Causality::input );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != integerInputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeIntegerInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeBooleanInputs( const vector<fmippString>& names, std::vector<fmippBoolean*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeBooleanInputs" );

	fmippStatus status = initializeVariables( booleanInputs_, "boolean_scalars", names, ScalarVariableAttributes::Causality::input );

	if ( fmippOK != status ) return fmippFatal;

	if ( inputs.size() != booleanInputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::const_iterator itInput = inputs.begin();
	vector<fmippBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeBooleanInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeBooleanInputs( const fmippString* names, fmippBoolean* inputs, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeBooleanInputs" );

	fmippStatus status = initializeVariables( booleanInputs_, "boolean_scalars", names, n, ScalarVariableAttributes::Causality::input );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != booleanInputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeBooleanInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeStringInputs( const vector<fmippString>& names, std::vector<fmippString*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeStringInputs" );

	fmippStatus status = initializeVariables( stringInputs_, "string_scalars", names, ScalarVariableAttributes::Causality::input );

	if ( fmippOK != status ) return fmippFatal;

	if ( inputs.size() != stringInputs_.size() ) return fmippFatal;

	vector<fmippString*>::const_iterator itInput = inputs.begin();
	vector<fmippIPCString*>::iterator itCopy = stringInputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeStringInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeStringInputs( const fmippString* names, fmippString* inputs, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeStringInputs" );

	fmippStatus status = initializeVariables( stringInputs_, "string_scalars", names, n, ScalarVariableAttributes::Causality::input );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != stringInputs_.size() ) return fmippFatal;

	vector<fmippIPCString*>::iterator itCopy = stringInputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeStringInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeRealOutputs( const vector<fmippString>& names, vector<fmippReal*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeRealOutputs" );

	fmippStatus status = initializeVariables( realOutputs_, "real_scalars", names, ScalarVariableAttributes::Causality::output );

	if ( fmippOK != status ) return fmippFatal;

	if ( outputs.size() != realOutputs_.size() ) return fmippFatal;

	vector<fmippReal*>::const_iterator itOutput = outputs.begin();
	vector<fmippReal*>::iterator itCopy = realOutputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itOutput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeRealOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeRealOutputs( const fmippString* names, fmippReal* outputs, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeRealOutputs" );

	fmippStatus status = initializeVariables( realOutputs_, "real_scalars", names, n, ScalarVariableAttributes::Causality::output );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != realOutputs_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itCopy = realOutputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) *outputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeRealOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeIntegerOutputs( const vector<fmippString>& names, std::vector<fmippInteger*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeIntegerOutputs" );

	fmippStatus status = initializeVariables( integerOutputs_, "integer_scalars", names, ScalarVariableAttributes::Causality::output );

	if ( fmippOK != status ) return fmippFatal;

	if ( outputs.size() != integerOutputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::const_iterator itOutput = outputs.begin();
	vector<fmippInteger*>::iterator itCopy = integerOutputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itOutput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeIntegerOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeIntegerOutputs( const fmippString* names, fmippInteger* outputs, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeIntegerOutputs" );

	fmippStatus status = initializeVariables( integerOutputs_, "integer_scalars", names, n, ScalarVariableAttributes::Causality::output );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != integerOutputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itCopy = integerOutputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) *outputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeIntegerOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeBooleanOutputs( const vector<fmippString>& names, std::vector<fmippBoolean*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeBooleanOutputs" );

	fmippStatus status = initializeVariables( booleanOutputs_, "boolean_scalars", names, ScalarVariableAttributes::Causality::output );

	if ( fmippOK != status ) return fmippFatal;

	if ( outputs.size() != booleanOutputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::const_iterator itOutput = outputs.begin();
	vector<fmippBoolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itOutput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeBooleanOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeBooleanOutputs( const fmippString* names, fmippBoolean* outputs, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeBooleanOutputs" );

	fmippStatus status = initializeVariables( booleanOutputs_, "boolean_scalars", names, n, ScalarVariableAttributes::Causality::output );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != booleanOutputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) *outputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeBooleanOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeStringOutputs( const vector<fmippString>& names, std::vector<fmippString*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeStringOutputs" );

	fmippStatus status = initializeVariables( stringOutputs_, "string_scalars", names, ScalarVariableAttributes::Causality::output );

	if ( fmippOK != status ) return fmippFatal;

	if ( outputs.size() != stringOutputs_.size() ) return fmippFatal;

	vector<fmippString*>::const_iterator itOutput = outputs.begin();
	vector<fmippIPCString*>::iterator itCopy = stringOutputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itOutput = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeStringOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::initializeStringOutputs( const fmippString* names, fmippString* outputs, const fmippSize n )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function initializeStringOutputs" );

	fmippStatus status = initializeVariables( stringOutputs_, "string_scalars", names, n, ScalarVariableAttributes::Causality::output );

	if ( fmippOK != status ) return fmippFatal;

	if ( n != stringOutputs_.size() ) return fmippFatal;

	vector<fmippIPCString*>::iterator itCopy = stringOutputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) *outputs = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "initializeStringOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getRealParameters( vector<fmippReal*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getRealParameters" );

	if ( parameters.size() != realParameters_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itParameter = parameters.begin();
	vector<fmippReal*>::iterator itCopy = realParameters_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getRealParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getRealParameters( fmippReal* parameters, fmippSize nParameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getRealParameters" );

	if ( nParameters != realParameters_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itCopy = realParameters_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) *parameters = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getRealParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getIntegerParameters( vector<fmippInteger*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getIntegerParameters" );

	if ( parameters.size() != integerParameters_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itParameter = parameters.begin();
	vector<fmippInteger*>::iterator itCopy = integerParameters_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getIntegerParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getIntegerParameters( fmippInteger* parameters, fmippSize nParameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getIntegerParameters" );

	if ( nParameters != integerParameters_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itCopy = integerParameters_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) *parameters = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getIntegerParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getBooleanParameters( vector<fmippBoolean*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getBooleanParameters" );

	if ( parameters.size() != booleanParameters_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itParameter = parameters.begin();
	vector<fmippBoolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getBooleanParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getBooleanParameters( fmippBoolean* parameters, fmippSize nParameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getBooleanParameters" );

	if ( nParameters != booleanParameters_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) *parameters = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getBooleanParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getStringParameters( vector<fmippString*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getStringParameters" );

	if ( parameters.size() != stringParameters_.size() ) return fmippFatal;

	vector<fmippString*>::iterator itParameter = parameters.begin();
	vector<fmippIPCString*>::iterator itCopy = stringParameters_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getStringParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getStringParameters( fmippString* parameters, fmippSize nParameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getStringParameters" );

	if ( nParameters != stringParameters_.size() ) return fmippFatal;

	vector<fmippIPCString*>::iterator itCopy = stringParameters_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) *parameters = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getStringParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setRealParameters( const vector<fmippReal*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setRealParameters" );

	if ( parameters.size() != realParameters_.size() ) return fmippFatal;

	vector<fmippReal*>::const_iterator itParameter = parameters.begin();
	vector<fmippReal*>::iterator itCopy = realParameters_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itCopy = **itParameter;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setRealParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setRealParameters( const fmippReal* parameters, fmippSize nParameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setRealParameters" );

	if ( nParameters != realParameters_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itCopy = realParameters_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) **itCopy = *parameters;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setRealParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setIntegerParameters( const vector<fmippInteger*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setIntegerParameters" );

	if ( parameters.size() != integerParameters_.size() ) return fmippFatal;

	vector<fmippInteger*>::const_iterator itParameter = parameters.begin();
	vector<fmippInteger*>::iterator itCopy = integerParameters_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itCopy = **itParameter;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setIntegerParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setIntegerParameters( const fmippInteger* parameters, fmippSize nParameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setIntegerParameters" );

	if ( nParameters != integerParameters_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itCopy = integerParameters_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) **itCopy = *parameters;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setIntegerParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setBooleanParameters( const vector<fmippBoolean*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setBooleanParameters" );

	if ( outputs.size() != booleanParameters_.size() ) return fmippFatal;

	vector<fmippBoolean*>::const_iterator itParameter = outputs.begin();
	vector<fmippBoolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itCopy = **itParameter;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setBooleanParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setBooleanParameters( const fmippBoolean* outputs, fmippSize nParameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setBooleanParameters" );

	if ( nParameters != booleanParameters_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setBooleanParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setStringParameters( const vector<fmippString*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setStringParameters" );

	if ( outputs.size() != stringParameters_.size() ) return fmippFatal;

	vector<fmippString*>::const_iterator itParameter = outputs.begin();
	vector<fmippIPCString*>::iterator itCopy = stringParameters_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itCopy = (*itParameter)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setStringParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setStringParameters( const fmippString* outputs, fmippSize nParameters )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setStringParameters" );

	if ( nParameters != stringParameters_.size() ) return fmippFatal;

	vector<fmippIPCString*>::iterator itCopy = stringParameters_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = outputs->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setStringParameters done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getRealInputs( vector<fmippReal*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getRealInputs" );

	if ( inputs.size() != realInputs_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itInput = inputs.begin();
	vector<fmippReal*>::iterator itCopy = realInputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getRealInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getRealInputs( fmippReal* inputs, fmippSize nInputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getRealInputs" );

	if ( nInputs != realInputs_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itCopy = realInputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getRealInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getIntegerInputs( vector<fmippInteger*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getIntegerInputs" );

	if ( inputs.size() != integerInputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itInput = inputs.begin();
	vector<fmippInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getIntegerInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getIntegerInputs( fmippInteger* inputs, fmippSize nInputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getIntegerInputs" );

	if ( nInputs != integerInputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getIntegerInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getBooleanInputs( vector<fmippBoolean*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getBooleanInputs" );

	if ( inputs.size() != booleanInputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itInput = inputs.begin();
	vector<fmippBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getBooleanInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getBooleanInputs( fmippBoolean* inputs, fmippSize nInputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getBooleanInputs" );

	if ( nInputs != booleanInputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getBooleanInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getStringInputs( vector<fmippString*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getStringInputs" );

	if ( inputs.size() != stringInputs_.size() ) return fmippFatal;

	vector<fmippString*>::iterator itInput = inputs.begin();
	vector<fmippIPCString*>::iterator itCopy = stringInputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getStringInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::getStringInputs( fmippString* inputs, fmippSize nInputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function getStringInputs" );

	if ( nInputs != stringInputs_.size() ) return fmippFatal;

	vector<fmippIPCString*>::iterator itCopy = stringInputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = (*itCopy)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "getStringInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::resetRealInputs( std::vector<fmippReal*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function resetRealInputs" );

	if ( inputs.size() != realInputs_.size() ) return fmippFatal;

	vector<fmippReal*>::const_iterator itInput = inputs.begin();
	vector<fmippReal*>::iterator itCopy = realInputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itCopy = **itInput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "resetRealInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::resetRealInputs( fmippReal* inputs, fmippSize nInputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function resetRealInputs" );

	if ( nInputs != realInputs_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itCopy = realInputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) **itCopy = *inputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "resetRealInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::resetIntegerInputs( std::vector<fmippInteger*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function resetIntegerInputs" );

	if ( inputs.size() != integerInputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::const_iterator itInput = inputs.begin();
	vector<fmippInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itCopy = **itInput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "resetIntegerInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::resetIntegerInputs( fmippInteger* inputs, fmippSize nInputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function resetIntegerInputs" );

	if ( nInputs != integerInputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) **itCopy = *inputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "resetIntegerInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::resetBooleanInputs( std::vector<fmippBoolean*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function resetBooleanInputs" );

	if ( inputs.size() != booleanInputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::const_iterator itInput = inputs.begin();
	vector<fmippBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itCopy = **itInput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "resetBooleanInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::resetBooleanInputs( fmippBoolean* inputs, fmippSize nInputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function resetBooleanInputs" );

	if ( nInputs != booleanInputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) **itCopy = *inputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "resetBooleanInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::resetStringInputs( std::vector<fmippString*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function resetStringInputs" );

	if ( inputs.size() != stringInputs_.size() ) return fmippFatal;

	vector<fmippString*>::const_iterator itInput = inputs.begin();
	vector<fmippIPCString*>::iterator itCopy = stringInputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itCopy = (*itInput)->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "resetStringInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::resetStringInputs( fmippString* inputs, fmippSize nInputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function resetStringInputs" );

	if ( nInputs != stringInputs_.size() ) return fmippFatal;

	vector<fmippIPCString*>::iterator itCopy = stringInputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) **itCopy = inputs->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "resetStringInputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setRealOutputs( const vector<fmippReal*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setRealOutputs" );

	if ( outputs.size() != realOutputs_.size() ) return fmippFatal;

	vector<fmippReal*>::const_iterator itOutput = outputs.begin();
	vector<fmippReal*>::iterator itCopy = realOutputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setRealOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setRealOutputs( const fmippReal* outputs, fmippSize nOutputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setRealOutputs" );

	if ( nOutputs != realOutputs_.size() ) return fmippFatal;

	vector<fmippReal*>::iterator itCopy = realOutputs_.begin();
	vector<fmippReal*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setRealOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setIntegerOutputs( const vector<fmippInteger*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setIntegerOutputs" );

	if ( outputs.size() != integerOutputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::const_iterator itOutput = outputs.begin();
	vector<fmippInteger*>::iterator itCopy = integerOutputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setIntegerOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setIntegerOutputs( const fmippInteger* outputs, fmippSize nOutputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setIntegerOutputs" );

	if ( nOutputs != integerOutputs_.size() ) return fmippFatal;

	vector<fmippInteger*>::iterator itCopy = integerOutputs_.begin();
	vector<fmippInteger*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setIntegerOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setBooleanOutputs( const vector<fmippBoolean*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setBooleanOutputs" );

	if ( outputs.size() != booleanOutputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::const_iterator itOutput = outputs.begin();
	vector<fmippBoolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setBooleanOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setBooleanOutputs( const fmippBoolean* outputs, fmippSize nOutputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setBooleanOutputs" );

	if ( nOutputs != booleanOutputs_.size() ) return fmippFatal;

	vector<fmippBoolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmippBoolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setBooleanOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setStringOutputs( const vector<fmippString*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setStringOutputs" );

	if ( outputs.size() != stringOutputs_.size() ) return fmippFatal;

	vector<fmippString*>::const_iterator itOutput = outputs.begin();
	vector<fmippIPCString*>::iterator itCopy = stringOutputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) {
		**itCopy = (*itOutput)->c_str();
	}

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setStringOutputs done" );

	return fmippOK;
}

fmippStatus
FMIComponentBackEnd::setStringOutputs( const fmippString* outputs, fmippSize nOutputs )
{
	if ( 0 == ipcSlave_ ) return fmippFatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function setStringOutputs" );

	if ( nOutputs != stringOutputs_.size() ) return fmippFatal;

	vector<fmippIPCString*>::iterator itCopy = stringOutputs_.begin();
	vector<fmippIPCString*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = outputs->c_str();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "setStringOutputs done" );

	return fmippOK;
}

///
/// Wait for signal from master to resume execution.
/// Blocks until signal from master is received.
///
void
FMIComponentBackEnd::waitForMaster() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function waitForMaster" );

	ipcSlave_->waitForMaster();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "waitForMaster done" );
}

///
/// Send signal to master to proceed with execution.
/// Do not alter shared data until waitForMaster() unblocks.
///
void
FMIComponentBackEnd::signalToMaster() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function signalToMaster" );

	ipcSlave_->signalToMaster();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "signalToMaster done" );
}

///
/// Inform frontend what the next simulation time step will be.
///
void
FMIComponentBackEnd::enforceTimeStep( const fmippTime& delta )
{
	if ( 0 == ipcSlave_ ) return;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function enforceTimeStep" );

	*enforceTimeStep_ = true;
	*communicationStepSize_ = delta;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "enforceTimeStep done" );
}

///
/// Inform frontend that the simulation step has been rejected.
///
void
FMIComponentBackEnd::rejectStep()
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "entering function rejectStep" );

	*rejectStep_ = true;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "rejectStep done" );
}

const fmippTime&
FMIComponentBackEnd::getCurrentCommunicationPoint() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function getCurrentCommunicationPoint" );

	return *currentCommunicationPoint_;
}

const fmippTime&
FMIComponentBackEnd::getCommunicationStepSize() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function getCommunicationStepSize" );

	return *communicationStepSize_;
}

const fmippReal&
FMIComponentBackEnd::getStopTime() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function getStopTime" );

	return *stopTime_;
}

const bool&
FMIComponentBackEnd::getStopTimeDefined() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function getStopTimeDefined" );

	return *stopTimeDefined_;
}

fmippString
FMIComponentBackEnd::getLogFileName() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmippOK, "DEBUG", "calling function getLogFileName" );

	return ipcLogger_->getLogFileName();
}

bool
FMIComponentBackEnd::loggingOn() const
{
	return loggingOn_;
}

///
/// Call the internal logger.
///
void
FMIComponentBackEnd::logger( fmippStatus status, const fmippString& category, const fmippString& msg )
{
	ipcLogger_->logger( fmippWarning, category, msg );
}

///
/// Get names of all real inputs initialized by the front end.
///
void
FMIComponentBackEnd::getRealInputNames( std::vector<fmippString>& names ) const
{
	getScalarNames<fmippReal>( names, "real_scalars", ScalarVariableAttributes::Causality::input );
}

///
/// Get names of all integer inputs initialized by the front end.
///
void
FMIComponentBackEnd::getIntegerInputNames( std::vector<fmippString>& names ) const
{
	getScalarNames<fmippReal>( names, "integer_scalars", ScalarVariableAttributes::Causality::input );
}

///
/// Get names of all boolean inputs initialized by the front end.
///
void
FMIComponentBackEnd::getBooleanInputNames( std::vector<fmippString>& names ) const
{
	getScalarNames<fmippReal>( names, "boolean_scalars", ScalarVariableAttributes::Causality::input );
}

///
/// Get names of all string inputs initialized by the front end.
///
void
FMIComponentBackEnd::getStringInputNames( std::vector<fmippString>& names ) const
{
	getScalarNames<fmippReal>( names, "string_scalars", ScalarVariableAttributes::Causality::input );
}

///
/// Get names of all real outputs initialized by the front end.
///
void
FMIComponentBackEnd::getRealOutputNames( std::vector<fmippString>& names ) const
{
	getScalarNames<fmippReal>( names, "real_scalars", ScalarVariableAttributes::Causality::output );
}

///
/// Get names of all integer outputs initialized by the front end.
///
void
FMIComponentBackEnd::getIntegerOutputNames( std::vector<fmippString>& names ) const
{
	getScalarNames<fmippReal>( names, "integer_scalars", ScalarVariableAttributes::Causality::output );
}

///
/// Get names of all boolean outputs initialized by the front end.
///
void
FMIComponentBackEnd::getBooleanOutputNames( std::vector<fmippString>& names ) const
{
	getScalarNames<fmippReal>( names, "boolean_scalars", ScalarVariableAttributes::Causality::output );
}

///
/// Get names of all string outputs initialized by the front end.
///
void
FMIComponentBackEnd::getStringOutputNames( std::vector<fmippString>& names ) const
{
	getScalarNames<fmippReal>( names, "string_scalars", ScalarVariableAttributes::Causality::output );
}

///
/// Internal helper function to get the process ID (or the
/// parent process in case flag BACKEND_USE_PARENT_PID is
/// set at compilation time).
///
const string
FMIComponentBackEnd::getProcessID() const
{
#ifdef BACKEND_USE_PARENT_PID
	// If this flag is set, retrieve the PID of the parent process.
	// This usefull in case the application using the back end component
	// was started by another application (e.g., a shell script).

#ifdef WIN32
	DWORD dwProcessID = GetCurrentProcessId();
	DWORD dwParentProcessID = -1;
	HANDLE hProcessSnapshot;
	PROCESSENTRY32 processEntry32;

	hProcessSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if ( hProcessSnapshot != INVALID_HANDLE_VALUE )
	{
		processEntry32.dwSize = sizeof( PROCESSENTRY32 );

		if ( Process32First( hProcessSnapshot, &processEntry32 ) )
		{
			do {
				if ( dwProcessID == processEntry32.th32ProcessID )
				{
					dwParentProcessID = processEntry32.th32ParentProcessID;
					break;
				}
			} while ( Process32Next( hProcessSnapshot, &processEntry32 ) );

			CloseHandle( hProcessSnapshot ) ;
		}
	}

	return boost::lexical_cast<string>( dwParentProcessID );
#else
	return boost::lexical_cast<string>( getppid() );
#endif

#else // Retrieve the PID of this process.

#ifdef WIN32
	return boost::lexical_cast<string>( GetCurrentProcessId() );
#else
	return boost::lexical_cast<string>( getpid() );
#endif

#endif
}
