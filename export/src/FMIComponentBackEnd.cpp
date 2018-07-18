// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file FMIComponentBackEnd.cpp

#ifdef WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif

//#include <iostream>
#include <fstream>

#include <boost/lexical_cast.hpp>


#include "export/include/FMIComponentBackEnd.h"
#include "export/include/ScalarVariable.h"
#include "export/include/SHMSlave.h"
#include "export/include/IPCSlaveLogger.h"


using namespace std;


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
fmi2Status
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
		ipcLogger_->logger( fmi2Warning, "WARNING", "IPC interface not operational" );
		ipcSlave_->sleep( 3000 ); /// \FIXME waiting time should be configurable
		ipcLogger_->logger( fmi2Warning, "WARNING", "retry to initialize IPC interface" );
		ipcSlave_->reinitialize();
	}

	ipcSlave_->waitForMaster();

	if ( false == ipcSlave_->retrieveVariable( "current_comm_point", currentCommunicationPoint_ ) ) {
		ipcLogger_->logger( fmi2Fatal, "ABORT", "unable to create internal variable 'current_comm_point'" );
		return fmi2Fatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "comm_step_size", communicationStepSize_ ) ) {
		ipcLogger_->logger( fmi2Fatal, "ABORT", "unable to create internal variable 'comm_step_size'" );
		return fmi2Fatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "stop_time", stopTime_ ) ) {
		ipcLogger_->logger( fmi2Fatal, "ABORT", "unable to create internal variable 'stop_time'" );
		return fmi2Fatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "stop_time_defined", stopTimeDefined_ ) ) {
		ipcLogger_->logger( fmi2Fatal, "ABORT", "unable to create internal variable 'stop_time_defined'" );
		return fmi2Fatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "enforce_step", enforceTimeStep_ ) ) {
		ipcLogger_->logger( fmi2Fatal, "ABORT", "unable to create internal variable 'enforce_step'" );
		return fmi2Fatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "reject_step", rejectStep_ ) ) {
		ipcLogger_->logger( fmi2Fatal, "ABORT", "unable to create internal variable 'reject_step'" );
		return fmi2Fatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "slave_has_terminated", slaveHasTerminated_ ) ) {
		ipcLogger_->logger( fmi2Fatal, "ABORT", "unable to create internal variable 'slave_has_terminated'" );
		return fmi2Fatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "fmu_type", fmuType_ ) ) {
		ipcLogger_->logger( fmi2Fatal, "ABORT", "unable to create internal variable 'fmu_type'" );
		return fmi2Fatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "logging_on", loggingOn_ ) ) {
		ipcLogger_->logger( fmi2Fatal, "ABORT", "unable to create internal variable 'logging_on'" );
		return fmi2Fatal;
	}

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "FMI component backend initialized successfully." );

	return fmi2OK;
}


///
/// End initialization of the backend (connect/sync with master).
///
fmi2Status
FMIComponentBackEnd::endInitialization()
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function endInitialization" );

	ipcSlave_->signalToMaster(); /// \FIXME is there a way to check whether everthing went fine?

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "endInitialization done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeRealParameters( const std::vector<std::string>& names, std::vector<fmi2Real*>& params )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "Invalid FMU type." );
			return fmi2Fatal;
	}

	fmi2Status status = initializeVariables( realParameters_, "real_scalars", names, causality );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( params.size() != realParameters_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::const_iterator itParameter = params.begin();
	vector<fmi2Real*>::iterator itCopy = realParameters_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeRealParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeRealParameters( const std::string* names, fmi2Real* params, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "Invalid FMU type." );
			return fmi2Fatal;
	}

	fmi2Status status = initializeVariables( realParameters_, "real_scalars", names, n, causality );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != realParameters_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itCopy = realParameters_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++params ) *params = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeRealParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeIntegerParameters( const std::vector<std::string>& names, std::vector<fmi2Integer*>& params )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerParameters" );

		ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "Invalid FMU type." );
			return fmi2Fatal;
	}

	fmi2Status status = initializeVariables( integerParameters_, "integer_scalars", names, causality );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( params.size() != integerParameters_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::const_iterator itParameter = params.begin();
	vector<fmi2Integer*>::iterator itCopy = integerParameters_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeIntegerParameters done" );

	return fmi2OK;

}


