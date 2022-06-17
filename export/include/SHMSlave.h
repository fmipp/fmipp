// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_SHMSLAVE_H
#define _FMIPP_SHMSLAVE_H

#include "export/include/IPCSlave.h"

class SHMManager;

/**
 * \file SHMSlave.h
 * \class SHMSlave SHMSlave.h
 * Implements proper shared memory access based on class IPCSlave.
 */


class SHMSlave: public IPCSlave
{

public:

	///
	/// Implementation of class IPCSlave using shared memory and semaphores.
	///
	SHMSlave( const std::string& shmSegmentId,
		  IPCLogger* logger );

	virtual ~SHMSlave();

	///
	/// Re-initialize the slave.
	///
	virtual void reinitialize();

	///
	/// Check if shared memory data exchange/syncing is working.
	///
	virtual bool isOperational();

	///
	/// Retrieve pointer to a double data object.
	///
	virtual bool retrieveVariable( const std::string& id,
		double*& var ) const;

	///
	/// Retrieve pointer to a integer data object.
	///
	virtual bool retrieveVariable( const std::string& id,
		int*& var ) const;

	///
	/// Retrieve pointer to a boolean data object.
	///
	virtual bool retrieveVariable( const std::string& id,
		bool*& var ) const;

	///
	/// Retrieve vector of pointers to double scalar variables.
	///
	virtual bool retrieveScalars( const std::string& id,
		std::vector<ScalarVariable<double>*>& vars ) const;

	///
	/// Retrieve vector of pointers to integer scalar variables.
	///
	virtual bool retrieveScalars( const std::string& id,
		std::vector<ScalarVariable<int>*>& vars ) const;

	///
	/// Retrieve vector of pointers to boolean scalar variables.
	///
	virtual bool retrieveScalars( const std::string& id,
		std::vector<ScalarVariable<bool>*>& vars ) const;

	///
	/// Retrieve vector of pointers to string scalar variables.
	///
	virtual bool retrieveScalars( const std::string& id,
		std::vector<ScalarVariable<IPCString>*>& vars ) const;

	///
	/// Wait for signal from master to resume execution.
	/// Blocks until signal from master is received.
	///
	virtual void waitForMaster();

	///
	/// Send signal to master to proceed with execution.
	/// Do not alter shared data until waitForMaster() unblocks.
	///
	virtual void signalToMaster();

	///
	/// Stop execution for ms milliseconds.
	///
	void sleep( unsigned int ms ) const;

private:

	///  Default contructor is private to prevent usage;
	SHMSlave();

	const std::string shmSegmentId_;

	SHMManager* shmManager_;
};


#endif // _FMIPP_SHMSLAVE_H
