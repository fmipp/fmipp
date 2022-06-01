// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file CallbackFunctions.cpp
 * Provide default callback functions for FMUs for ME and CS.
 */

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <sstream>
#include <iostream>

#include "import/base/include/CallbackFunctions.h"
#include "import/base/include/LogBuffer.h"


namespace callback {

	// This is a very verbose logger that prints out all messages it receives.
	void verboseLogger( fmiComponent c, fmiString instanceName,
		fmiStatus status, fmiString category, fmiString message, ... )
	{
		static char msgBuffer[1028];
		static int capacity = sizeof( msgBuffer ) - 1;

		va_list ap;
		va_start( ap, message );
		
		int length = vsnprintf( msgBuffer, capacity, message, ap );

		if ( length < 0 ) {
			// Get error message.
			perror( msgBuffer );
		} else {
			// Append line break.
			msgBuffer[length] = '\n';
			msgBuffer[length+1] = 0;
		}

		va_end( ap );

		LogBuffer& logBuffer = LogBuffer::getLogBuffer();

		std::stringstream out;
		out << instanceName << " [" << category << "]: " << msgBuffer;
		
		if ( true == logBuffer.isActivated() ) {
			logBuffer.writeToBuffer( out.str() );
		} else {
			std::cout << out.str();
		}
	}

	// This logger only prints out messages with status fmiDiscard, fmiError, fmiFatal or fmiPending.
	void succinctLogger( fmiComponent c, fmiString instanceName,
		fmiStatus status, fmiString category, fmiString message, ... )
	{
		if ( ( fmiOK == status ) || ( fmiWarning == status ) ) return;

		static char msgBuffer[1028];
		static int capacity = sizeof( msgBuffer ) - 1;

		va_list ap;
		va_start( ap, message );
		
		int length = vsnprintf( msgBuffer, capacity, message, ap );

		if ( length < 0 ) {
			// Get error message.
			perror( msgBuffer );
		} else {
			// Append line break.
			msgBuffer[length] = '\n';
			msgBuffer[length+1] = 0;
		}

		va_end( ap );

		LogBuffer& logBuffer = LogBuffer::getLogBuffer();

		std::stringstream out;
		out << instanceName << " [" << category << "]: " << msgBuffer;
		
		if ( true == logBuffer.isActivated() ) {
			logBuffer.writeToBuffer( out.str() );
		} else {
			std::cout << out.str();
		}
	}

	void* allocateMemory( size_t nobj, size_t size )
	{
		// Use standard function "calloc(...)" as default.
		return std::calloc( nobj, size );
	}

	void freeMemory( void* obj )
	{
		// Use standard function "free(...)" as default.
		std::free( obj );
	}

	void stepFinished( fmiComponent c, fmiStatus status )
	{
		// Do nothing.
	}

} // namespace callback


namespace callback2 {
	// contains copies of the functions from namespace callback, but  with different types
	// (fmi2Boolean instead of fmiBoolean, etc.).

	// This is a very verbose logger that prints out all messages it receives.
	void verboseLogger( fmi2ComponentEnvironment componentEnviroment, fmi2String instanceName,
		fmi2Status status, fmi2String category, fmi2String message, ... )
	{
		static char msgBuffer[1028];
		static int capacity = sizeof( msgBuffer ) - 1;

		va_list ap;
		va_start( ap, message );
		
		int length = vsnprintf( msgBuffer, capacity, message, ap );

		if ( length < 0 ) {
			// Get error message.
			perror( msgBuffer );
		} else {
			// Append line break.
			msgBuffer[length] = '\n';
			msgBuffer[length+1] = 0;
		}

		va_end( ap );

		LogBuffer& logBuffer = LogBuffer::getLogBuffer();

		std::stringstream out;
		out << instanceName << " [" << category << "]: " << msgBuffer;
		
		if ( true == logBuffer.isActivated() ) {
			logBuffer.writeToBuffer( out.str() );
		} else {
			std::cout << out.str();
		}
	}

	// This logger only prints out messages with status fmiDiscard, fmiError, fmiFatal or fmiPending.
	void succinctLogger( fmi2ComponentEnvironment componentEnviroment, fmi2String instanceName,
		fmi2Status status, fmi2String category, fmi2String message, ... )
	{
		if ( ( fmi2OK == status ) || ( fmi2Warning == status ) ) return;

		static char msgBuffer[1028];
		static int capacity = sizeof( msgBuffer ) - 1;

		va_list ap;
		va_start( ap, message );

		int length = vsnprintf( msgBuffer, capacity, message, ap );

		if ( length < 0 ) {
			// Get error message.
			perror( msgBuffer );
		} else {
			// Append line break.
			msgBuffer[length] = '\n';
			msgBuffer[length+1] = 0;
		}

		va_end( ap );

		LogBuffer& logBuffer = LogBuffer::getLogBuffer();

		std::stringstream out;
		out << instanceName << " [" << category << "]: " << msgBuffer;

		if ( true == logBuffer.isActivated() ) {
			logBuffer.writeToBuffer( out.str() );
		} else {
			std::cout << out.str();
		}
	}

	void* allocateMemory( size_t nobj, size_t size )
	{
		// Use standard function "calloc(...)" as default.
		return std::calloc( nobj, size );
	}

	void freeMemory( void* obj )
	{
		// Use standard function "free(...)" as default.
		std::free( obj );
	}

	void stepFinished( fmi2ComponentEnvironment componentEnvironment, fmi2Status status )
	{
		// Do nothing.
	}
}
