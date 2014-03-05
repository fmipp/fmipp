/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef SHMManager_H
#define SHMManager_H

#include <vector>
//#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>


class SHMManager
{

public:

	///
	/// Default constructor: Do nothing.
	///
	SHMManager();

	///
	/// Constructor: Create new shared memory segment.
	///
	SHMManager( const std::string& segmentId,
		    const long unsigned int segmentSize );

	///
	/// Constructor: Open existing shared memory segment.
	///
	SHMManager( const std::string& segmentId );

	~SHMManager();

	///
	/// Wait for signal from slave to resume execution, to be used by master.
	/// Blocks until signal from slave is received.
	///
	void masterWaitForSlave();

	///
	/// Wait for signal from master to resume execution, to be used by slave.
	/// Blocks until signal from master is received.
	///
	void slaveWaitForMaster();

	///
	/// Send signal to slave to proceed with execution, to be used by master.
	/// Do not alter shared data until masterWaitForSlave() unblocks.
	///
	void masterSignalToSlave();

	///
	/// Send signal to master to proceed with execution, to be used by slave.
	/// Do not alter shared data until slaveWaitForMaster() unblocks.
	///
	void slaveSignalToMaster();

#ifndef _MSC_VER
	///
	/// Create a data object in shared memory and retrieve pointer to it.
	///
	template<typename Type, typename... Params>
	void createObject( const std::string& id,
			   Type* &object,
			   Params... params );

	///
	/// Create data objects in shared memory and retrieve vector of pointers to it.
	///
	template<typename Type, typename... Params>
	void createVector( const std::string& id,
			   unsigned int numObj,
			   std::vector<Type*> &vector,
			   Params... params );

#else // Unfortunatelly, MSVC does not not support variadic templates ...

	///
	/// Create a data object in shared memory and retrieve pointer to it.
	///
	template<typename Type, typename Param1>
	void createObject( const std::string& id,
			   Type* &object,
			   Param1 p1 );

	///
	/// Create data objects in shared memory and retrieve vector of pointers to it.
	///
	template<typename Type>
	void createVector( const std::string& id,
			   unsigned int numObj,
			   std::vector<Type*> &vector );
#endif

	///
	/// Retrieve pointer to data object in shared memory.
	///
	template<typename Type>
	void retrieveObject( const std::string& id,
			     Type* &object ) const;

	///
	/// Retrieve vector of pointers to data objects in shared memory.
	///
	template<typename Type>
	void retrieveVector( const std::string& id,
			     std::vector<Type*> &vector ) const;

	///
	/// Check if shared memory data exchange/syncing is working.
	///
	bool isOperational() const { return operational_; }

	///
	/// Create new shared memory segment.
	///
	void createSHMSegment( const std::string& segmentId,
			       const long unsigned int segmentSize );

	///
	/// Open existing shared memory segment.
	///
	void openSHMSegment( const std::string& segmentId );

	///
	/// Stop execution for ms milliseconds.
	///
	void sleep( unsigned int ms ) const;

private:

	bool operational_;

	// The ID of the SHM segment has to be stored, in order to
	// call "shared_memory_object::remove" in the destructor.
	std::string segmentId_;

	//boost::interprocess::managed_shared_memory *segment_;
	boost::interprocess::managed_windows_shared_memory *segment_;

	// Semaphores to protect and synchronize access
	boost::interprocess::interprocess_semaphore *semaphoreMaster_;
	boost::interprocess::interprocess_semaphore *semaphoreSlave_;

};


#ifndef _MSC_VER

template<typename Type, typename... Params>
void SHMManager::createObject( const std::string& id,
			       Type* &object,
			       Params... params )
{
	if ( !segment_ ) {
		std::cerr << "[SHMManager::createObject] ERROR: "
			  << "shared memory segment not initialized: " << segmentId_ << std::endl;
		return;
	}

	object = segment_->construct<Type>( id.c_str() )( params... );
}


