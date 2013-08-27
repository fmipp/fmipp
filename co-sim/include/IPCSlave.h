#ifndef IPCSlave_H
#define IPCSlave_H

#include <vector>
#include <string>

template<class T> class ScalarVariable;



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

	///
	/// Helper function to create arbitrary instances of IPC slaves.
	///
	template<typename Type, typename... Params>
	IPCSlave* createIPCSlave( Params... params ) { return new Type( params... ); }

}


#endif // IPCSlave_H
