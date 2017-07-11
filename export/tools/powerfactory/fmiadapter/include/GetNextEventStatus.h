/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _POWER_FACTORY_GETNEXTEVENTSTATUS_H
#define _POWER_FACTORY_GETNEXTEVENTSTATUS_H


/// Enumeration that specifies the result of a call to function RmsSimEventQueue::getNextEvent.
enum GetNextEventStatus {
	ok = 0,         ///< The next event has been successfully retrieved.
	queueEmpty = 1, ///< No event has been retrieved because the event queue was empty.
	failed = 2      ///< Retrieving the event failed.
};


#endif // _POWER_FACTORY_GETNEXTEVENTSTATUS_H