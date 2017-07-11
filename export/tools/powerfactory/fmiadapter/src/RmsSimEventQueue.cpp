/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file RmsSimEventQueue.cpp
/// \authors Roman Schwalbe, Matthias Stifter, Edmund Widl

#include "fmiadapter/include/RmsSimEventQueue.h"


std::queue<RmsSimEventQueue::RmsSimEventInfo*> RmsSimEventQueue::eventQueue_ =
	std::queue<RmsSimEventQueue::RmsSimEventInfo*>();

boost::mutex RmsSimEventQueue::eventQueueMutex_ = boost::mutex();


void
RmsSimEventQueue::addEvent( const std::string& name,
	const std::string& type,
	const std::string& target,
	const std::string& event )
{
	// Lock the mutex to avoid concurrent queue access.
	eventQueueMutex_.lock();

	// Add new event information to queue.
	eventQueue_.push( new RmsSimEventInfo( name, type, target, event ) );

	// Unlock mutex.
	eventQueueMutex_.unlock();
}


GetNextEventStatus
RmsSimEventQueue::getNextEvent( std::string& name,
	std::string& type,
	std::string& target,
	std::string& event )
{
	// Lock the mutex to avoid concurrent queue access.
	eventQueueMutex_.lock();

	// Check if there's an event in the queue.
	if ( true == eventQueue_.empty() ) {
		// Unlock the mutex.
		eventQueueMutex_.unlock();
		return GetNextEventStatus::queueEmpty;
	}

	// Retrieve event info.
	RmsSimEventInfo* evt = eventQueue_.front();

	// Copy event information.
	name = evt->name_;
	type = evt->type_;
	target = evt->target_;
	event = evt->event_;

	// Remove event.
	delete evt;
	eventQueue_.pop();

	// Unlock the mutex.
	eventQueueMutex_.unlock();

	return GetNextEventStatus::ok;
}


GetNextEventStatus
RmsSimEventQueue::getNextEvent( char* name, size_t lenName,
		char* type, size_t lenType,
		char* target, size_t lenTarget,
		char* event, size_t lenEvent )
{
	// Lock the mutex to avoid concurrent queue access.
	eventQueueMutex_.lock();

	// Check if there's an event in the queue.
	if ( true == eventQueue_.empty() ) {
		// Unlock the mutex.
		eventQueueMutex_.unlock();
		return GetNextEventStatus::queueEmpty;
	}

	// Retrieve event info.
	RmsSimEventInfo* evt = eventQueue_.front();
	GetNextEventStatus status = GetNextEventStatus::ok;

	// Check capacity of character arrays.
	if ( ( lenName > evt->name_.size() ) &&
		 ( lenType > evt->type_.size() ) &&
		 ( lenTarget > evt->target_.size() ) &&
		 ( lenEvent > evt->event_.size() ) )
	{
		// The character arrays have enough space to hold
		// the complete event information.
		strcpy( name, evt->name_.c_str() );
		strcpy( type, evt->type_.c_str() );
		strcpy( target, evt->target_.c_str() );
		strcpy( event, evt->event_.c_str() );
	}
	else
	{
		// The character arrays have NOT enough space to hold
		// the complete event information. Copy as much as
		// possible but set status to 'failed'.
		strcpy( name, evt->name_.substr( 0, lenName - 1 ).c_str() );
		strcpy( type, evt->type_.substr( 0, lenType - 1 ).c_str() );
		strcpy( target, evt->target_.substr( 0, lenTarget - 1 ).c_str() );
		strcpy( event, evt->event_.substr( 0, lenEvent - 1 ).c_str() );

		status = GetNextEventStatus::failed;
	}

	// Remove event.
	delete evt;
	eventQueue_.pop();

	// Unlock the mutex.
	eventQueueMutex_.unlock();

	return status;
}


bool
RmsSimEventQueue::isEmpty()
{
	return eventQueue_.empty();
}


size_t
RmsSimEventQueue::size()
{
	return eventQueue_.size();
}
