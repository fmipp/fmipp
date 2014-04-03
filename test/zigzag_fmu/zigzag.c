#define MODEL_IDENTIFIER zigzag
#include "fmiModelFunctions.h"

#include <string.h>
#include <stdio.h>

#define x_ 0
#define der_x_ 1
#define k_ 2
#define x0_ 3

typedef struct fmustruct
{
	fmiString instanceName;
	fmiReal time;
	fmiReal rvar[4];
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

	fmu->time = 0;
	fmu->rvar[k_] = 1;
	fmu->rvar[x0_] = 0;

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

	if ( fmu->rvar[x_] >= 1 ) {
		fmu->rvar[der_x_] = -(fmu->rvar[k_]);
		*callEventUpdate = fmiTrue;
	} else if ( fmu->rvar[x_] <= -1 ) {
		fmu->rvar[der_x_] = fmu->rvar[k_];
		*callEventUpdate = fmiTrue;
	} else {
		*callEventUpdate = fmiFalse;
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
	fmu->rvar[x_] = fmu->rvar[x0_];
	fmu->rvar[der_x_] = fmu->rvar[k_];
	if ( fmu->rvar[k_] < 0 )
		fmu->rvar[k_] = -(fmu->rvar[k_]);

	eventInfo->upcomingTimeEvent = fmiFalse;
	eventInfo->terminateSimulation = fmiFalse;

	return fmiOK;
}

DllExport fmiStatus fmiGetDerivatives( fmiComponent c, fmiReal derivatives[], size_t nx )
{
	fmustruct* fmu = (fmustruct*) c;
	if ( fmu->rvar[x_] >= 1 ) {
		derivatives[0] = fmu->rvar[der_x_] = -(fmu->rvar[k_]);
	} else if ( fmu->rvar[x_] <= -1 ) {
		derivatives[0] = fmu->rvar[der_x_] = fmu->rvar[k_];
	} else {
		derivatives[0] = fmu->rvar[der_x_];
	}

	return fmiOK;
}

DllExport fmiStatus fmiGetEventIndicators( fmiComponent c, fmiReal eventIndicators[], size_t ni )
{
	fmustruct* fmu = (fmustruct*) c;
	if ( fmu->rvar[x_] >= 1 ) {
		eventIndicators[0] = 1;
	} else if ( fmu->rvar[x_] <= -1 ) {
		eventIndicators[0] = -1;
	} else {
		eventIndicators[0] = fmu->rvar[der_x_] > 0 ? -1 : 1;
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
