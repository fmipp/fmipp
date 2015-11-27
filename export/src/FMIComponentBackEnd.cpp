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
fmiStatus
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
		ipcLogger_->logger( fmiWarning, "WARNING", "IPC interface not operational" );
		ipcSlave_->sleep( 3000 ); /// \FIXME waiting time should be configurable
		ipcLogger_->logger( fmiWarning, "WARNING", "retry to initialize IPC interface" );
		ipcSlave_->reinitialize();
	}

	ipcSlave_->waitForMaster();

	if ( false == ipcSlave_->retrieveVariable( "current_comm_point", currentCommunicationPoint_ ) ) {
		ipcLogger_->logger( fmiFatal, "ABORT", "unable to create internal variable 'current_comm_point'" );
		return fmiFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "comm_step_size", communicationStepSize_ ) ) {
		ipcLogger_->logger( fmiFatal, "ABORT", "unable to create internal variable 'comm_step_size'" );
		return fmiFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "enforce_step", enforceTimeStep_ ) ) {
		ipcLogger_->logger( fmiFatal, "ABORT", "unable to create internal variable 'enforce_step'" );
		return fmiFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "reject_step", rejectStep_ ) ) {
		ipcLogger_->logger( fmiFatal, "ABORT", "unable to create internal variable 'reject_step'" );
		return fmiFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "slave_has_terminated", slaveHasTerminated_ ) ) {
		ipcLogger_->logger( fmiFatal, "ABORT", "unable to create internal variable 'slave_has_terminated'" );
		return fmiFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "logging_on", loggingOn_ ) ) {
		ipcLogger_->logger( fmiFatal, "ABORT", "unable to create internal variable 'logging_on'" );
		return fmiFatal;
	}

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "FMI component backend initialized successfully." );

	return fmiOK;
}


///
/// End initialization of the backend (connect/sync with master).
///
fmiStatus
FMIComponentBackEnd::endInitialization()
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function endInitialization" );

	ipcSlave_->signalToMaster(); /// \FIXME is there a way to check whether everthing went fine?

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "endInitialization done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::initializeRealInputs( const vector<string>& names )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function initializeRealInputs" );

	return initializeVariables( realInputs_, "real_scalars", names, ScalarVariableAttributes::input );
}


fmiStatus
FMIComponentBackEnd::initializeIntegerInputs( const vector<string>& names )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function initializeIntegerInputs" );

	return initializeVariables( integerInputs_, "integer_scalars", names, ScalarVariableAttributes::input );
}


fmiStatus
FMIComponentBackEnd::initializeBooleanInputs( const vector<string>& names )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function initializeBooleanInputs" );

	return initializeVariables( booleanInputs_, "boolean_scalars", names, ScalarVariableAttributes::input );
}


fmiStatus
FMIComponentBackEnd::initializeStringInputs( const vector<string>& names )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function initializeStringInputs" );

	return initializeVariables( stringInputs_, "string_scalars", names, ScalarVariableAttributes::input );
}


fmiStatus
FMIComponentBackEnd::initializeRealOutputs( const vector<string>& names )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function initializeRealOutputs" );

	return initializeVariables( realOutputs_, "real_scalars", names, ScalarVariableAttributes::output );
}


fmiStatus
FMIComponentBackEnd::initializeIntegerOutputs( const vector<string>& names )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function initializeIntegerOutputs" );

	return initializeVariables( integerOutputs_, "integer_scalars", names, ScalarVariableAttributes::output );
}


fmiStatus
FMIComponentBackEnd::initializeBooleanOutputs( const vector<string>& names )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function initializeBooleanOutputs" );

	return initializeVariables( booleanOutputs_, "boolean_scalars", names, ScalarVariableAttributes::output );
}


fmiStatus
FMIComponentBackEnd::initializeStringOutputs( const vector<string>& names )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function initializeStringOutputs" );

	return initializeVariables( stringOutputs_, "string_scalars", names, ScalarVariableAttributes::output );
}


