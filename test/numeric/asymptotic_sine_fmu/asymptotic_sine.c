/**-------------------------------------------------------------------
 * Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * -------------------------------------------------------------------
 * asymptotic_sine test-fmu for fmi++
 *
 * this fmu corresponds to the following ODE
 *
 *	dot(y) = A( y - F(t) ) + F'(t)
 *	y(0)   = ( 1.1, 0.1 )
 *
 * with
 *
 *	F(t) = ( sin(t), cos(t) )^T
 *	A    = ( -lambda    b       )
 *	       ( 0          -lambda )
 *
 * and
 *
 *	lambda = 100, b = 1
 *
 * since the matric A is strictly negative definite, it holds that
 *
 *	|| y(t) - F(t) || --> 0     for     t --> infinity
 *
 * this convergence happens faster for lambda >> 0. At the same time, a big
 * lambda makes the problem very stiff.
 *
 *
 * PS: The exact solution is
 *
 *	y(t) = F(t) + e^(At)*( y0 - F( t0 ) )
 *
 * where e^(At) is the matrix exponential function.
 *
 **/

#define MODEL_IDENTIFIER asymptotic_sine
#include "fmiModelFunctions.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#define x_ 0
#define y_ 1
#define b_ 2
#define lambda_ 3

typedef struct fmustruct
{
	fmiString instanceName;
	fmiReal time;
	fmiReal rvar[4];
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

	if ( !strcmp( GUID, "{12045678-1234-1234-1234-12345678910f}" ) )
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
	fmu->rvar[x_] = 1.1;
	fmu->time = 0.0;
	fmu->rvar[y_] = .1;
	fmu->rvar[b_] = 1.;
	fmu->rvar[lambda_] = 100.0;

	eventInfo->upcomingTimeEvent = fmiFalse;
	eventInfo->terminateSimulation = fmiFalse;

	return fmiOK;
}


DllExport fmiStatus fmiGetDerivatives( fmiComponent c, fmiReal derivatives[], size_t nx )
{
	fmustruct* fmu = (fmustruct*) c;
	fmiReal x      = fmu->rvar[x_];
	fmiReal y      = fmu->rvar[y_];
	fmiReal b      = fmu->rvar[b_];
	fmiReal lambda = fmu->rvar[lambda_];
	fmiReal t      = fmu->time;
	derivatives[0] =  -lambda*( x - sin(t) ) +      b*( y - cos(t) ) + cos(t);
	derivatives[1] =                         - lambda*( y - cos(t) ) - sin(t);
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
