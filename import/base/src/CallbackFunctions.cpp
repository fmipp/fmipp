/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file CallbackFunctions.cpp
 * Provide default callback functions for FMUs for ME and CS.
 */


#include <cstdlib>
#include <cstdio>
#include <cstdarg>


#include "import/base/include/CallbackFunctions.h"

namespace callback{


	void logger( fmiComponent c, fmiString instanceName, fmiStatus status,
		     fmiString category, fmiString message, ... )
	{
		char msg[4096];
		char buf[4096];
		int len;
		int capacity;

		va_list ap;
		va_start( ap, message );
		capacity = sizeof(buf) - 1;

#if defined(_MSC_VER) && _MSC_VER>=1400
		len = _snprintf_s( msg, capacity, _TRUNCATE, "%s [%s]: %s", instanceName, category, message );
		if ( len < 0 ) goto fail;
		len = vsnprintf_s( buf, capacity, _TRUNCATE, msg, ap );
		if ( len < 0 ) goto fail;
#elif defined(WIN32)
		len = _snprintf( msg, capacity, "%s [%s]: %s", instanceName, category, message );
		if ( len < 0 ) goto fail;
		len = vsnprintf( buf, capacity, msg, ap );
		if ( len < 0 ) goto fail;
#else
		len = snprintf( msg, capacity, "%s [%s]: %s", instanceName, category, message );
		if ( len < 0 ) goto fail;
		len = vsnprintf( buf, capacity, msg, ap );
		if ( len < 0 ) goto fail;
#endif

		// Append line break.
		buf[len] = '\n';
		buf[len + 1] = 0;
		va_end( ap );

		fprintf( stderr, "%s", buf );

		return;
	fail:
		fprintf( stderr, "logger failed, message too long?" );
}


	void* allocateMemory( size_t nobj, size_t size )
	{
		// Use standard function "calloc(...)" as default.
		return calloc( nobj, size );
	}


	void freeMemory( void* obj )
	{
		// Use standard function "free(...)" as default.
		free( obj );
	}


	void stepFinished( fmiComponent c, fmiStatus status )
	{
		// Do nothing.
	}
}


namespace callback2{
	// contains copies of the functions from namespace callback, but  with different types
	// (fmi2Boolean instead of fmiBoolean, etc.).

	void logger( fmi2ComponentEnvironment c, fmi2String instanceName, fmi2Status status,
		     fmi2String category, fmi2String message, ... )
	{
		char msg[4096];
		char buf[4096];
		int len;
		int capacity;

		va_list ap;
		va_start( ap, message );
		capacity = sizeof(buf) - 1;

#if defined(_MSC_VER) && _MSC_VER>=1400
		len = _snprintf_s( msg, capacity, _TRUNCATE, "%s [%s]: %s", instanceName, category, message );
		if ( len < 0 ) goto fail;
		len = vsnprintf_s( buf, capacity, _TRUNCATE, msg, ap );
		if ( len < 0 ) goto fail;
#elif defined(WIN32)
		len = _snprintf( msg, capacity, "%s [%s]: %s", instanceName, category, message );
		if ( len < 0 ) goto fail;
		len = vsnprintf( buf, capacity, msg, ap );
		if ( len < 0 ) goto fail;
#else
		len = snprintf( msg, capacity, "%s [%s]: %s", instanceName, category, message );
		if ( len < 0 ) goto fail;
		len = vsnprintf( buf, capacity, msg, ap );
		if ( len < 0 ) goto fail;
#endif

		// Append line break.
		buf[len] = '\n';
		buf[len + 1] = 0;
		va_end( ap );

		printf( "%s", buf );

		return;
	fail:
		printf( "logger failed, message too long?" );
}


	void* allocateMemory( size_t nobj, size_t size )
	{
		// Use standard function "calloc(...)" as default.
		return calloc( nobj, size );
	}


	void freeMemory( void* obj )
	{
		// Use standard function "free(...)" as default.
		free( obj );
	}


	void stepFinished( fmi2Component c, fmi2Status status )
	{
		// Do nothing.
	}

}
