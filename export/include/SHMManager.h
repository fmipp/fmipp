// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_SHMMANAGER_H
#define _FMIPP_SHMMANAGER_H

// Standard includes.
#include <sstream>
#include <vector>

// Boost includes.
#ifdef WIN32
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#else
#include <boost/interprocess/managed_shared_memory.hpp>
#endif
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

// Project includes.
#include "export/include/IPCLogger.h"
#include "export/include/IPCString.h"
#include "export/include/ScalarVariable.h"

/**
 * \file SHMManager.h
 * \class SHMManager SHMManager.h
 * Used by classes SHMMaster and SHMSlave to establish proper shared memory access.
 */

class SHMManager
{

public:

	///
	/// Dummy constructor: Do nothing, except init logger.
	///
	SHMManager( IPCLogger* logger );

	///
	/// Constructor: Create new shared memory segment.
	///
	SHMManager( const std::string& segmentId,
		const long unsigned int segmentSize,
		IPCLogger* logger  );

	///
	/// Constructor: Open existing shared memory segment.
	///
	SHMManager( const std::string& segmentId,
		IPCLogger* logger  );

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
	bool createObject( const std::string& id,
		Type* &object,
		Params... params );

	///
	/// Create data objects in shared memory and retrieve vector of pointers to it.
	///
	template<typename Type, typename... Params>
	bool createVector( const std::string& id,
		size_t numObj,
		std::vector<Type*> &vector,
		Params... params );

#else // Unfortunatelly, MSVC does not not support variadic templates ...

	///
	/// Create a data object in shared memory and retrieve pointer to it.
	///
	template<typename Type, typename Param1>
	bool createObject( const std::string& id,
		Type* &object,
		Param1 p1 );

	///
	/// Create data objects in shared memory and retrieve vector of pointers to it.
	///
	template<typename Type>
	bool createVector( const std::string& id,
		size_t numObj,
		std::vector<Type*> &vector );
#endif

	///
	/// Retrieve pointer to data object in shared memory.
	///
	template<typename Type>
	bool retrieveObject( const std::string& id,
		Type* &object ) const;

