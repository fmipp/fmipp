/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#include "FMIComponentBackEnd.h"
#include "ScalarVariable.h"
#include "SHMSlave.h"

#ifdef WIN32
#include "Windows.h"
#else
#include <unistd.h>
#endif

#include <iostream>
#include <fstream>

#include <boost/lexical_cast.hpp>

using namespace std;


FMIComponentBackEnd::FMIComponentBackEnd() {}


FMIComponentBackEnd::~FMIComponentBackEnd()
{
	if ( 0 != ipcSlave_ ) {
		// Notify frontend that the backend has been terminated.
		*slaveHasTerminated_ = true;

		ipcSlave_->signalToMaster();
		delete ipcSlave_;
	}
}


///
/// Start initialization of the backend (connect/sync with master).
///
fmiStatus
FMIComponentBackEnd::startInitialization()
{
#ifdef WIN32
	string shmSegmentName = string( "FMI_SEGMENT_PID" ) + boost::lexical_cast<string>( GetCurrentProcessId() );
#else
	string shmSegmentName = string( "FMI_SEGMENT_PID" ) + boost::lexical_cast<string>( getpid() );
#endif
	ipcSlave_ = IPCSlaveFactory::createIPCSlave< SHMSlave >( shmSegmentName );

	while ( false == ipcSlave_->isOperational() ) {
		//cerr << "IPC interface not operational" << endl; fflush(stdout); // FIXME: call logger
		ipcSlave_->sleep( 3000 );
		//cerr << "retry ..." << endl; fflush(stdout); // FIXME: call logger
		ipcSlave_->reinitialize();
		//throw bad_alloc();
	}

	ipcSlave_->waitForMaster();

	ipcSlave_->retrieveVariable( "master_time", masterTime_ );
	ipcSlave_->retrieveVariable( "next_step_size", nextStepSize_ );
	ipcSlave_->retrieveVariable( "enforce_step", enforceTimeStep_ );
	ipcSlave_->retrieveVariable( "reject_step", rejectStep_ );
	ipcSlave_->retrieveVariable( "slave_has_terminated", slaveHasTerminated_ );

	return fmiOK; // FIXME: function shuold check whether everthing went fine ...
}


///
/// End initialization of the backend (connect/sync with master).
///
fmiStatus
FMIComponentBackEnd::endInitialization()
{
	ipcSlave_->signalToMaster();
	return fmiOK; // FIXME: function shuold check whether everthing went fine ...
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
