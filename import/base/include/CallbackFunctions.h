// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file CallbackFunctions.h
 * Provide default callback functions for FMUs for ME and CS.
 */

#include "common/FMIPPConfig.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/fmi_v2.0/fmi2ModelTypes.h"

/// This namespace contains very simple default implementations for FMI callback functions.
namespace callback
{
	__FMI_DLL void verboseLogger( fmiComponent c, fmiString instanceName,
		fmiStatus status, fmiString category, fmiString message, ... );

	__FMI_DLL void succinctLogger( fmiComponent c, fmiString instanceName,
		fmiStatus status, fmiString category, fmiString message, ... );

	__FMI_DLL void* allocateMemory( size_t nobj, size_t size );

	__FMI_DLL void freeMemory( void* obj );

	__FMI_DLL void stepFinished( fmiComponent c, fmiStatus status );
}


/// Same as above, but with the types of FMI 2.0
namespace callback2
{
	__FMI_DLL void verboseLogger( fmi2ComponentEnvironment componentEnviroment, fmi2String instanceName,
		fmi2Status status, fmi2String category, fmi2String message, ... );

	__FMI_DLL void succinctLogger( fmi2ComponentEnvironment componentEnviroment, fmi2String instanceName,
		fmi2Status status, fmi2String category, fmi2String message, ... );

	__FMI_DLL void* allocateMemory( size_t nobj, size_t size );

	__FMI_DLL void freeMemory( void* obj );

	__FMI_DLL void stepFinished( fmi2ComponentEnvironment componentEnvironment, fmi2Status status );
}
