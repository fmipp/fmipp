/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef IPCSlave_H
#define IPCSlave_H

#include <vector>
#include <string>

template<class T> class ScalarVariable;

/**
 * \file IPCSlave.h
 * \class IPCSlave IPCSlave.h
 * Abstract interface class for handling the inter-process communication for FMIComponentBackEnd.
 */


class IPCSlave
{

public:

	virtual ~IPCSlave() {}

	///
	/// Re-initialize the slave.
	///
	virtual void reinitialize() = 0;

	///
	/// Check if data exchange/syncing is working.
	///
	virtual bool isOperational() = 0;

	///
	/// Retrieve pointer to a double data object.
	///
	virtual void retrieveVariable( const std::string& id,
				       double*& var ) const = 0;

	///
	/// Retrieve pointer to an integer data object.
	///
	virtual void retrieveVariable( const std::string& id,
				       int*& var ) const = 0;

	///
	/// Retrieve pointer to a boolean data object.
	///
	virtual void retrieveVariable( const std::string& id,
				       bool*& var ) const = 0;

	///
	/// Retrieve vector of pointers to double scalar variables.
	///
	virtual void retrieveScalars( const std::string& id,
				      std::vector<ScalarVariable<double>*>& vars ) const = 0;

	///
	/// Retrieve vector of pointers to integer scalar variables.
	///
	virtual void retrieveScalars( const std::string& id,
				      std::vector<ScalarVariable<int>*>& vars ) const = 0;

	///
	/// Wait for signal from master to resume execution.
	/// Blocks until signal from slave is received.
	///
	virtual void waitForMaster() = 0;

	///
	/// Send signal to master to proceed with execution.
	/// Do not alter shared data until waitForMaster() unblocks.
	///
	virtual void signalToMaster() = 0;

	///
	/// Stop execution for ms milliseconds.
	///
	virtual void sleep( unsigned int ms ) const = 0;
};


namespace IPCSlaveFactory {

#ifndef _MSC_VER
	///
	/// Helper function to create arbitrary instances of IPC slaves.
	///
	template<typename Type, typename... Params>
	IPCSlave* createIPCSlave( Params... params ) { return new Type( params... ); }

#else // Unfortunatelly, MSVC does not not support variadic templates ...

	///
	/// Helper function to create an instance of IPC slave (no arguments).
	///
	template<typename Type>
	IPCSlave* createIPCSlave() { return new Type(); }

	///
	/// Helper function to create an instance of IPC slave (1 argument).
	///
	template<typename Type, typename Param1>
	IPCSlave* createIPCSlave( Param1 p1 ) { return new Type( p1 ); }

	///
	/// Helper function to create an instance of IPC slave (2 arguments).
	///
	template<typename Type, typename Param1, typename Param2>
	IPCSlave* createIPCSlave( Param1 p1, Param2 p2 ) { return new Type( p1, p2 ); }

	///
	/// Helper function to create an instance of IPC slave (3 arguments).
	///
	template<typename Type, typename Param1, typename Param2, typename Param3>
	IPCSlave* createIPCSlave( Param1 p1, Param2 p2, Param3 p3 ) { return new Type( p1, p2, p3 ); }

#endif


}


#endif // IPCSlave_H
