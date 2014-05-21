/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef IPCMaster_H
#define IPCMaster_H

#include <vector>
#include <string>

template<class T> class ScalarVariable;



/**
 * \file IPCMaster.h
 * \class IPCMaster IPCMaster.h
 * Abstract interface class for handling the inter-process communication for FMIComponentFrontEnd.
 */


class IPCMaster
{

public:

	virtual ~IPCMaster() {}


	///
	/// Re-initialize the master.
	///
	virtual void reinitialize() = 0;

	///
	/// Check if data exchange/syncing is working.
	///
	virtual bool isOperational() = 0;

	///
	/// Create internally a double data object and retrieve pointer to it.
	///
	virtual void createVariable( const std::string& id,
				     double*& var,
				     const double& val = 0. ) = 0;

	///
	/// Create internally an integer data object and retrieve pointer to it.
	///
	virtual void createVariable( const std::string& id,
				     int*& var,
				     const int& val = 0 ) = 0;

	///
	/// Create internally a boolean data object and retrieve pointer to it.
	///
	virtual void createVariable( const std::string& id,
				     bool*& var,
				     const bool& val = false ) = 0;

	///
	/// Create internally double scalar variables and retrieve pointers to it.
	///
	virtual void createScalars( const std::string& id,
				    unsigned int numObj,
				    std::vector<ScalarVariable<double>*>& vars ) = 0;

	///
	/// Create internally integer scalar variables and retrieve pointers to it.
	///
	virtual void createScalars( const std::string& id,
				    unsigned int numObj,
				    std::vector<ScalarVariable<int>*>& vars ) = 0;

	///
	/// Create internally char (fmiBoolean) scalar variables and retrieve pointers to it.
	///
	virtual void createScalars( const std::string& id,
				    unsigned int numObj,
				    std::vector<ScalarVariable<char>*>& vars ) = 0;

	///
	/// Create internally string scalar variables and retrieve pointers to it.
	///
	virtual void createScalars( const std::string& id,
				    unsigned int numObj,
				    std::vector<ScalarVariable<std::string>*>& vars ) = 0;

	///
	/// Wait for signal from slave to resume execution.
	/// Blocks until signal from slave is received.
	///
	virtual void waitForSlave() = 0;

	///
	/// Send signal to slave to proceed with execution.
	/// Do not alter shared data until waitForSlave() unblocks.
	///
	virtual void signalToSlave() = 0;

	///
	/// Stop execution for ms milliseconds.
	///
	virtual void sleep( unsigned int ms ) const = 0;
};




namespace IPCMasterFactory {


#ifndef _MSC_VER
	///
	/// Helper function to create arbitrary instances of IPC masters.
	///
	template<typename Type, typename... Params>
	IPCMaster* createIPCMaster( Params... params ) { return new Type( params... ); }

#else // Unfortunatelly, MSVC does not not support variadic templates ...

	///
	/// Helper function to create an instance of IPC master (no arguments).
	///
	template<typename Type>
	IPCMaster* createIPCMaster() { return new Type(); }

	///
	/// Helper function to create an instance of IPC master (1 argument).
	///
	template<typename Type, typename Param1>
	IPCMaster* createIPCMaster( Param1 p1 ) { return new Type( p1 ); }

	///
	/// Helper function to create an instance of IPC master (2 arguments).
	///
	template<typename Type, typename Param1, typename Param2>
	IPCMaster* createIPCMaster( Param1 p1, Param2 p2 ) { return new Type( p1, p2 ); }

	///
	/// Helper function to create an instance of IPC master (3 arguments).
	///
	template<typename Type, typename Param1, typename Param2, typename Param3>
	IPCMaster* createIPCMaster( Param1 p1, Param2 p2, Param3 p3 ) { return new Type( p1, p2, p3 ); }

#endif

}


#endif // IPCMaster_H
