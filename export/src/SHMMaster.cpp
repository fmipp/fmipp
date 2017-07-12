// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file SHMMaster.cpp

#include <sstream>

#include "export/include/SHMMaster.h"
#include "export/include/SHMManager.h"
#include "export/include/IPCLogger.h"
#include "export/include/ScalarVariable.h"



SHMMaster::SHMMaster( const std::string& shmSegmentId,
		      const long unsigned int& shmSegmentSize,
		      IPCLogger* logger ) :
	IPCMaster( logger ),
	shmSegmentId_( shmSegmentId ),
	shmSegmentSize_( shmSegmentSize ),
	shmManager_( new SHMManager( logger ) )
{
	shmManager_->createSHMSegment( shmSegmentId_, shmSegmentSize_ );
}


SHMMaster::~SHMMaster()
{
	delete shmManager_;
}


// Re-initialize the master.
void
SHMMaster::reinitialize()
{
	shmManager_->createSHMSegment( shmSegmentId_, shmSegmentSize_ );
}


// Check if shared memory data exchange/syncing is working.
bool
SHMMaster::isOperational()
{
	return shmManager_->isOperational();
}


// Create internally a double data object and retrieve pointer to it.
bool
SHMMaster::createVariable( const std::string& id,
			   double*& var,
			   const double& val )
{
	logger( fmi2OK, "DEBUG", "create variable of type 'double'" );
	return shmManager_->createObject( id, var, val );
}


// Create internally an integer data object and retrieve pointer to it.
bool
SHMMaster::createVariable( const std::string& id,
			   int*& var,
			   const int& val )
{
	logger( fmi2OK, "DEBUG", "create variable of type 'int'" );
	return shmManager_->createObject( id, var, val );
}


// Create internally a boolean data object and retrieve pointer to it.
bool
SHMMaster::createVariable( const std::string& id,
			   bool*& var,
			   const bool& val )
{
	logger( fmi2OK, "DEBUG", "create variable of type 'bool'" );
	return shmManager_->createObject( id, var, val );
}


// Create internally double scalar variables and retrieve pointers to it.
bool
SHMMaster::createScalars( const std::string& id,
			  unsigned int numObj,
			  std::vector<ScalarVariable<double>*>& vars )
{
	if ( 0 == numObj ) { vars.clear(); return true; }

	std::stringstream info;
	info << "create vector containing " << numObj << " object(s) of type 'double'";
	logger( fmi2OK, "DEBUG", info.str() );
	return shmManager_->createVector( id, numObj, vars );
}


// Create internally integer scalar variables and retrieve pointers to it.
bool
SHMMaster::createScalars( const std::string& id,
			  unsigned int numObj,
			  std::vector<ScalarVariable<int>*>& vars )
{
	if ( 0 == numObj ) { vars.clear(); return true; }

	std::stringstream info;
	info << "create vector containing " << numObj << " object(s) of type 'int'";
	logger( fmi2OK, "DEBUG", info.str() );
	return shmManager_->createVector( id, numObj, vars );
}


// Create internally char (fmi2Boolean) scalar variables and retrieve pointers to it.
bool
SHMMaster::createScalars( const std::string& id,
			  unsigned int numObj,
			  std::vector<ScalarVariable<char>*>& vars )
{
	if ( 0 == numObj ) { vars.clear(); return true; }

	std::stringstream info;
	info << "create vector containing " << numObj << " object(s) of type 'char' (fmi2Boolean)";
	logger( fmi2OK, "DEBUG", info.str() );
	return shmManager_->createVector( id, numObj, vars );
}


// Create internally string scalar variables and retrieve pointers to it.
bool
SHMMaster::createScalars( const std::string& id,
			  unsigned int numObj,
			  std::vector<ScalarVariable<std::string>*>& vars )
{
	if ( 0 == numObj ) { vars.clear(); return true; }

	std::stringstream info;
	info << "create vector containing " << numObj << " object(s) of type 'std::string'";
	logger( fmi2OK, "DEBUG", info.str() );
	return shmManager_->createVector( id, numObj, vars );
}


// Wait for signal from slave to resume execution.
// Blocks until signal from slave is received.
void
SHMMaster::waitForSlave()
{
	shmManager_->masterWaitForSlave();
}


// Send signal to slave to proceed with execution.
// Do not alter shared data until waitForSlave() unblocks.
void
SHMMaster::signalToSlave()
{
	shmManager_->masterSignalToSlave();
}


// Stop execution for ms milliseconds.
void
SHMMaster::sleep( unsigned int ms ) const
{
	shmManager_->sleep( ms );
}