fmiStatus
FMIComponentBackEnd::getRealInputs( vector<fmiReal*>& inputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function getRealInputs" );

	if ( inputs.size() != realInputs_.size() ) return fmiFatal;

	vector<fmiReal*>::iterator itInput = inputs.begin();
	vector<fmiReal*>::iterator itCopy = realInputs_.begin();
	vector<fmiReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "getRealInputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getRealInputs( fmiReal* inputs, size_t nInputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function getRealInputs" );

	if ( nInputs != realInputs_.size() ) return fmiFatal;

	vector<fmiReal*>::iterator itCopy = realInputs_.begin();
	vector<fmiReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "getRealInputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getIntegerInputs( vector<fmiInteger*>& inputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function getIntegerInputs" );

	if ( inputs.size() != integerInputs_.size() ) return fmiFatal;

	vector<fmiInteger*>::iterator itInput = inputs.begin();
	vector<fmiInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmiInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "getIntegerInputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getIntegerInputs( fmiInteger* inputs, size_t nInputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function getIntegerInputs" );

	if ( nInputs != integerInputs_.size() ) return fmiFatal;

	vector<fmiInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmiInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "getIntegerInputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getBooleanInputs( vector<fmiBoolean*>& inputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function getBooleanInputs" );

	if ( inputs.size() != booleanInputs_.size() ) return fmiFatal;

	vector<fmiBoolean*>::iterator itInput = inputs.begin();
	vector<fmiBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmiBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "getBooleanInputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getBooleanInputs( fmiBoolean* inputs, size_t nInputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function getBooleanInputs" );

	if ( nInputs != booleanInputs_.size() ) return fmiFatal;

	vector<fmiBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmiBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "getBooleanInputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getStringInputs( vector<string*>& inputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function getStringInputs" );

	if ( inputs.size() != stringInputs_.size() ) return fmiFatal;

	vector<string*>::iterator itInput = inputs.begin();
	vector<string*>::iterator itCopy = stringInputs_.begin();
	vector<string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "getStringInputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getStringInputs( string* inputs, size_t nInputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function getStringInputs" );

	if ( nInputs != stringInputs_.size() ) return fmiFatal;

	vector<string*>::iterator itCopy = stringInputs_.begin();
	vector<string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "getStringInputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setRealOutputs( const vector<fmiReal*>& outputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function setRealOutputs" );

	if ( outputs.size() != realOutputs_.size() ) return fmiFatal;

	vector<fmiReal*>::const_iterator itOutput = outputs.begin();
	vector<fmiReal*>::iterator itCopy = realOutputs_.begin();
	vector<fmiReal*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "setRealOutputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setRealOutputs( const fmiReal* outputs, size_t nOutputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function setRealOutputs" );

	if ( nOutputs != realOutputs_.size() ) return fmiFatal;

	vector<fmiReal*>::iterator itCopy = realOutputs_.begin();
	vector<fmiReal*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "setRealOutputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setIntegerOutputs( const vector<fmiInteger*>& outputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function setIntegerOutputs" );

	if ( outputs.size() != integerOutputs_.size() ) return fmiFatal;

	vector<fmiInteger*>::const_iterator itOutput = outputs.begin();
	vector<fmiInteger*>::iterator itCopy = integerOutputs_.begin();
	vector<fmiInteger*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "setIntegerOutputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setIntegerOutputs( const fmiInteger* outputs, size_t nOutputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function setIntegerOutputs" );

	if ( nOutputs != integerOutputs_.size() ) return fmiFatal;

	vector<fmiInteger*>::iterator itCopy = integerOutputs_.begin();
	vector<fmiInteger*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "setIntegerOutputs done" );

	return fmiOK;
}

	
fmiStatus
FMIComponentBackEnd::setBooleanOutputs( const vector<fmiBoolean*>& outputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function setBooleanOutputs" );

	if ( outputs.size() != booleanOutputs_.size() ) return fmiFatal;

	vector<fmiBoolean*>::const_iterator itOutput = outputs.begin();
	vector<fmiBoolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmiBoolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "setBooleanOutputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setBooleanOutputs( const fmiBoolean* outputs, size_t nOutputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function setBooleanOutputs" );

	if ( nOutputs != booleanOutputs_.size() ) return fmiFatal;

	vector<fmiBoolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmiBoolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "setBooleanOutputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setStringOutputs( const vector<string*>& outputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function setStringOutputs" );

	if ( outputs.size() != stringOutputs_.size() ) return fmiFatal;

	vector<string*>::const_iterator itOutput = outputs.begin();
	vector<string*>::iterator itCopy = stringOutputs_.begin();
	vector<string*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "setStringOutputs done" );

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setStringOutputs( const string* outputs, size_t nOutputs )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function setStringOutputs" );

	if ( nOutputs != stringOutputs_.size() ) return fmiFatal;

	vector<string*>::iterator itCopy = stringOutputs_.begin();
	vector<string*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "setStringOutputs done" );

	return fmiOK;
}


