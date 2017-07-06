/**
 * \file accessRmsSimEventQueue.cpp
 * Provide access to the sim event queue (digexdyn.dll) as C functions.
 *
 * \author Edmund Widl
 */

#include <string>
#include <vector>
// #include <iostream>

#include <boost/algorithm/string.hpp>

#include "RmsSimEventQueue.h"


extern "C" {

	const size_t rmsSimEventQueueGetMaxLen() { return RmsSimEventQueue::getMaxLen(); }

	bool rmsSimEventQueueIsEmpty() { return RmsSimEventQueue::isEmpty(); }

	size_t rmsSimEventQueueSize() { return RmsSimEventQueue::size(); }
	
	void rmsSimEventQueueAddEvent( const char* eventString ) { RmsSimEventQueue::addEvent( eventString ); }

	bool rmsSimEventQueueGetNextEvent( char* type, char* name, char* target, char* evt )
	{
		// Get next event from event queue.
		std::string strEvent = RmsSimEventQueue::getNextEvent();

		// If the string is empty, then there was actually no event in the queue.
		if ( strEvent.empty() ) return false;

		// One string for each category.
		std::string strType;
		std::string strName;
		std::string strTarget;
		
		// Split event string into sub-strings.
		std::vector<std::string> vecParam;
		boost::split( vecParam, strEvent, boost::is_any_of(" ") );
		
		// Parse individual sub-strings to retrieve event type, target, variable and value.
		std::vector<std::string> strParam;
		std::vector<std::string>::iterator it;
		for ( it = vecParam.begin(); it != vecParam.end(); ++it )
		{
			boost::split( strParam, *it, boost::is_any_of( "=" ) );

			if( strParam.size() < 2 ) return false; // Syntax error.

			if ( strParam[0].compare( "create" ) == 0 ) {
				strType = strParam[1]; // Parse event type.
			} else if ( strParam[0].compare( "name" ) == 0 ) {
				strName = strParam[1]; // Parse event name.
			} else if ( strParam[0].compare( "target" ) == 0 ) {
				strTarget = strParam[1]; // Parse target name.
			}
		}
		
		if ( ( true == strType.empty() ) || ( true == strName.empty() ) || ( true == strTarget.empty() ) )
			return false; // Not all required information received.
		
		strcpy( type, strType.c_str() ); // Copy event type.
		strcpy( name, strName.c_str() ); // Copy event name.
		strcpy( target, strTarget.c_str() ); // Copy target name.
		strcpy( evt, strEvent.c_str() );
		
		// std::cout << "type: >>>" << type << "<<< - target: >>>" << target << "<<< - name: >>>" << name << "<<< - evt: >>>" << evt << "<<<" << std::endl;
		
		return true;
	}

}