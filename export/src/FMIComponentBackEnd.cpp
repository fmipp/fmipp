/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file FMIComponentBackEnd.cpp

#ifdef WIN32
#include "Windows.h"
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
	masterTime_( 0 ),
	nextStepSize_( 0 ),
	enforceTimeStep_( 0 ),
	rejectStep_( 0 ),
	slaveHasTerminated_( 0 )
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

	if ( false == ipcSlave_->retrieveVariable( "master_time", masterTime_ ) ) {
		ipcLogger_->logger( fmiFatal, "ABORT", "unable to create internal variable 'master_time'" );
		return fmiFatal;
	}

	if ( false == ipcSlave_->retrieveVariable( "next_step_size", nextStepSize_ ) ) {
		ipcLogger_->logger( fmiFatal, "ABORT", "unable to create internal variable 'next_step_size'" );
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

	return fmiOK;
}


///
/// End initialization of the backend (connect/sync with master).
///
fmiStatus
FMIComponentBackEnd::endInitialization()
{
	ipcSlave_->signalToMaster(); /// \FIXME is there a way to check whether everthing went fine?
	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::initializeRealInputs( const vector<string>& names )
{
	return initializeVariables( realInputs_, "real_scalars", names, ScalarVariableAttributes::input );
}


fmiStatus
FMIComponentBackEnd::initializeIntegerInputs( const vector<string>& names )
{
	return initializeVariables( integerInputs_, "integer_scalars", names, ScalarVariableAttributes::input );
}


fmiStatus
FMIComponentBackEnd::initializeBooleanInputs( const vector<string>& names )
{
	return initializeVariables( booleanInputs_, "boolean_scalars", names, ScalarVariableAttributes::input );
}


fmiStatus
FMIComponentBackEnd::initializeStringInputs( const vector<string>& names )
{
	return initializeVariables( stringInputs_, "string_scalars", names, ScalarVariableAttributes::input );
}


fmiStatus
FMIComponentBackEnd::initializeRealOutputs( const vector<string>& names )
{
	return initializeVariables( realOutputs_, "real_scalars", names, ScalarVariableAttributes::output );
}


fmiStatus
FMIComponentBackEnd::initializeIntegerOutputs( const vector<string>& names )
{
	return initializeVariables( integerOutputs_, "integer_scalars", names, ScalarVariableAttributes::output );
}


fmiStatus
FMIComponentBackEnd::initializeBooleanOutputs( const vector<string>& names )
{
	return initializeVariables( booleanOutputs_, "boolean_scalars", names, ScalarVariableAttributes::output );
}


fmiStatus
FMIComponentBackEnd::initializeStringOutputs( const vector<string>& names )
{
	return initializeVariables( stringOutputs_, "string_scalars", names, ScalarVariableAttributes::output );
}


fmiStatus
FMIComponentBackEnd::getRealInputs( vector<fmiReal*>& inputs )
{
	if ( inputs.size() != realInputs_.size() ) return fmiFatal;

	vector<fmiReal*>::iterator itInput = inputs.begin();
	vector<fmiReal*>::iterator itCopy = realInputs_.begin();
	vector<fmiReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getRealInputs( fmiReal*& inputs, size_t nInputs )
{
	if ( nInputs != realInputs_.size() ) return fmiFatal;

	vector<fmiReal*>::iterator itCopy = realInputs_.begin();
	vector<fmiReal*>::iterator itCopyEnd = realInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getIntegerInputs( vector<fmiInteger*>& inputs )
{
	if ( inputs.size() != integerInputs_.size() ) return fmiFatal;

	vector<fmiInteger*>::iterator itInput = inputs.begin();
	vector<fmiInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmiInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getIntegerInputs( fmiInteger*& inputs, size_t nInputs )
{
	if ( nInputs != integerInputs_.size() ) return fmiFatal;

	vector<fmiInteger*>::iterator itCopy = integerInputs_.begin();
	vector<fmiInteger*>::iterator itCopyEnd = integerInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getBooleanInputs( vector<fmiBoolean*>& inputs )
{
	if ( inputs.size() != booleanInputs_.size() ) return fmiFatal;

	vector<fmiBoolean*>::iterator itInput = inputs.begin();
	vector<fmiBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmiBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getBooleanInputs( fmiBoolean*& inputs, size_t nInputs )
{
	if ( nInputs != booleanInputs_.size() ) return fmiFatal;

	vector<fmiBoolean*>::iterator itCopy = booleanInputs_.begin();
	vector<fmiBoolean*>::iterator itCopyEnd = booleanInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getStringInputs( vector<string*>& inputs )
{
	if ( inputs.size() != stringInputs_.size() ) return fmiFatal;

	vector<string*>::iterator itInput = inputs.begin();
	vector<string*>::iterator itCopy = stringInputs_.begin();
	vector<string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itInput ) **itInput = **itCopy;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::getStringInputs( string*& inputs, size_t nInputs )
{
	if ( nInputs != stringInputs_.size() ) return fmiFatal;

	vector<string*>::iterator itCopy = stringInputs_.begin();
	vector<string*>::iterator itCopyEnd = stringInputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++inputs ) *inputs = **itCopy;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setRealOutputs( const vector<fmiReal*>& outputs )
{
	if ( outputs.size() != realOutputs_.size() ) return fmiFatal;

	vector<fmiReal*>::const_iterator itOutput = outputs.begin();
	vector<fmiReal*>::iterator itCopy = realOutputs_.begin();
	vector<fmiReal*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setRealOutputs( const fmiReal* outputs, size_t nOutputs )
{
	if ( nOutputs != realOutputs_.size() ) return fmiFatal;

	vector<fmiReal*>::iterator itCopy = realOutputs_.begin();
	vector<fmiReal*>::iterator itCopyEnd = realOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setIntegerOutputs( const vector<fmiInteger*>& outputs )
{
	if ( outputs.size() != integerOutputs_.size() ) return fmiFatal;

	vector<fmiInteger*>::const_iterator itOutput = outputs.begin();
	vector<fmiInteger*>::iterator itCopy = integerOutputs_.begin();
	vector<fmiInteger*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setIntegerOutputs( const fmiInteger* outputs, size_t nOutputs )
{
	if ( nOutputs != integerOutputs_.size() ) return fmiFatal;

	vector<fmiInteger*>::iterator itCopy = integerOutputs_.begin();
	vector<fmiInteger*>::iterator itCopyEnd = integerOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	return fmiOK;
}

	
fmiStatus
FMIComponentBackEnd::setBooleanOutputs( const vector<fmiBoolean*>& outputs )
{
	if ( outputs.size() != booleanOutputs_.size() ) return fmiFatal;

	vector<fmiBoolean*>::const_iterator itOutput = outputs.begin();
	vector<fmiBoolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmiBoolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setBooleanOutputs( const fmiBoolean* outputs, size_t nOutputs )
{
	if ( nOutputs != booleanOutputs_.size() ) return fmiFatal;

	vector<fmiBoolean*>::iterator itCopy = booleanOutputs_.begin();
	vector<fmiBoolean*>::iterator itCopyEnd = booleanOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setStringOutputs( const vector<string*>& outputs )
{
	if ( outputs.size() != stringOutputs_.size() ) return fmiFatal;

	vector<string*>::const_iterator itOutput = outputs.begin();
	vector<string*>::iterator itCopy = stringOutputs_.begin();
	vector<string*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++itOutput ) **itCopy = **itOutput;

	return fmiOK;
}


fmiStatus
FMIComponentBackEnd::setStringOutputs( const string* outputs, size_t nOutputs )
{
	if ( nOutputs != stringOutputs_.size() ) return fmiFatal;

	vector<string*>::iterator itCopy = stringOutputs_.begin();
	vector<string*>::iterator itCopyEnd = stringOutputs_.end();
	for ( ; itCopy != itCopyEnd; ++itCopy, ++outputs ) **itCopy = *outputs;

	return fmiOK;
}


///
/// Wait for signal from master to resume execution.
/// Blocks until signal from master is received.
///
void
FMIComponentBackEnd::waitForMaster() const
{
	ipcSlave_->waitForMaster();
}


///
/// Send signal to master to proceed with execution.
/// Do not alter shared data until waitForMaster() unblocks.
///
void
FMIComponentBackEnd::signalToMaster() const
{
	ipcSlave_->signalToMaster();
}


///
/// Inform frontend what the next simulation time step will be.
///
void
FMIComponentBackEnd::enforceTimeStep( const fmiReal& delta )
{
	*enforceTimeStep_ = true;
	*nextStepSize_ = delta;
}


///
/// Inform frontend that the simulation step has been rejected.
///
void
FMIComponentBackEnd::rejectStep()
{
	*rejectStep_ = true;
}


const fmiReal&
FMIComponentBackEnd::getMasterTime() const
{
	return *masterTime_;
}


const fmiReal&
FMIComponentBackEnd::getNextStepSize() const
{
	return *nextStepSize_;
}


///
/// Call the internal logger.
///
void
FMIComponentBackEnd::logger( fmiStatus status, const std::string& category, const std::string& msg )
{
	ipcLogger_->logger( fmiWarning, category, msg );
}
