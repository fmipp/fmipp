// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#define MODEL_IDENTIFIER v2_0
#include "fmi2Functions.h"
#define DllExport FMI2_Export

#include <string.h>
#include <stdio.h>

#define x_ 0
#define der_x_ 1
#define k_ 2
#define x0_ 3

typedef struct fmustruct
{
	fmi2String instanceName;
	fmi2Real time;
	fmi2Real rvar[4];
} fmustruct;


DllExport const char* fmi2GetTypesPlatform()
{
	return fmi2TypesPlatform;
}


DllExport const char* fmi2GetVersion()
{
	return fmi2Version;
}


DllExport fmi2Component fmi2Instantiate( fmi2String instanceName,
										 fmi2Type fmuType,
										 fmi2String GUID,
										 fmi2String fmuResourceLocation,
										 const fmi2CallbackFunctions* functions,
										 fmi2Boolean visible,
										 fmi2Boolean loggingOn )
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


DllExport fmi2Status fmi2SetupExperiment( fmi2Component c, fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime )
{
	fmustruct* fmu = (fmustruct*) c;

	fmu->time = startTime;

	return fmi2OK;
}


DllExport fmi2Status fmi2EnterInitializationMode( fmi2Component c )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2ExitInitializationMode(fmi2Component c )
{
	return fmi2OK;
}


DllExport void fmi2FreeInstance( fmi2Component c )
{
	free( c );
}


DllExport fmi2Status fmi2SetDebugLogging( fmi2Component c, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[] )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2Reset( fmi2Component c )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2SetTime( fmi2Component c, fmi2Real time )
{
	fmustruct* fmu = (fmustruct*) c;
	fmu->time = time;

	return fmi2OK;
}


DllExport fmi2Status fmi2SetContinuousStates( fmi2Component c, const fmi2Real x[], size_t nx )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for ( i = 0; i < nx; i++ )
		fmu->rvar[i] = x[i];

	return fmi2OK;
}


DllExport fmi2Status fmi2EnterEventMode( fmi2Component c )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2NewDiscreteStates( fmi2Component c , fmi2EventInfo* eventInfo )
{
	eventInfo->newDiscreteStatesNeeded = fmi2False;
	return fmi2OK;
}


DllExport fmi2Status fmi2EnterContinuousTimeMode( fmi2Component c )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2CompletedIntegratorStep( fmi2Component c, fmi2Boolean noSetFMUStatePriorToCurrentPoint, fmi2Boolean* enterEventMode, fmi2Boolean* terminateSimulation )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2SetReal( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[] )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for ( i = 0; i < nvr; i++ )
		fmu->rvar[vr[i]] = value[i];

	return fmi2OK;
}


DllExport fmi2Status fmi2SetInteger( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[] )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2SetBoolean( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[] )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2SetString( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String  value[] )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2Initialize( fmi2Component c,
				   fmi2Boolean toleranceControlled,
				   fmi2Real relativeTolerance,
				   fmi2EventInfo* eventInfo )
{
	return fmi2OK;
}

DllExport fmi2Status fmi2GetDerivatives( fmi2Component c, fmi2Real derivatives[], size_t nx )
{
	fmustruct* fmu = (fmustruct*) c;
	derivatives[0] = 0;

	return fmi2OK;
}

DllExport fmi2Status fmi2GetEventIndicators( fmi2Component c, fmi2Real eventIndicators[], size_t ni )
{

	return fmi2OK;
}

DllExport fmi2Status fmi2GetReal( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[] )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for ( i = 0; i < nvr; i++ )
		value[i] = fmu->rvar[vr[i]];

	return fmi2OK;
}

DllExport fmi2Status fmi2GetInteger( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[] )
{

	return fmi2OK;
}


DllExport fmi2Status fmi2GetBoolean( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[] )
{

	return fmi2OK;
}


DllExport fmi2Status fmi2GetString( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String  value[] )
{

	return fmi2OK;
}


DllExport fmi2Status fmi2EventUpdate( fmi2Component c, fmi2Boolean intermediateResults, fmi2EventInfo* eventInfo )
{
	//fmustruct* fmu = (fmustruct*) c;

	eventInfo->newDiscreteStatesNeeded = fmi2False;
	eventInfo->nextEventTimeDefined = fmi2False;
	eventInfo->terminateSimulation = fmi2False;

	return fmi2OK;
}


DllExport fmi2Status fmi2GetContinuousStates( fmi2Component c, fmi2Real states[], size_t nx )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for( i = 0; i < nx; i++ )
		states[i] = fmu->rvar[i];

	return fmi2OK;
}
		

DllExport fmi2Status fmi2GetNominalsOfContinuousStates( fmi2Component c, fmi2Real x_nominal[], size_t nx )
{
	return fmi2OK;
}


DllExport fmi2Status fmi2Terminate(fmi2Component c)
{
	return fmi2OK;
}
