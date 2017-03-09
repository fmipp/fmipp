/**
 * \file accessRmsSimEventQueue.cpp
 * Provide access to the sim event queue (digexdyn.dll) as C functions.
 *
 * \author Edmund Widl
 */
 
#include "RmsSimEventQueue.h"
#include <string>

extern "C" {

	const size_t rmsSimEventQueueGetMaxLen() { return pf_api::RmsSimEventQueue::getMaxLen(); }

	const char* rmsSimEventQueueGetNextEvent() { return pf_api::RmsSimEventQueue::getNextEvent().c_str(); }

	void rmsSimEventQueueAddEvent( const char* eventString ) { pf_api::RmsSimEventQueue::addEvent( eventString ); }

	bool rmsSimEventQueueIsEmpty() { return pf_api::RmsSimEventQueue::isEmpty(); }

	size_t rmsSimEventQueueSize() { return pf_api::RmsSimEventQueue::size(); }
}