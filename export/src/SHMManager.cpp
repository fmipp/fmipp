// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file SHMManager.cpp

#include <boost/interprocess/shared_memory_object.hpp>

#include "export/include/SHMManager.h"


using namespace boost::interprocess;


SHMManager::SHMManager() :
	logger_( 0 ),
	operational_( false ),
	segmentId_( "" ),
	segment_( 0 ),
	semaphoreMaster_( 0 ),
	semaphoreSlave_( 0 )
{}


SHMManager::SHMManager( IPCLogger* logger ) :
	logger_( logger ),
	operational_( false ),
	segmentId_( "" ),
	segment_( 0 ),
	semaphoreMaster_( 0 ),
	semaphoreSlave_( 0 )
{}


SHMManager::SHMManager( const std::string& segmentId,
			const long unsigned int segmentSize,
			IPCLogger* logger ) :
	logger_( logger ),
	segment_( 0 ),
	semaphoreMaster_( 0 ),
	semaphoreSlave_( 0 )
{
	createSHMSegment( segmentId, segmentSize );
}


SHMManager::SHMManager( const std::string& segmentId,
			IPCLogger* logger ) :
	logger_( logger ),
	segment_( 0 )
{
	openSHMSegment( segmentId );
}


SHMManager::~SHMManager()
{
	if ( segment_ ) {
		shared_memory_object::remove( segmentId_.c_str() );
		delete segment_;
	}
}


void
SHMManager::masterWaitForSlave()
{
	// Wait until next notification.
	if ( semaphoreMaster_ ) semaphoreMaster_->wait();
}


void
SHMManager::slaveWaitForMaster()
{
	// Wait until next notification.
	if ( semaphoreSlave_ ) semaphoreSlave_->wait();
}


void
SHMManager::masterSignalToSlave()
{
	// Done -> send notification.
	if ( semaphoreMaster_ ) semaphoreSlave_->post();
}


void
SHMManager::slaveSignalToMaster()
{
	// Done -> send notification.
	if ( semaphoreSlave_ ) semaphoreMaster_->post();
}


void
SHMManager::createSHMSegment( const std::string& segmentId,
			      const long unsigned int segmentSize )
{
	if ( 0 == segmentSize ) {
		operational_ = false;
		return;
	}

	try {
		if ( segment_ )
		{
			shared_memory_object::remove( segmentId_.c_str() );
			delete segment_;
		}


		// Create new shared memory object.
#ifdef WIN32
		segment_ = new managed_windows_shared_memory( create_only, segmentId.c_str(), segmentSize );
#else
		segment_ = new managed_shared_memory( create_only, segmentId.c_str(), segmentSize );
#endif


		// Store new ID.
		segmentId_ = segmentId;

		// Create semaphores for master-slave synchronization.
		std::string semaphoreMasterName = segmentId_ + "_sem_master";
		semaphoreMaster_ = segment_->construct<interprocess_semaphore>( semaphoreMasterName.c_str() )( 1 );

		std::string semaphoreSlaveName = segmentId_ + "_sem_slave";
		semaphoreSlave_ = segment_->construct<interprocess_semaphore>( semaphoreSlaveName.c_str() )( 0 );

	}
	catch ( interprocess_exception& e )
	{
		std::stringstream err;
		err << "unable to create proper shared memory segment: "
		    << segmentId << std::endl << "ERROR: " << e.what();
		logger_->logger( fmi2Fatal, "ABORT", err.str() );
		operational_ = false;
		segmentId_.clear();

		// All other functions check whether these pointers are NULL or not. Hence, setting them to
		// NULL effectively renders the manager inoperational without any dangerous side effects.
		if ( segment_ ) { delete segment_; segment_ = 0; }
		if ( semaphoreMaster_ ) { delete semaphoreMaster_; semaphoreMaster_ = 0; }
		if ( semaphoreSlave_ ) { delete semaphoreSlave_; semaphoreSlave_ = 0; }
		return;
	}

	// Everything worked out fine, the interface is operational.
	operational_ = true;
}


void
SHMManager::openSHMSegment( const std::string& segmentId )
{
	segmentId_ = segmentId;

	try {
		if ( segment_ )
		{
			//shared_memory_object::remove( segmentId_.c_str() );
			delete segment_;
		}

		// Open existing shared memory.
#ifdef WIN32
		segment_ = new managed_windows_shared_memory( open_only, segmentId_.c_str() );
#else
		segment_ = new managed_shared_memory( open_only, segmentId_.c_str() );
#endif

	} catch ( interprocess_exception& e ) {
		std::stringstream err;
		err << "unable to create shared memory segment: "
		    << segmentId << std::endl << "ERROR: " << e.what();
		logger_->logger( fmi2Fatal, "ABORT", err.str() );
		segment_ = 0;
		semaphoreMaster_ = 0;
		semaphoreSlave_ = 0;
		operational_ = false;
		return;
	}


	// Get semaphore for syncing from shared memory.
	std::string semaphoreName;
#ifdef WIN32
	std::pair<interprocess_semaphore*, managed_windows_shared_memory::size_type> findSemaphore;
#else
	std::pair<interprocess_semaphore*, managed_shared_memory::size_type> findSemaphore;
#endif

	// Slave sempahore.
	semaphoreName = segmentId_ + "_sem_slave";
	findSemaphore = segment_->find<interprocess_semaphore>( semaphoreName.c_str() );
	if ( findSemaphore.second != 1 ) {
		std::stringstream err;
		err << "found " << findSemaphore.second << " semaphores called '"
		    << semaphoreName << "', but expected only 1.";
		logger_->logger( fmi2Fatal, "ABORT", err.str() );

		operational_ = false;
		return;
	}
	//if ( semaphoreSlave_ ) delete semaphoreSlave_;
	semaphoreSlave_ = findSemaphore.first;

	// Master semaphore.
	semaphoreName = segmentId_ + "_sem_master";
	findSemaphore = segment_->find<interprocess_semaphore>( semaphoreName.c_str() );
	if ( findSemaphore.second != 1 ) {
		std::stringstream err;
		err << "found " << findSemaphore.second << " semaphores called '"
		    << semaphoreName << "', but expected only 1.";
		logger_->logger( fmi2Fatal, "ABORT", err.str() );
		operational_ = false;
		return;
	}
	//if ( semaphoreMaster_ ) delete semaphoreMaster_;
	semaphoreMaster_ = findSemaphore.first;

	// Everything worked out fine, the interface is operational.
	operational_ = true;
}


void
SHMManager::sleep( unsigned int ms ) const
{
	ipcdetail::thread_sleep( ms );
}