fmi2Status
FMIComponentBackEnd::initializeIntegerParameters( const std::string* names, fmi2Integer* params, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "Invalid FMU type." );
			return fmi2Fatal;
	}

	fmi2Status status = initializeVariables( integerParameters_, "integer_scalars", names, n, causality );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != integerParameters_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itCopy = integerParameters_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++params ) *params = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeIntegerParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeBooleanParameters( const std::vector<std::string>& names, std::vector<fmi2Boolean*>& params )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "Invalid FMU type." );
			return fmi2Fatal;
	}

	fmi2Status status = initializeVariables( booleanParameters_, "boolean_scalars", names, causality );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( params.size() != booleanParameters_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::const_iterator itParameter = params.begin();
	vector<fmi2Boolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeBooleanParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeBooleanParameters( const std::string* names, fmi2Boolean* params, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "Invalid FMU type." );
			return fmi2Fatal;
	}

	fmi2Status status = initializeVariables( booleanParameters_, "boolean_scalars", names, n, causality );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != booleanParameters_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++params ) *params = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeBooleanParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeStringParameters( const std::vector<std::string>& names, std::vector<std::string*>& params )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "Invalid FMU type." );
			return fmi2Fatal;
	}

	fmi2Status status = initializeVariables( stringParameters_, "string_scalars", names, causality );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( params.size() != stringParameters_.size() ) return fmi2Fatal;

	vector<std::string*>::const_iterator itParameter = params.begin();
	vector<std::string*>::iterator itCopy = stringParameters_.begin();
	vector<std::string*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeStringParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeStringParameters( const std::string* names, std::string* params, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringParameters" );

	ScalarVariableAttributes::Causality::Causality causality;
	switch ( getFMUType() ) {
		case fmi_1_0_cs:
			causality = ScalarVariableAttributes::Causality::internal;
			break;
		case fmi_2_0_cs:
			causality = ScalarVariableAttributes::Causality::parameter;
			break;
		default:
			if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "Invalid FMU type." );
			return fmi2Fatal;
	}

	fmi2Status status = initializeVariables( stringParameters_, "string_scalars", names, n, causality );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != stringParameters_.size() ) return fmi2Fatal;

	vector<std::string*>::iterator itCopy = stringParameters_.begin();
	vector<std::string*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++params ) *params = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeStringParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeRealInputs( const vector<string>& names, std::vector<fmi2Real*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealInputs" );

	fmi2Status status = initializeVariables( realInputs_, "real_scalars", names, ScalarVariableAttributes::Causality::input );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( inputs.size() != realInputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::const_iterator itInput = inputs.begin();
	vector<fmi2Real*>::iterator itCopy = realInputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeRealInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeRealInputs( const std::string* names, fmi2Real* inputs, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealInputs" );

	fmi2Status status = initializeVariables( realInputs_, "real_scalars", names, n, ScalarVariableAttributes::Causality::input );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != realInputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itCopy = realInputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeRealInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeIntegerInputs( const vector<string>& names, std::vector<fmi2Integer*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerInputs" );

	fmi2Status status = initializeVariables( integerInputs_, "integer_scalars", names, ScalarVariableAttributes::Causality::input );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( inputs.size() != integerInputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::const_iterator itInput = inputs.begin();
	vector<fmi2Integer*>::iterator itCopy = integerInputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeIntegerInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeIntegerInputs( const std::string* names, fmi2Integer* inputs, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerInputs" );

	fmi2Status status = initializeVariables( integerInputs_, "integer_scalars", names, n, ScalarVariableAttributes::Causality::input );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != integerInputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itCopy = integerInputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeIntegerInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeBooleanInputs( const vector<string>& names, std::vector<fmi2Boolean*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanInputs" );

	fmi2Status status = initializeVariables( booleanInputs_, "boolean_scalars", names, ScalarVariableAttributes::Causality::input );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( inputs.size() != booleanInputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::const_iterator itInput = inputs.begin();
	vector<fmi2Boolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeBooleanInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeBooleanInputs( const std::string* names, fmi2Boolean* inputs, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanInputs" );

	fmi2Status status = initializeVariables( booleanInputs_, "boolean_scalars", names, n, ScalarVariableAttributes::Causality::input );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != booleanInputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeBooleanInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeStringInputs( const vector<string>& names, std::vector<std::string*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringInputs" );

	fmi2Status status = initializeVariables( stringInputs_, "string_scalars", names, ScalarVariableAttributes::Causality::input );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( inputs.size() != stringInputs_.size() ) return fmi2Fatal;

	vector<std::string*>::const_iterator itInput = inputs.begin();
	vector<std::string*>::iterator itCopy = stringInputs_.begin();
	vector<std::string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeStringInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeStringInputs( const std::string* names, std::string* inputs, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringInputs" );

	fmi2Status status = initializeVariables( stringInputs_, "string_scalars", names, n, ScalarVariableAttributes::Causality::input );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != stringInputs_.size() ) return fmi2Fatal;

	vector<std::string*>::iterator itCopy = stringInputs_.begin();
	vector<std::string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeStringInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeRealOutputs( const vector<string>& names, vector<fmi2Real*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealOutputs" );

	fmi2Status status = initializeVariables( realOutputs_, "real_scalars", names, ScalarVariableAttributes::Causality::output );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( outputs.size() != realOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::const_iterator itOutput = outputs.begin();
	vector<fmi2Real*>::iterator itCopy = realOutputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itOutput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeRealOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeRealOutputs( const std::string* names, fmi2Real* outputs, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealOutputs" );

	fmi2Status status = initializeVariables( realOutputs_, "real_scalars", names, n, ScalarVariableAttributes::Causality::output );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != realOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itCopy = realOutputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) *outputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeRealOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeIntegerOutputs( const vector<string>& names, std::vector<fmi2Integer*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerOutputs" );

	fmi2Status status = initializeVariables( integerOutputs_, "integer_scalars", names, ScalarVariableAttributes::Causality::output );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( outputs.size() != integerOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::const_iterator itOutput = outputs.begin();
	vector<fmi2Integer*>::iterator itCopy = integerOutputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itOutput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeIntegerOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeIntegerOutputs( const std::string* names, fmi2Integer* outputs, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerOutputs" );

	fmi2Status status = initializeVariables( integerOutputs_, "integer_scalars", names, n, ScalarVariableAttributes::Causality::output );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != integerOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itCopy = integerOutputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) *outputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeIntegerOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeBooleanOutputs( const vector<string>& names, std::vector<fmi2Boolean*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanOutputs" );

	fmi2Status status = initializeVariables( booleanOutputs_, "boolean_scalars", names, ScalarVariableAttributes::Causality::output );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( outputs.size() != booleanOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::const_iterator itOutput = outputs.begin();
	vector<fmi2Boolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itOutput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeBooleanOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeBooleanOutputs( const std::string* names, fmi2Boolean* outputs, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanOutputs" );

	fmi2Status status = initializeVariables( booleanOutputs_, "boolean_scalars", names, n, ScalarVariableAttributes::Causality::output );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != booleanOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) *outputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeBooleanOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeStringOutputs( const vector<string>& names, std::vector<std::string*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringOutputs" );

	fmi2Status status = initializeVariables( stringOutputs_, "string_scalars", names, ScalarVariableAttributes::Causality::output );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( outputs.size() != stringOutputs_.size() ) return fmi2Fatal;

	vector<std::string*>::const_iterator itOutput = outputs.begin();
	vector<std::string*>::iterator itCopy = stringOutputs_.begin();
	vector<std::string*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itOutput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeStringOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::initializeStringOutputs( const std::string* names, std::string* outputs, const size_t n )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringOutputs" );

	fmi2Status status = initializeVariables( stringOutputs_, "string_scalars", names, n, ScalarVariableAttributes::Causality::output );

	if ( fmi2OK != status ) return fmi2Fatal;

	if ( n != stringOutputs_.size() ) return fmi2Fatal;

	vector<std::string*>::iterator itCopy = stringOutputs_.begin();
	vector<std::string*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) *outputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "initializeStringOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getRealParameters( vector<fmi2Real*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getRealParameters" );

	if ( parameters.size() != realParameters_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itParameter = parameters.begin();
	vector<fmi2Real*>::iterator itCopy = realParameters_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getRealParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getRealParameters( fmi2Real* parameters, size_t nParameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getRealParameters" );

	if ( nParameters != realParameters_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itCopy = realParameters_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) *parameters = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getRealParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getIntegerParameters( vector<fmi2Integer*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getIntegerParameters" );

	if ( parameters.size() != integerParameters_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itParameter = parameters.begin();
	vector<fmi2Integer*>::iterator itCopy = integerParameters_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getIntegerParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getIntegerParameters( fmi2Integer* parameters, size_t nParameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getIntegerParameters" );

	if ( nParameters != integerParameters_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itCopy = integerParameters_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) *parameters = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getIntegerParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getBooleanParameters( vector<fmi2Boolean*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getBooleanParameters" );

	if ( parameters.size() != booleanParameters_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itParameter = parameters.begin();
	vector<fmi2Boolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getBooleanParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getBooleanParameters( fmi2Boolean* parameters, size_t nParameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getBooleanParameters" );

	if ( nParameters != booleanParameters_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) *parameters = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getBooleanParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getStringParameters( vector<string*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getStringParameters" );

	if ( parameters.size() != stringParameters_.size() ) return fmi2Fatal;

	vector<string*>::iterator itParameter = parameters.begin();
	vector<string*>::iterator itCopy = stringParameters_.begin();
	vector<string*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itParameter = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getStringParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getStringParameters( string* parameters, size_t nParameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getStringParameters" );

	if ( nParameters != stringParameters_.size() ) return fmi2Fatal;

	vector<string*>::iterator itCopy = stringParameters_.begin();
	vector<string*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) *parameters = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getStringParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setRealParameters( const vector<fmi2Real*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setRealParameters" );

	if ( parameters.size() != realParameters_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::const_iterator itParameter = parameters.begin();
	vector<fmi2Real*>::iterator itCopy = realParameters_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itCopy = **itParameter;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setRealParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setRealParameters( const fmi2Real* parameters, size_t nParameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setRealParameters" );

	if ( nParameters != realParameters_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itCopy = realParameters_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) **itCopy = *parameters;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setRealParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setIntegerParameters( const vector<fmi2Integer*>& parameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setIntegerParameters" );

	if ( parameters.size() != integerParameters_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::const_iterator itParameter = parameters.begin();
	vector<fmi2Integer*>::iterator itCopy = integerParameters_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itCopy = **itParameter;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setIntegerParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setIntegerParameters( const fmi2Integer* parameters, size_t nParameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setIntegerParameters" );

	if ( nParameters != integerParameters_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itCopy = integerParameters_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++parameters ) **itCopy = *parameters;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setIntegerParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setBooleanParameters( const vector<fmi2Boolean*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setBooleanParameters" );

	if ( outputs.size() != booleanParameters_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::const_iterator itParameter = outputs.begin();
	vector<fmi2Boolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itCopy = **itParameter;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setBooleanParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setBooleanParameters( const fmi2Boolean* outputs, size_t nParameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setBooleanParameters" );

	if ( nParameters != booleanParameters_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itCopy = booleanParameters_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setBooleanParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setStringParameters( const vector<string*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setStringParameters" );

	if ( outputs.size() != stringParameters_.size() ) return fmi2Fatal;

	vector<string*>::const_iterator itParameter = outputs.begin();
	vector<string*>::iterator itCopy = stringParameters_.begin();
	vector<string*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itParameter ) **itCopy = **itParameter;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setStringParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setStringParameters( const string* outputs, size_t nParameters )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setStringParameters" );

	if ( nParameters != stringParameters_.size() ) return fmi2Fatal;

	vector<string*>::iterator itCopy = stringParameters_.begin();
	vector<string*>::iterator itCopyEnd = stringParameters_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setStringParameters done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getRealInputs( vector<fmi2Real*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getRealInputs" );

	if ( inputs.size() != realInputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itInput = inputs.begin();
	vector<fmi2Real*>::iterator itCopy = realInputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getRealInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getRealInputs( fmi2Real* inputs, size_t nInputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getRealInputs" );

	if ( nInputs != realInputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itCopy = realInputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getRealInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getIntegerInputs( vector<fmi2Integer*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getIntegerInputs" );

	if ( inputs.size() != integerInputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itInput = inputs.begin();
	vector<fmi2Integer*>::iterator itCopy = integerInputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getIntegerInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getIntegerInputs( fmi2Integer* inputs, size_t nInputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getIntegerInputs" );

	if ( nInputs != integerInputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itCopy = integerInputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getIntegerInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getBooleanInputs( vector<fmi2Boolean*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getBooleanInputs" );

	if ( inputs.size() != booleanInputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itInput = inputs.begin();
	vector<fmi2Boolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getBooleanInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getBooleanInputs( fmi2Boolean* inputs, size_t nInputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getBooleanInputs" );

	if ( nInputs != booleanInputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getBooleanInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getStringInputs( vector<string*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getStringInputs" );

	if ( inputs.size() != stringInputs_.size() ) return fmi2Fatal;

	vector<string*>::iterator itInput = inputs.begin();
	vector<string*>::iterator itCopy = stringInputs_.begin();
	vector<string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getStringInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::getStringInputs( string* inputs, size_t nInputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function getStringInputs" );

	if ( nInputs != stringInputs_.size() ) return fmi2Fatal;

	vector<string*>::iterator itCopy = stringInputs_.begin();
	vector<string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "getStringInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::resetRealInputs( std::vector<fmi2Real*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function resetRealInputs" );

	if ( inputs.size() != realInputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::const_iterator itInput = inputs.begin();
	vector<fmi2Real*>::iterator itCopy = realInputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itCopy = **itInput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "resetRealInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::resetRealInputs( fmi2Real* inputs, size_t nInputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function resetRealInputs" );

	if ( nInputs != realInputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itCopy = realInputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) **itCopy = *inputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "resetRealInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::resetIntegerInputs( std::vector<fmi2Integer*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function resetIntegerInputs" );

	if ( inputs.size() != integerInputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::const_iterator itInput = inputs.begin();
	vector<fmi2Integer*>::iterator itCopy = integerInputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itCopy = **itInput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "resetIntegerInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::resetIntegerInputs( fmi2Integer* inputs, size_t nInputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function resetIntegerInputs" );

	if ( nInputs != integerInputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itCopy = integerInputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) **itCopy = *inputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "resetIntegerInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::resetBooleanInputs( std::vector<fmi2Boolean*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function resetBooleanInputs" );

	if ( inputs.size() != booleanInputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::const_iterator itInput = inputs.begin();
	vector<fmi2Boolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itCopy = **itInput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "resetBooleanInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::resetBooleanInputs( fmi2Boolean* inputs, size_t nInputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function resetBooleanInputs" );

	if ( nInputs != booleanInputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) **itCopy = *inputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "resetBooleanInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::resetStringInputs( std::vector<std::string*>& inputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function resetStringInputs" );

	if ( inputs.size() != stringInputs_.size() ) return fmi2Fatal;

	vector<std::string*>::const_iterator itInput = inputs.begin();
	vector<std::string*>::iterator itCopy = stringInputs_.begin();
	vector<std::string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itCopy = **itInput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "resetStringInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::resetStringInputs( std::string* inputs, size_t nInputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function resetStringInputs" );

	if ( nInputs != stringInputs_.size() ) return fmi2Fatal;

	vector<std::string*>::iterator itCopy = stringInputs_.begin();
	vector<std::string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) **itCopy = *inputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "resetStringInputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setRealOutputs( const vector<fmi2Real*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setRealOutputs" );

	if ( outputs.size() != realOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::const_iterator itOutput = outputs.begin();
	vector<fmi2Real*>::iterator itCopy = realOutputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setRealOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setRealOutputs( const fmi2Real* outputs, size_t nOutputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setRealOutputs" );

	if ( nOutputs != realOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Real*>::iterator itCopy = realOutputs_.begin();
	vector<fmi2Real*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setRealOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setIntegerOutputs( const vector<fmi2Integer*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setIntegerOutputs" );

	if ( outputs.size() != integerOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::const_iterator itOutput = outputs.begin();
	vector<fmi2Integer*>::iterator itCopy = integerOutputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setIntegerOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setIntegerOutputs( const fmi2Integer* outputs, size_t nOutputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setIntegerOutputs" );

	if ( nOutputs != integerOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Integer*>::iterator itCopy = integerOutputs_.begin();
	vector<fmi2Integer*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setIntegerOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setBooleanOutputs( const vector<fmi2Boolean*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setBooleanOutputs" );

	if ( outputs.size() != booleanOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::const_iterator itOutput = outputs.begin();
	vector<fmi2Boolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setBooleanOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setBooleanOutputs( const fmi2Boolean* outputs, size_t nOutputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setBooleanOutputs" );

	if ( nOutputs != booleanOutputs_.size() ) return fmi2Fatal;

	vector<fmi2Boolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmi2Boolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setBooleanOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setStringOutputs( const vector<string*>& outputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setStringOutputs" );

	if ( outputs.size() != stringOutputs_.size() ) return fmi2Fatal;

	vector<string*>::const_iterator itOutput = outputs.begin();
	vector<string*>::iterator itCopy = stringOutputs_.begin();
	vector<string*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setStringOutputs done" );

	return fmi2OK;
}


fmi2Status
FMIComponentBackEnd::setStringOutputs( const string* outputs, size_t nOutputs )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function setStringOutputs" );

	if ( nOutputs != stringOutputs_.size() ) return fmi2Fatal;

	vector<string*>::iterator itCopy = stringOutputs_.begin();
	vector<string*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "setStringOutputs done" );

	return fmi2OK;
}


///
/// Wait for signal from master to resume execution.
/// Blocks until signal from master is received.
///
void
FMIComponentBackEnd::waitForMaster() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function waitForMaster" );

	ipcSlave_->waitForMaster();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "waitForMaster done" );
}


///
/// Send signal to master to proceed with execution.
/// Do not alter shared data until waitForMaster() unblocks.
///
void
FMIComponentBackEnd::signalToMaster() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function signalToMaster" );

	ipcSlave_->signalToMaster();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "signalToMaster done" );
}


///
/// Inform frontend what the next simulation time step will be.
///
void
FMIComponentBackEnd::enforceTimeStep( const fmi2Real& delta )
{
	if ( 0 == ipcSlave_ ) return;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function enforceTimeStep" );

	*enforceTimeStep_ = true;
	*communicationStepSize_ = delta;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "enforceTimeStep done" );
}


///
/// Inform frontend that the simulation step has been rejected.
///
void
FMIComponentBackEnd::rejectStep()
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "entering function rejectStep" );

	*rejectStep_ = true;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "rejectStep done" );
}


const fmi2Real&
FMIComponentBackEnd::getCurrentCommunicationPoint() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function getCurrentCommunicationPoint" );

	return *currentCommunicationPoint_;
}


const fmi2Real&
FMIComponentBackEnd::getCommunicationStepSize() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function getCommunicationStepSize" );

	return *communicationStepSize_;
}


const fmi2Real&
FMIComponentBackEnd::getStopTime() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function getStopTime" );

	return *stopTime_;
}


const bool&
FMIComponentBackEnd::getStopTimeDefined() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function getStopTimeDefined" );

	return *stopTimeDefined_;
}


std::string
FMIComponentBackEnd::getLogFileName() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function getLogFileName" );

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
FMIComponentBackEnd::logger( fmi2Status status, const std::string& category, const std::string& msg )
{
	ipcLogger_->logger( fmi2Warning, category, msg );
}


///
/// Get names of all real inputs initialized by the front end.
///
void
FMIComponentBackEnd::getRealInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "real_scalars", ScalarVariableAttributes::Causality::input );
}


///
/// Get names of all integer inputs initialized by the front end.
///
void
FMIComponentBackEnd::getIntegerInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "integer_scalars", ScalarVariableAttributes::Causality::input );
}


///
/// Get names of all boolean inputs initialized by the front end.
///
void
FMIComponentBackEnd::getBooleanInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "boolean_scalars", ScalarVariableAttributes::Causality::input );
}


///
/// Get names of all string inputs initialized by the front end.
///
void
FMIComponentBackEnd::getStringInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "string_scalars", ScalarVariableAttributes::Causality::input );
}


///
/// Get names of all real outputs initialized by the front end.
///
void
FMIComponentBackEnd::getRealOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "real_scalars", ScalarVariableAttributes::Causality::output );
}


///
/// Get names of all integer outputs initialized by the front end.
///
void
FMIComponentBackEnd::getIntegerOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "integer_scalars", ScalarVariableAttributes::Causality::output );
}


///
/// Get names of all boolean outputs initialized by the front end.
///
void
FMIComponentBackEnd::getBooleanOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "boolean_scalars", ScalarVariableAttributes::Causality::output );
}


///
/// Get names of all string outputs initialized by the front end.
///
void
FMIComponentBackEnd::getStringOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "string_scalars", ScalarVariableAttributes::Causality::output );
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