///
/// Wait for signal from master to resume execution.
/// Blocks until signal from master is received.
///
void
FMIComponentBackEnd::waitForMaster() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function waitForMaster" );

	ipcSlave_->waitForMaster();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "waitForMaster done" );
}


///
/// Send signal to master to proceed with execution.
/// Do not alter shared data until waitForMaster() unblocks.
///
void
FMIComponentBackEnd::signalToMaster() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function signalToMaster" );

	ipcSlave_->signalToMaster();

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "signalToMaster done" );
}


///
/// Inform frontend what the next simulation time step will be.
///
void
FMIComponentBackEnd::enforceTimeStep( const fmiReal& delta )
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function enforceTimeStep" );

	*enforceTimeStep_ = true;
	*communicationStepSize_ = delta;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "enforceTimeStep done" );
}


///
/// Inform frontend that the simulation step has been rejected.
///
void
FMIComponentBackEnd::rejectStep()
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "entering function rejectStep" );

	*rejectStep_ = true;

	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "rejectStep done" );
}


const fmiReal&
FMIComponentBackEnd::getCurrentCommunicationPoint() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function getCurrentCommunicationPoint" );

	return *currentCommunicationPoint_;
}


const fmiReal&
FMIComponentBackEnd::getCommunicationStepSize() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function getCommunicationStepSize" );

	return *communicationStepSize_;
}


std::string
FMIComponentBackEnd::getLogFileName() const
{
	if ( true == *loggingOn_ ) ipcLogger_->logger( fmiOK, "DEBUG", "calling function getLogFileName" );

	return ipcLogger_->getLogFileName();
}


///
/// Call the internal logger.
///
void
FMIComponentBackEnd::logger( fmiStatus status, const std::string& category, const std::string& msg )
{
	ipcLogger_->logger( fmiWarning, category, msg );
}


///
/// Get names of all real inputs initialized by the front end.
///
void
FMIComponentBackEnd::getRealInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmiReal>( names, "real_scalars", ScalarVariableAttributes::input );
}


///
/// Get names of all integer inputs initialized by the front end.
///
void
FMIComponentBackEnd::getIntegerInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmiReal>( names, "integer_scalars", ScalarVariableAttributes::input );
}


///
/// Get names of all boolean inputs initialized by the front end.
///
void
FMIComponentBackEnd::getBooleanInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmiReal>( names, "boolean_scalars", ScalarVariableAttributes::input );
}


///
/// Get names of all string inputs initialized by the front end.
///
void
FMIComponentBackEnd::getStringInputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmiReal>( names, "string_scalars", ScalarVariableAttributes::input );
}


///
/// Get names of all real outputs initialized by the front end.
///
void
FMIComponentBackEnd::getRealOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmiReal>( names, "real_scalars", ScalarVariableAttributes::output );
}


///
/// Get names of all integer outputs initialized by the front end.
///
void
FMIComponentBackEnd::getIntegerOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmiReal>( names, "integer_scalars", ScalarVariableAttributes::output );
}


///
/// Get names of all boolean outputs initialized by the front end.
///
void
FMIComponentBackEnd::getBooleanOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmiReal>( names, "boolean_scalars", ScalarVariableAttributes::output );
}


///
/// Get names of all string outputs initialized by the front end.
///
void
FMIComponentBackEnd::getStringOutputNames( std::vector<std::string>& names ) const
{
	getScalarNames<fmiReal>( names, "string_scalars", ScalarVariableAttributes::output );
}
