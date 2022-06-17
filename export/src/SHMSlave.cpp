// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file SHMSlave.cpp

#include "export/include/SHMSlave.h"
#include "export/include/SHMManager.h"
#include "export/include/IPCLogger.h"
#include "export/include/ScalarVariable.h"


SHMSlave::SHMSlave( const std::string& shmSegmentId,
		    IPCLogger* logger ) :
	IPCSlave( logger ),
	shmSegmentId_( shmSegmentId ),
	shmManager_( new SHMManager( logger ) )
{
	shmManager_->openSHMSegment( shmSegmentId_ );
}


SHMSlave::~SHMSlave()
{
	delete shmManager_;
}


// Re-initialize the master data manager.
void
SHMSlave::reinitialize()
{
	shmManager_->openSHMSegment( shmSegmentId_ );
}


// Check if shared memory data exchange/syncing is working.
bool
SHMSlave::isOperational()
{
	return shmManager_->isOperational();
}


// Retrieve pointer to a double data object.
bool
SHMSlave::retrieveVariable( const std::string& id,
	double*& var ) const
{
	return shmManager_->retrieveObject( id, var );
}


// Retrieve pointer to an integer data object.
bool
SHMSlave::retrieveVariable( const std::string& id,
	int*& var ) const
{
	return shmManager_->retrieveObject( id, var );
}


// Retrieve pointer to a boolean data object.
bool
SHMSlave::retrieveVariable( const std::string& id,
	bool*& var ) const
{
	return shmManager_->retrieveObject( id, var );
}


// Retrieve vector of pointers to double scalar variables.
bool
SHMSlave::retrieveScalars( const std::string& id,
	std::vector<ScalarVariable<double>*>& vars ) const
{
	return shmManager_->retrieveVector( id, vars );
}


// Retrieve vector of pointers to integer scalar variables.
bool
SHMSlave::retrieveScalars( const std::string& id,
	std::vector<ScalarVariable<int>*>& vars ) const
{
	return shmManager_->retrieveVector( id, vars );
}


// Retrieve vector of pointers to boolean scalar variables.
bool
SHMSlave::retrieveScalars( const std::string& id,
	std::vector<ScalarVariable<bool>*>& vars ) const
{
	return shmManager_->retrieveVector( id, vars );
}


// Retrieve vector of pointers to string scalar variables.
bool
SHMSlave::retrieveScalars( const std::string& id,
	std::vector<ScalarVariable<IPCString>*>& vars ) const
{
	return shmManager_->retrieveVector( id, vars );
}


// Wait for signal from master to resume execution.
// Blocks until signal from master is received.
void
SHMSlave::waitForMaster()
{
	shmManager_->slaveWaitForMaster();
}


// Send signal to master to proceed with execution.
// Do not alter shared data until waitForMaster() unblocks.
void
SHMSlave::signalToMaster()
{
	shmManager_->slaveSignalToMaster();
}


// Stop execution for ms milliseconds.
void
SHMSlave::sleep( unsigned int ms ) const
{
	shmManager_->sleep( ms );
}