template<typename Type, typename... Params>
void SHMManager::createVector( const std::string& id,
			       unsigned int numObj,
			       std::vector<Type*> &vector,
			       Params... params )
{
	if ( !segment_ ) {
		std::cerr << "[SHMManager::createVector] ERROR: "
			  << "shared memory segment not initialized: " << segmentId_ << std::endl;
		return;
	}

	if ( false == vector.empty() ) {
		vector.clear();
		std::cerr << "[SHMManager::createVector] WARNING: "
			  << "previous elements of input vector have been erased." << std::endl;
	}

	typedef boost::interprocess::managed_windows_shared_memory::segment_manager SHMManager;
	typedef boost::interprocess::allocator<Type, SHMManager> SHMAllocator;
	typedef boost::interprocess::vector<Type, SHMAllocator> SHMVector;

	const SHMAllocator allocInst( segment_->get_segment_manager() );
	SHMVector *shmVector = segment_->construct<SHMVector>( id.c_str() )( allocInst );

	vector.reserve( numObj );
	shmVector->reserve( numObj );

	for ( unsigned int i = 0; i < numObj; ++i ) {
		shmVector->push_back( Type( params... ) );
		vector.push_back( &shmVector->back() );
	}
}

#else // Unfortunatelly, MSVC does not not support variadic templates ...

template<typename Type, typename Param1>
void SHMManager::createObject( const std::string& id,
			       Type* &object,
			       Param1 p1 )
{
	if ( !segment_ ) {
		std::cerr << "[SHMManager::createObject] ERROR: "
			  << "shared memory segment not initialized: " << segmentId_ << std::endl;
		return;
	}

	object = segment_->construct<Type>( id.c_str() )( p1 );
}


template<typename Type>
void SHMManager::createVector( const std::string& id,
			       unsigned int numObj,
			       std::vector<Type*> &vector )
{
	if ( !segment_ ) {
		std::cerr << "[SHMManager::createVector] ERROR: "
			  << "shared memory segment not initialized: " << segmentId_ << std::endl;
		return;
	}

	if ( false == vector.empty() ) {
		vector.clear();
		std::cerr << "[SHMManager::createVector] WARNING: "
			  << "previous elements of input vector have been erased." << std::endl;
	}

	typedef boost::interprocess::managed_windows_shared_memory::segment_manager SHMManager;
	typedef boost::interprocess::allocator<Type, SHMManager> SHMAllocator;
	typedef boost::interprocess::vector<Type, SHMAllocator> SHMVector;

	const SHMAllocator allocInst( segment_->get_segment_manager() );
	SHMVector *shmVector = segment_->construct<SHMVector>( id.c_str() )( allocInst );

	vector.reserve( numObj );
	shmVector->reserve( numObj );

	for ( unsigned int i = 0; i < numObj; ++i ) {
		shmVector->push_back( Type() );
		vector.push_back( &shmVector->back() );
	}
}

#endif


template<typename Type>
void SHMManager::retrieveObject( const std::string& id,
				 Type* &object ) const
{
	if ( !segment_ ) {
		std::cerr << "[SHMManager::retrieveObject] ERROR: "
			  << "shared memory segment not initialized: " << segmentId_ << std::endl;
		return;
	}

	std::pair<Type*, boost::interprocess::managed_windows_shared_memory::size_type> res;
	res = segment_->find<Type>( id.c_str() );
	object = ( res.second == 1 ) ? res.first : 0;
}


template<typename Type>
void SHMManager::retrieveVector( const std::string& id,
				 std::vector<Type*> &vector ) const
{
	if ( !segment_ ) {
		std::cerr << "[SHMManager::retrieveVector] ERROR: "
			  << "shared memory segment not initialized: " << segmentId_ << std::endl;
		return;
	}

	if ( false == vector.empty() ) {
		vector.clear();
		std::cerr << "[SHMManager::retrieveVector] WARNING: "
			  << "previous elements of input vector have been erased." << std::endl;
	}

	typedef boost::interprocess::managed_windows_shared_memory::segment_manager SHMManager;
	typedef boost::interprocess::allocator<Type, SHMManager> SHMAllocator;
	typedef boost::interprocess::vector<Type, SHMAllocator> SHMVector;

	std::pair<SHMVector*, boost::interprocess::managed_windows_shared_memory::size_type> res;
	res = segment_->find<SHMVector>( id.c_str() );

	if ( res.second == 1 ) {

		SHMVector* shmVector = res.first;
		vector.reserve( shmVector->size() );

		typename SHMVector::iterator it = shmVector->begin();
		typename SHMVector::iterator end = shmVector->end();
		while ( it != end ) {
			vector.push_back( &*it );
			++it;
		}
	}
}


#endif // SHMManager_H
