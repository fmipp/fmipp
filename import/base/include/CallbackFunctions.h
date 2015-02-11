/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file CallbackFunctions.h
 * Provide default callback functions for FMUs for ME and CS.
 */


#include "common/fmi_v1.0/fmiModelTypes.h"

#include "common/FMIPPConfig.h"

/// This namespace contains very simple default implementations for FMI callback functions.
namespace callback
{
	__FMI_DLL void logger( fmiComponent c, fmiString instanceName, fmiStatus status,
		     fmiString category, fmiString message, ... );

	__FMI_DLL void* allocateMemory( size_t nobj, size_t size );

	__FMI_DLL void freeMemory( void* obj );

	__FMI_DLL void stepFinished( fmiComponent c, fmiStatus status );
}
