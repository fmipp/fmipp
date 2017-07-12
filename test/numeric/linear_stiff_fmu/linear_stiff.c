/**-------------------------------------------------------------------
 * Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * -------------------------------------------------------------------
 * linear_stiff test-fmu for fmi++
 *
 * this fmu corresponds to the following ode
 *
 *	dot( x ) =  998 * x + 1998 * y;
 *	dot( y ) = -999 * x - 1999 * y;
 *
 * with initial conditions
 *
 *	x(0) = 1, y(0) = 0
 *
 * the solution is
 *
 *	x(t) = 2*exp(-t) - exp( -1000*t )
 *	y(t) =  -exp(-t) + exp( -1000*t )
 *
 **/

#define MODEL_IDENTIFIER linear_stiff
#include "fmiModelFunctions.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#define x_ 0
#define y_ 1

typedef struct fmustruct
{
	fmiString instanceName;
	fmiReal time;
	fmiReal rvar[2];
	//	fmiReal ind[1];
} fmustruct;


DllExport const char* fmiGetModelTypesPlatform()
{
	return fmiModelTypesPlatform;
}


DllExport const char* fmiGetVersion()
{
	return fmiVersion;
}


DllExport fmiComponent fmiInstantiateModel( fmiString instanceName,
					    fmiString            GUID,
					    fmiCallbackFunctions functions,
					    fmiBoolean           loggingOn )
{
	fmustruct* fmu = NULL;

	if ( !strcmp( GUID, "{87654321-1234-1234-1234-12345678910f}" ) )
		return NULL;
	fmu = malloc( sizeof( fmustruct ) );
	fmu->instanceName = instanceName;

	return (void*)fmu;
}


DllExport void fmiFreeModelInstance( fmiComponent c )
{
	free( c );
}


DllExport fmiStatus fmiSetDebugLogging( fmiComponent c, fmiBoolean loggingOn )
{

	return fmiOK;
}


DllExport fmiStatus fmiSetTime( fmiComponent c, fmiReal time )
{
	fmustruct* fmu = (fmustruct*) c;
	fmu->time = time;

	return fmiOK;
}


DllExport fmiStatus fmiSetContinuousStates( fmiComponent c, const fmiReal x[], size_t nx )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for ( i = 0; i < nx; i++ )
		fmu->rvar[i] = x[i];

	return fmiOK;
}


DllExport fmiStatus fmiCompletedIntegratorStep( fmiComponent c, fmiBoolean* callEventUpdate )
{
	*callEventUpdate = fmiFalse;
	return fmiOK;
}


DllExport fmiStatus fmiSetReal( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[] )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for ( i = 0; i < nvr; i++ )
		fmu->rvar[vr[i]] = value[i];

	return fmiOK;
}


DllExport fmiStatus fmiSetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[] )
{

	return fmiOK;
}


DllExport fmiStatus fmiSetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[] )
{

	return fmiOK;
}


DllExport fmiStatus fmiSetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[] )
{

	return fmiOK;
}


DllExport fmiStatus fmiInitialize( fmiComponent c,
				   fmiBoolean toleranceControlled,
				   fmiReal relativeTolerance,
				   fmiEventInfo* eventInfo )
{
        fmustruct* fmu = (fmustruct*) c;
	fmu->rvar[x_] = 1;
	fmu->rvar[y_] = 0;
	fmu->time = 0.0;

	eventInfo->upcomingTimeEvent = fmiFalse;
	eventInfo->terminateSimulation = fmiFalse;

	return fmiOK;
}


DllExport fmiStatus fmiGetDerivatives( fmiComponent c, fmiReal derivatives[], size_t nx )
{
	fmustruct* fmu = (fmustruct*) c;
	fmiReal x = fmu->rvar[x_];
	fmiReal y = fmu->rvar[y_];

	derivatives[0] =  998 * x + 1998 * y;
	derivatives[1] = -999 * x - 1999 * y;

	return fmiOK;
}


DllExport fmiStatus fmiGetEventIndicators( fmiComponent c, fmiReal eventIndicators[], size_t ni )
{
	return fmiOK;
}


DllExport fmiStatus fmiGetReal( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal value[] )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for ( i = 0; i < nvr; i++ )
		value[i] = fmu->rvar[vr[i]];

	return fmiOK;
}


DllExport fmiStatus fmiGetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[] )
{

	return fmiOK;
}


DllExport fmiStatus fmiGetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[] )
{

	return fmiOK;
}


DllExport fmiStatus fmiGetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[] )
{

	return fmiOK;
}


DllExport fmiStatus fmiEventUpdate( fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo* eventInfo )
{
        //fmustruct* fmu = (fmustruct*) c;

	eventInfo->iterationConverged = fmiTrue;
	eventInfo->upcomingTimeEvent = fmiFalse;
	eventInfo->terminateSimulation = fmiFalse;

	return fmiOK;
}


DllExport fmiStatus fmiGetContinuousStates( fmiComponent c, fmiReal states[], size_t nx )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for( i = 0; i < nx; i++ )
		states[i] = fmu->rvar[i];

	return fmiOK;
}

DllExport fmiStatus fmiGetNominalContinuousStates( fmiComponent c, fmiReal x_nominal[], size_t nx )
{

	return fmiOK;
}


DllExport fmiStatus fmiGetStateValueReferences( fmiComponent c, fmiValueReference vrx[], size_t nx )
{
	//fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for( i = 0; i < nx; i++ )
		vrx[i] = fmiUndefinedValueReference;

	return fmiOK;
}


DllExport fmiStatus fmiTerminate(fmiComponent c)
{

	return fmiOK;
}
