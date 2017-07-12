/**-------------------------------------------------------------------
 * Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * -------------------------------------------------------------------
 *
 * stiff test-fmu for fmi++
 *
 * this fmu corresponds to the following ODE
 *
 *	dot(y) =  k*y*(1-y)     , t <= ts
 *	       = -k*y*(1-y)     , t >  ts
 *	  y(0) = 1/(1+exp(k/2))
 *
 * with a parameter ts. The default value is
 *
 *	ts = 0.5.
 *
 * The solution of this system is
 *
 *	y(t) =	exp( k*    t    )/( 1+exp(k*   t    ) )    , t <  ts
 *	        exp( k*(2*ts-t) )/( 1+exp(k*(2*ts-t)) )	   , t >= ts
 *
 * the event ( t = ts ) is implemented as an int event even tough a
 * time event implementation would be more natural.
 *
 **/

#define MODEL_IDENTIFIER stiff
#include "fmiModelFunctions.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#define x_ 0
#define k_ 1
#define x0_ 2
#define sgn_ 3
#define ts_ 4

typedef struct fmustruct
{
	fmiString instanceName;
	fmiReal time;
	fmiReal rvar[5];
	fmiReal ind[1];
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

	if ( !strcmp( GUID, "{12345678-1234-1234-1234-12345678910f}" ) )
		return NULL;
	fmu = malloc( sizeof( fmustruct ) );
	fmu->instanceName = instanceName;

	fmu->rvar[k_] = 100;
	fmu->rvar[ts_] = 0.5;

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
        fmustruct* fmu = (fmustruct*) c;
	if ( (fmu->time) >= (fmu->rvar[ts_])) {
		fmu->rvar[sgn_]= -1;
		*callEventUpdate = fmiFalse;
	} else {
	        *callEventUpdate = fmiFalse;
		 fmu->rvar[sgn_] = 1;
	}

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
	fmiReal k = fmu->rvar[k_];
	fmu->rvar[x0_] = 1.0/( 1.0 + exp(k/2.0) );
	fmu->rvar[x_] = fmu->rvar[x0_];
	fmu->time = 0.0;
	fmu->rvar[sgn_] = 1;


	eventInfo->upcomingTimeEvent = fmiFalse;
	eventInfo->terminateSimulation = fmiFalse;

	return fmiOK;
}


DllExport fmiStatus fmiGetDerivatives( fmiComponent c, fmiReal derivatives[], size_t nx )
{
	fmustruct* fmu = (fmustruct*) c;
	fmiReal y = fmu->rvar[x_];
	fmiReal k = fmu->rvar[k_];
	fmiReal sgn = fmu->rvar[sgn_];
	derivatives[0] =  sgn*y*( 1-y )*k;

	return fmiOK;
}


DllExport fmiStatus fmiGetEventIndicators( fmiComponent c, fmiReal eventIndicators[], size_t ni )
{
        fmustruct* fmu = (fmustruct*) c;
	if ((fmu->time) < (fmu->rvar[ts_])){
		eventIndicators[0] =  1;
        }else {
		eventIndicators[0] = -1;
	}
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
