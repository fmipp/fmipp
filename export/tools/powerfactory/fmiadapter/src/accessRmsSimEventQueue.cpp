/**
 * \file accessRmsSimEventQueue.cpp
 * Provide access to the sim event queue as C functions.
 *
 * \author Edmund Widl
 */

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "fmiadapter/include/RmsSimEventQueue.h"


extern "C" {

	bool rmsSimEventQueueIsEmpty() { return RmsSimEventQueue::isEmpty(); }

	GetNextEventStatus rmsSimEventQueueGetNextEvent( char* name, size_t lenName,
		char* type, size_t lenType,
		char* target, size_t lenTarget,
		char* event, size_t lenEvent )
	{
		return RmsSimEventQueue::getNextEvent( name, lenName, type, lenType, target, lenTarget, event, lenEvent );
	}

}