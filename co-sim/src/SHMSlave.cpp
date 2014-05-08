/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file SHMSlave.cpp

#include "SHMSlave.h"
#include "SHMManager.h"
#include "ScalarVariable.h"


SHMSlave::SHMSlave( const std::string& shmSegmentId ) :
	shmSegmentId_( shmSegmentId ),
	shmManager_( new SHMManager )
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
void
SHMSlave::retrieveVariable( const std::string& id,
			    double*& var ) const
{
	shmManager_->retrieveObject( id, var );
}


// Retrieve pointer to an integer data object.
void
SHMSlave::retrieveVariable( const std::string& id,
			    int*& var ) const
{
	shmManager_->retrieveObject( id, var );
}


// Retrieve pointer to a boolean data object.
void
SHMSlave::retrieveVariable( const std::string& id,
			    bool*& var ) const
{
	shmManager_->retrieveObject( id, var );
}


// Retrieve vector of pointers to double scalar variables.
void
SHMSlave::retrieveScalars( const std::string& id,
			   std::vector<ScalarVariable<double>*>& vars ) const
{
	shmManager_->retrieveVector( id, vars );
}


// Retrieve vector of pointers to integer scalar variables.
void
SHMSlave::retrieveScalars( const std::string& id,
			   std::vector<ScalarVariable<int>*>& vars ) const
{
	shmManager_->retrieveVector( id, vars );
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
