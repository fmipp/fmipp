#ifndef IPCMaster_H
#define IPCMaster_H

#include <vector>
#include <string>

template<class T> class ScalarVariable;



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

	///
	/// Helper function to create arbitrary instances of IPC masters.
	///
	template<typename Type, typename... Params>
	IPCMaster* createIPCMaster( Params... params ) { return new Type( params... ); }

}


#endif // IPCMaster_H
