#ifndef _POWER_FACTORY_RMSSIMEVENTQUEUE_H
#define _POWER_FACTORY_RMSSIMEVENTQUEUE_H

/**
 * \file RmsSimEventQueue.h 
 *
 * \class RmsSimEventQueue RmsSimEventQueue.h 
 * Event queue for comminucation with PowerFactory at 
 * run-time (for sending events during RMS simulation).
 * Implements a first in first out queue (FIFO).
 *  
 * \authors Roman Schwalbe, Matthias Stifter, Edmund Widl
 */

// Standard includes.
#include <string>

// Boost includes.
#include <queue>
#include <boost/thread.hpp>

// Project includes.
#include "fmiadapter/include/GetNextEventStatus.h"

// FMI++ includes.
#include "common/FMIPPConfig.h"


class __FMI_DLL RmsSimEventQueue
{

public:

	/// Structure used internally by RmsSimEventQueue to store event information.
	struct RmsSimEventInfo
	{
		const std::string name_;
		const std::string type_;
		const std::string target_;
		const std::string event_;

		RmsSimEventInfo( const std::string& name,
			const std::string& type,
			const std::string& target,
			const std::string& event ) :
		name_( name ), type_( type ), target_( target ), event_( event ) {}
	};

	/// Add new event to the queue.
	static void addEvent( const std::string& name,
		const std::string& type,
		const std::string& target,
		const std::string& event );

	/// Retrieve event info from the first entry in the queue.
	static GetNextEventStatus getNextEvent( std::string& name,
		std::string& type,
		std::string& target,
		std::string& event );

	/// Retrieve event info from the first entry in the queue.
	/// In case one or more of the input character arrays is
	/// not long enough to hold the corresponding event info,
	/// the info is not copied and the function returns false.
	static GetNextEventStatus getNextEvent( char* name, size_t lenName,
		char* type, size_t lenType,
		char* target, size_t lenTarget,
		char* event, size_t lenEvent );

	/// Check if event queue is empty.
	static bool isEmpty();

	/// Get size of event queue.
	static size_t size();

private:

	static std::queue<RmsSimEventInfo*> eventQueue_;
	static boost::mutex eventQueueMutex_;

};


#endif // _POWER_FACTORY_RMSSIMEVENTQUEUE_H
