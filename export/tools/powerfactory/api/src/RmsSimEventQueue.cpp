/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file RmsSimEventQueue.cpp
/// \authors Roman Schwalbe, Matthias Stifter, Edmund Widl

// Boost includes.
#include <queue>
#include <boost/thread.hpp>

// Project includes.
#include "RmsSimEventQueue.h"


using namespace pf_api;


const size_t RmsSimEventQueue::POWERFACTORY_MAX_LEN = 100;


namespace /// \FIXME Could the queue and the mutex be members of class RmsSimEventQueue?
{
	std::queue<std::string> eventQueue_;
	boost::mutex eventQueueMutex_;
}


const size_t
RmsSimEventQueue::getMaxLen()
{
	return POWERFACTORY_MAX_LEN;
}


std::string
RmsSimEventQueue::getNextEvent()
{
	std::string eventString;
	eventQueueMutex_.lock();
	if ( false == eventQueue_.empty() ) {
		eventString = eventQueue_.front();
		eventQueue_.pop();
	}
	eventQueueMutex_.unlock();
	return eventString;
}


void
RmsSimEventQueue::addEvent( const std::string& eventString )
{
	eventQueueMutex_.lock();
	eventQueue_.push( eventString );
	eventQueueMutex_.unlock();
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
