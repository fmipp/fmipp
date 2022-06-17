// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_SHMMASTER_H
#define _FMIPP_SHMMASTER_H


#include "export/include/IPCMaster.h"


class SHMManager;
class IPCLogger;


/**
 * \file SHMMaster.h
 * \class SHMMaster SHMMaster.h
 * Implements proper shared memory access based on class IPCMaster.
 */


class SHMMaster: public IPCMaster
{

public:

	///
	/// Implementation of class IPCMaster using shared memory and semaphores.
	///
	SHMMaster( const std::string& shmSegmentId,
		   const long unsigned int& shmSegmentSize,
		   IPCLogger* logger );

	virtual ~SHMMaster();

	///
	/// Re-initialize the master.
	///
	virtual void reinitialize();

	///
	/// Check if shared memory data exchange/syncing is working.
	///
	virtual bool isOperational();

	///
	/// Create internally a double data object and retrieve pointer to it.
	///
	virtual bool createVariable( const std::string& id,
		double*& var,
		const double& val = 0. );

	///
	/// Create internally an integer data object and retrieve pointer to it.
	///
	virtual bool createVariable( const std::string& id,
		int*& var,
		const int& val = 0 );

	///
	/// Create internally a bool data object and retrieve pointer to it.
	///
	virtual bool createVariable( const std::string& id,
		bool*& var,
		const bool& val = false );

	///
	/// Create internally double scalar variables and retrieve pointers to it.
	///
	virtual bool createScalars( const std::string& id,
		size_t numObj,
		std::vector<ScalarVariable<double>*>& vars );

	///
	/// Create internally integer scalar variables and retrieve pointers to it.
	///
	virtual bool createScalars( const std::string& id,
		size_t numObj,
		std::vector<ScalarVariable<int>*>& vars );

	///
	/// Create internally boolean scalar variables and retrieve pointers to it.
	///
	virtual bool createScalars( const std::string& id,
		size_t numObj,
		std::vector<ScalarVariable<bool>*>& vars );

	///
	/// Create internally string scalar variables and retrieve pointers to it.
	///
	virtual bool createScalars( const std::string& id,
		size_t numObj,
		std::vector<ScalarVariable<IPCString>*>& vars );

	///
	/// Wait for signal from slave to resume execution.
	/// Blocks until signal from slave is received.
	///
	virtual void waitForSlave();

	///
	/// Send signal to slave to proceed with execution.
	/// Do not alter shared data until waitForSlave() unblocks.
	///
	virtual void signalToSlave();

	///
	/// Stop execution for ms milliseconds.
	///
	void sleep( unsigned int ms ) const;

private:

	const std::string shmSegmentId_;
	const long unsigned int shmSegmentSize_;

	SHMManager* shmManager_;

};


#endif // _FMIPP_SHMMASTER_H
