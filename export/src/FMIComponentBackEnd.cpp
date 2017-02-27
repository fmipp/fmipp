/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file FMIComponentBackEnd.cpp

#ifdef WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif

#include <iostream>
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
	
#ifdef WIN32
	string pid = boost::lexical_cast<string>( GetCurrentProcessId() );
#else
	string pid = boost::lexical_cast<string>( getpid() );
#endif

	string shmSegmentName = string( "FMI_SEGMENT_PID" ) + pid;
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
FMIComponentBackEnd::initializeRealParameters( const std::vector<std::string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealParameters" );

	return initializeVariables( realParameters_, "real_scalars", names, ScalarVariableAttributes::internal );
}


fmi2Status
FMIComponentBackEnd::initializeRealParameters( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealParameters" );

	return initializeVariables( realParameters_, "real_scalars", names, nNames, ScalarVariableAttributes::internal );
}


fmi2Status
FMIComponentBackEnd::initializeIntegerParameters( const std::vector<std::string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerParameters" );

	return initializeVariables( integerParameters_, "integer_scalars", names, ScalarVariableAttributes::internal );
}


fmi2Status
FMIComponentBackEnd::initializeIntegerParameters( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerParameters" );

	return initializeVariables( integerParameters_, "integer_scalars", names, nNames, ScalarVariableAttributes::internal );
}


fmi2Status
FMIComponentBackEnd::initializeBooleanParameters( const std::vector<std::string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanParameters" );

	return initializeVariables( booleanParameters_, "boolean_scalars", names, ScalarVariableAttributes::internal );
}


fmi2Status
FMIComponentBackEnd::initializeBooleanParameters( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanParameters" );

	return initializeVariables( booleanParameters_, "boolean_scalars", names, nNames, ScalarVariableAttributes::internal );
}


fmi2Status
FMIComponentBackEnd::initializeStringParameters( const std::vector<std::string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringParameters" );

	return initializeVariables( stringParameters_, "string_scalars", names, ScalarVariableAttributes::internal );
}


fmi2Status
FMIComponentBackEnd::initializeStringParameters( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringParameters" );

	return initializeVariables( stringParameters_, "string_scalars", names, nNames, ScalarVariableAttributes::internal );
}

	
fmi2Status
FMIComponentBackEnd::initializeRealInputs( const vector<string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealInputs" );

	return initializeVariables( realInputs_, "real_scalars", names, ScalarVariableAttributes::input );
}


fmi2Status
FMIComponentBackEnd::initializeRealInputs( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealInputs" );

	return initializeVariables( realInputs_, "real_scalars", names, nNames, ScalarVariableAttributes::input );
}


fmi2Status
FMIComponentBackEnd::initializeIntegerInputs( const vector<string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerInputs" );

	return initializeVariables( integerInputs_, "integer_scalars", names, ScalarVariableAttributes::input );
}


fmi2Status
FMIComponentBackEnd::initializeIntegerInputs( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerInputs" );

	return initializeVariables( integerInputs_, "integer_scalars", names, nNames, ScalarVariableAttributes::input );
}


fmi2Status
FMIComponentBackEnd::initializeBooleanInputs( const vector<string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanInputs" );

	return initializeVariables( booleanInputs_, "boolean_scalars", names, ScalarVariableAttributes::input );
}


fmi2Status
FMIComponentBackEnd::initializeBooleanInputs( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanInputs" );

	return initializeVariables( booleanInputs_, "boolean_scalars", names, nNames, ScalarVariableAttributes::input );
}


fmi2Status
FMIComponentBackEnd::initializeStringInputs( const vector<string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringInputs" );

	return initializeVariables( stringInputs_, "string_scalars", names, ScalarVariableAttributes::input );
}


fmi2Status
FMIComponentBackEnd::initializeStringInputs( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringInputs" );

	return initializeVariables( stringInputs_, "string_scalars", names, nNames, ScalarVariableAttributes::input );
}


fmi2Status
FMIComponentBackEnd::initializeRealOutputs( const vector<string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealOutputs" );

	return initializeVariables( realOutputs_, "real_scalars", names, ScalarVariableAttributes::output );
}


fmi2Status
FMIComponentBackEnd::initializeRealOutputs( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeRealOutputs" );

	return initializeVariables( realOutputs_, "real_scalars", names, nNames, ScalarVariableAttributes::output );
}


fmi2Status
FMIComponentBackEnd::initializeIntegerOutputs( const vector<string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerOutputs" );

	return initializeVariables( integerOutputs_, "integer_scalars", names, ScalarVariableAttributes::output );
}


fmi2Status
FMIComponentBackEnd::initializeIntegerOutputs( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeIntegerOutputs" );

	return initializeVariables( integerOutputs_, "integer_scalars", names, nNames, ScalarVariableAttributes::output );
}


fmi2Status
FMIComponentBackEnd::initializeBooleanOutputs( const vector<string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanOutputs" );

	return initializeVariables( booleanOutputs_, "boolean_scalars", names, ScalarVariableAttributes::output );
}


fmi2Status
FMIComponentBackEnd::initializeBooleanOutputs( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeBooleanOutputs" );

	return initializeVariables( booleanOutputs_, "boolean_scalars", names, nNames, ScalarVariableAttributes::output );
}


fmi2Status
FMIComponentBackEnd::initializeStringOutputs( const vector<string>& names )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringOutputs" );

	return initializeVariables( stringOutputs_, "string_scalars", names, ScalarVariableAttributes::output );
}


fmi2Status
FMIComponentBackEnd::initializeStringOutputs( const std::string* names, const size_t nNames )
{
	if ( 0 == ipcSlave_ ) return fmi2Fatal;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function initializeStringOutputs" );

	return initializeVariables( stringOutputs_, "string_scalars", names, nNames, ScalarVariableAttributes::output );
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


std::string
FMIComponentBackEnd::getLogFileName() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmi2OK, "DEBUG", "calling function getLogFileName" );

	return ipcLogger_->getLogFileName();
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
	getScalarNames<fmi2Real>( names, "real_scalars", ScalarVariableAttributes::input );
}


///
/// Get names of all integer inputs initialized by the front end.
///
void
FMIComponentBackEnd::getIntegerInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "integer_scalars", ScalarVariableAttributes::input );
}


///
/// Get names of all boolean inputs initialized by the front end.
///
void
FMIComponentBackEnd::getBooleanInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "boolean_scalars", ScalarVariableAttributes::input );
}


///
/// Get names of all string inputs initialized by the front end.
///
void
FMIComponentBackEnd::getStringInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "string_scalars", ScalarVariableAttributes::input );
}


///
/// Get names of all real outputs initialized by the front end.
///
void
FMIComponentBackEnd::getRealOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "real_scalars", ScalarVariableAttributes::output );
}


///
/// Get names of all integer outputs initialized by the front end.
///
void
FMIComponentBackEnd::getIntegerOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "integer_scalars", ScalarVariableAttributes::output );
}


///
/// Get names of all boolean outputs initialized by the front end.
///
void
FMIComponentBackEnd::getBooleanOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "boolean_scalars", ScalarVariableAttributes::output );
}


///
/// Get names of all string outputs initialized by the front end.
///
void
FMIComponentBackEnd::getStringOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmi2Real>( names, "string_scalars", ScalarVariableAttributes::output );
}
