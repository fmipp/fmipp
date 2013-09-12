/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef SHMSlave_H
#define SHMSlave_H


#include "IPCSlave.h"

class SHMManager;



class SHMSlave: public IPCSlave
{

public:

	///
	/// Implementation of class IPCSlave using shared memory and semaphores.
	///
	SHMSlave( const std::string& shmSegmentId );

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
	virtual void retrieveVariable( const std::string& id,
				       double*& var ) const;

	///
	/// Retrieve pointer to a integer data object.
	///
	virtual void retrieveVariable( const std::string& id,
				       int*& var ) const;

	///
	/// Retrieve pointer to a boolean data object.
	///
	virtual void retrieveVariable( const std::string& id,
				       bool*& var ) const;

	///
	/// Retrieve vector of pointers to double scalar variables.
	///
	virtual void retrieveScalars( const std::string& id,
				      std::vector<ScalarVariable<double>*>& vars ) const;

	///
	/// Retrieve vector of pointers to integer scalar variables.
	///
	virtual void retrieveScalars( const std::string& id,
				      std::vector<ScalarVariable<int>*>& vars ) const;

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

	const std::string shmSegmentId_;

	SHMManager* shmManager_;

};


#endif // SHMSlave_H