	///
	/// Retrieve vector of pointers to data objects in shared memory.
	///
	template<typename Type>
	bool retrieveVector( const std::string& id,
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

	/// Default constructor is private to prevent usage.
	SHMManager();

	IPCLogger* logger_;

	bool operational_;

	// The ID of the SHM segment has to be stored, in order to
	// call "shared_memory_object::remove" in the destructor.
	std::string segmentId_;

#ifdef WIN32
	boost::interprocess::managed_windows_shared_memory *segment_;
#else
	boost::interprocess::managed_shared_memory *segment_;
#endif

	// Semaphores to protect and synchronize access
	boost::interprocess::interprocess_semaphore *semaphoreMaster_;
	boost::interprocess::interprocess_semaphore *semaphoreSlave_;

};


template<>
bool SHMManager::createVector( const std::string& id,
	size_t numObj,
	std::vector<ScalarVariable<IPCString>*>& vector );

#ifndef _MSC_VER

template<typename Type, typename... Params>
bool SHMManager::createObject( const std::string& id,
	Type* &object,
	Params... params )
{
	if ( !segment_ ) {
		std::stringstream err;
		err << "shared memory segment not initialized: " << segmentId_;
		logger_->logger( fmippFatal, "ABORT", err.str() );
		return false;
	}

	object = segment_->construct<Type>( id.c_str(), std::nothrow )( params... );
	return ( 0 == object ) ? false : true;
}

template<typename Type, typename... Params>
bool SHMManager::createVector( const std::string& id,
	size_t numObj,
	std::vector<Type*> &vector,
	Params... params )
{
	if ( !segment_ ) {
		std::stringstream err;
		err << "shared memory segment not initialized: " << segmentId_;
		logger_->logger( fmippFatal, "ABORT", err.str() );
		return false;
	}

	if ( false == vector.empty() ) {
		vector.clear();
		logger_->logger( fmippWarning, "WARNING", "previous elements of input vector have been erased" );
	}

#ifdef MINGW
	typedef boost::interprocess::managed_windows_shared_memory::segment_manager SHMSegmentManager;
#else
	typedef boost::interprocess::managed_shared_memory::segment_manager SHMSegmentManager;
#endif // MINGW

	typedef boost::interprocess::allocator<Type, SHMSegmentManager> SHMAllocator;
	typedef boost::interprocess::vector<Type, SHMAllocator> SHMVector;

	const SHMAllocator allocInst( segment_->get_segment_manager() );
	SHMVector *shmVector = segment_->construct<SHMVector>( id.c_str(), std::nothrow )( allocInst );

	if ( 0 == shmVector ) return false;

	try {
		vector.reserve( numObj );
		shmVector->reserve( numObj );

		for ( unsigned int i = 0; i < numObj; ++i ) {
			shmVector->push_back( Type( params... ) );
			vector.push_back( &shmVector->back() );
		}
	} catch(...) {
		return false;
	}

	return true;
}

#else // Unfortunatelly, MSVC does not not support variadic templates ...

template<typename Type, typename Param1>
bool SHMManager::createObject( const std::string& id,
	Type* &object,
	Param1 p1 )
{
	if ( !segment_ ) {
		std::stringstream err;
		err << "shared memory segment not initialized: " << segmentId_;
		logger_->logger( fmippFatal, "ABORT", err.str() );
		return false;
	}

	object = segment_->construct<Type>( id.c_str(), std::nothrow )( p1 );
	return ( 0 == object ) ? false : true;
}

template<typename Type>
bool SHMManager::createVector( const std::string& id,
	size_t numObj,
	std::vector<Type*> &vector )
{
	if ( !segment_ ) {
		std::stringstream err;
		err << "shared memory segment not initialized: " << segmentId_;
		logger_->logger( fmippFatal, "ABORT", err.str() );
		return false;
	}

	if ( false == vector.empty() ) {
		vector.clear();
		logger_->logger( fmippWarning, "WARNING", "previous elements of input vector have been erased" );
	}

	typedef boost::interprocess::managed_windows_shared_memory::segment_manager SHMManager;
	typedef boost::interprocess::allocator<Type, SHMManager> SHMAllocator;
	typedef boost::interprocess::vector<Type, SHMAllocator> SHMVector;

	const SHMAllocator allocInst( segment_->get_segment_manager() );
	SHMVector *shmVector = segment_->construct<SHMVector>( id.c_str(), std::nothrow )( allocInst );

	if ( 0 == shmVector ) return false;

	try {
		vector.reserve( numObj );
		shmVector->reserve( numObj );

		for ( unsigned int i = 0; i < numObj; ++i ) {
			shmVector->push_back( Type() );
			vector.push_back( &shmVector->back() );
		}
	} catch(...) {
		return false;
	}

	return true;
}

#endif // _MSC_VER

template<typename Type>
bool SHMManager::retrieveObject( const std::string& id,
	Type* &object ) const
{
	if ( !segment_ ) {
		std::stringstream err;
		err << "shared memory segment not initialized: " << segmentId_;
		logger_->logger( fmippFatal, "ABORT", err.str() );
		return false;
	}

#ifdef WIN32
	std::pair<Type*, boost::interprocess::managed_windows_shared_memory::size_type> res;
#else
	std::pair<Type*, boost::interprocess::managed_shared_memory::size_type> res;
#endif

	res = segment_->find<Type>( id.c_str() );
	object = ( res.second == 1 ) ? res.first : 0;

	return ( 0 == object ) ? false : true;
}

template<>
bool SHMManager::retrieveVector( const std::string& id,
	std::vector<ScalarVariable<IPCString>*> &vector ) const;

template<typename Type>
bool SHMManager::retrieveVector( const std::string& id,
	std::vector<Type*> &vector ) const
{
	if ( !segment_ ) {
		std::stringstream err;
		err << "shared memory segment not initialized: " << segmentId_;
		logger_->logger( fmippFatal, "ABORT", err.str() );
		return false;
	}

	if ( false == vector.empty() ) {
		vector.clear();
		logger_->logger( fmippWarning, "WARNING", "previous elements of input vector have been erased" );
	}

#ifdef WIN32
	typedef boost::interprocess::managed_windows_shared_memory::segment_manager SHMSegmentManager;
#else
	typedef boost::interprocess::managed_shared_memory::segment_manager SHMSegmentManager;
#endif

	typedef boost::interprocess::allocator<Type, SHMSegmentManager> SHMAllocator;
	typedef boost::interprocess::vector<Type, SHMAllocator> SHMVector;

#ifdef WIN32
	std::pair<SHMVector*, boost::interprocess::managed_windows_shared_memory::size_type> res;
#else
	std::pair<SHMVector*, boost::interprocess::managed_shared_memory::size_type> res;
#endif

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
	} else {
		return false;
	}

	return true;
}


#endif // _FMIPP_SHMMANAGER_H
