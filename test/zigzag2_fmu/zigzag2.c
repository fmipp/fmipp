/** --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------
 *
 * zigzag2 test-fmu for fmi++
 *
 **/

#define MODEL_IDENTIFIER zigzag2
#include "fmi2ModelFunctions.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#define x_     0
#define der_x_ 1
#define k_     2
#define x0_    3

#define state_de_ 0

typedef struct ModelInstance
{
	fmi2String instanceName;
	fmi2Real time;
	fmi2Real rvar[4];
	fmi2Real ind[1];
	fmi2Integer ivar[1];
	fmi2Type type;
	fmi2String GUID;
	const fmi2CallbackFunctions *functions;
	fmi2EventInfo eventInfo;
	ModelState state;
	fmi2ComponentEnvironment componentEnvironment;//???
} ModelInstance;

//*********** Common Functions for ME and CS **************

FMI2_Export const char* fmi2GetTypesPlatform()
{
	return fmi2ModelTypesPlatform;
}


FMI2_Export const char* fmi2GetVersion()
{
	return fmi2Version;
}


FMI2_Export fmi2Status fmi2SetDebugLogging( fmi2Component c,
					    fmi2Boolean loggingOn,
					    size_t n_Categories,
					    const fmi2String categories[] )
{
	return fmi2OK;
}


FMI2_Export fmi2Component fmi2Instantiate( fmi2String  instanceName,
					   fmi2Type    fmuType,
					   fmi2String  GUID,
					   fmi2String  fmuResourceLocation,
					   const fmi2CallbackFunctions* functions,
					   fmi2Boolean visible,
					   fmi2Boolean loggingOn )
{
	ModelInstance* fmu = NULL;

	if ( strcmp( GUID, "{12345678-1234-1234-1235-12345678910f}" ) )
		return NULL;

	fmu = (ModelInstance*) functions->allocateMemory( 1, sizeof( ModelInstance ) );

	fmu->instanceName = instanceName;
	fmu->GUID = GUID;

	fmu->time = 0;
	fmu->rvar[k_]  = 1;
	fmu->rvar[x0_] = 0;
	fmu->ivar[state_de_] = 1;

	fmu->functions = (fmi2CallbackFunctions*) functions;
	//fmu->eventInfo = malloc( sizeof(fmi2EventInfo) );

	fmu->type = fmuType;

	fmu->state = modelInstantiated;

	fmu->eventInfo.newDiscreteStatesNeeded = fmi2False;
	fmu->eventInfo.terminateSimulation = fmi2False;
	fmu->eventInfo.nominalsOfContinuousStatesChanged = fmi2False;
	fmu->eventInfo.valuesOfContinuousStatesChanged = fmi2False;
	fmu->eventInfo.nextEventTimeDefined = fmi2False;
	fmu->eventInfo.nextEventTime = 0;

	return fmu;
}


FMI2_Export void fmi2FreeInstance( fmi2Component c )
{
	ModelInstance* fmu = (ModelInstance*) c;
	fmu->functions->freeMemory( fmu );
}


FMI2_Export fmi2Status fmi2SetupExperiment(fmi2Component c,
					 fmi2Boolean    toleranceDefined, fmi2Real tolerance,
					 fmi2Real      startTime,
					 fmi2Boolean   stopTimeDefined , fmi2Real stopTime)
{
        ModelInstance* fmu = (ModelInstance*) c;

	fmu->rvar[x_] = fmu->rvar[x0_];
	fmu->rvar[der_x_] = fmu->ivar[state_de_] * fmu->rvar[k_];
	if ( fmu->rvar[k_] < 0 )
		fmu->rvar[k_] = -(fmu->rvar[k_]);

	fmu->eventInfo.nextEventTimeDefined = fmi2False;
	fmu->eventInfo.terminateSimulation  = fmi2False;

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2EnterInitializationMode(fmi2Component c) {
	ModelInstance *fmu = (ModelInstance*) c;
	fmu->state = modelInitializationMode;
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2ExitInitializationMode(fmi2Component c) {
	ModelInstance *fmu = (ModelInstance*) c;
	fmu->state = modelStepComplete;
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2Terminate(fmi2Component c)
{
	ModelInstance *fmu = (ModelInstance*) c;
	fmu->state = modelTerminated;
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2Reset( fmi2Component c )
{
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetReal( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[] )
{
	ModelInstance* fmu = (ModelInstance*) c;
	size_t i;
	for ( i = 0; i < nvr; i++ )
		value[i] = fmu->rvar[vr[i]];

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetInteger( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[] )
{
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetBoolean( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[] )
{
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetString( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String  value[] )
{
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2SetReal( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[] )
{
	ModelInstance* fmu = (ModelInstance*) c;
	size_t i;
	for ( i = 0; i < nvr; i++ )
		fmu->rvar[vr[i]] = value[i];
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2SetInteger( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[] )
{
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2SetBoolean( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[] )
{
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2SetString( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String  value[] )
{
	return fmi2OK;
}


/// ************* unsupported functions ***********

FMI2_Export fmi2Status fmi2GetFMUstate (fmi2Component c, fmi2FMUstate* FMUstate)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2SetFMUstate (fmi2Component c, fmi2FMUstate FMUstate)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2FreeFMUstate(fmi2Component c, fmi2FMUstate* FMUstate)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2SerializedFMUstateSize(fmi2Component c, fmi2FMUstate FMUstate, size_t *size)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2SerializeFMUstate (fmi2Component c, fmi2FMUstate FMUstate, fmi2Byte serializedState[], size_t size)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2DeSerializeFMUstate (fmi2Component c, const fmi2Byte serializedState[], size_t size,
                                    fmi2FMUstate* FMUstate)
{
	return fmi2Error;
}

FMI2_Export fmi2Status fmi2GetDirectionalDerivative(fmi2Component c,
						    const fmi2ValueReference vUnknown_ref[], size_t nUnknown,
						    const fmi2ValueReference vKnown_ref[]  , size_t nKnown,
						    const fmi2Real dvKnown[], fmi2Real dvUnknown[])
{
	return fmi2Error;
}


//************ CoSimulation Functions *********************

FMI2_Export fmi2Status fmi2EnterEventMode(fmi2Component c)
{
	ModelInstance *fmu = (ModelInstance*) c;
	fmu->state = modelEventMode;
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo *eventInfo)
{	ModelInstance *fmu = (ModelInstance*) c;

	fmu->eventInfo.newDiscreteStatesNeeded           = fmi2False;
	fmu->eventInfo.terminateSimulation               = fmi2False;
	fmu->eventInfo.nominalsOfContinuousStatesChanged = fmi2False;
	fmu->eventInfo.valuesOfContinuousStatesChanged   = fmi2False;
	fmu->eventInfo.nextEventTimeDefined              = fmi2False;

	// copy internal eventInfo of component to output eventInfo
	*eventInfo = fmu->eventInfo;

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c)
{
	ModelInstance *fmu = (ModelInstance*) c;

	fmu->state = modelContinuousTimeMode;

	fmu->eventInfo.terminateSimulation = fmi2False;


	return fmi2OK;
}


FMI2_Export fmi2Status fmi2CompletedIntegratorStep(fmi2Component c,
						   fmi2Boolean noSetFMUStatePriorToCurrentPoint,
						   fmi2Boolean* enterEventMode,
						   fmi2Boolean* terminateSimulation)
{
        ModelInstance* fmu = (ModelInstance*) c;


	if ( fmu->rvar[x_] >= 1 ) {
		fmu->ivar[state_de_] = -1;
		fmu->rvar[der_x_] = fmu->ivar[state_de_] * fmu->rvar[k_];
		*enterEventMode = fmi2True;
	} else if ( fmu->rvar[x_] <= -1 ) {
		fmu->ivar[state_de_] = 1;
		fmu->rvar[der_x_] = fmu->ivar[state_de_] * fmu->rvar[k_];
		*enterEventMode = fmi2True;
	} else {
		*enterEventMode = fmi2False;
	}


	return fmi2OK;
}



FMI2_Export fmi2Status fmi2SetTime( fmi2Component c, fmi2Real time )
{

	ModelInstance* fmu = (ModelInstance*) c;
	fmu->time = time;

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2SetContinuousStates( fmi2Component c, const fmi2Real x[], size_t nx )
{
	ModelInstance* fmu = (ModelInstance*) c;
	size_t i;
	for ( i = 0; i < nx; i++ )
		fmu->rvar[i] = x[i];

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetDerivatives( fmi2Component c, fmi2Real derivatives[], size_t nx )
{
	ModelInstance* fmu = (ModelInstance*) c;
	derivatives[0] = fmu->ivar[state_de_] * fmu->rvar[k_];

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetEventIndicators( fmi2Component c, fmi2Real eventIndicators[], size_t ni )
{
        ModelInstance* fmu = (ModelInstance*) c;
	if ( fmu->rvar[x_] >= 1 ) {
		eventIndicators[0] = 1;
	} else if ( fmu->rvar[x_] <= -1 ) {
		eventIndicators[0] = -1;
	} else {
		eventIndicators[0] = fmu->rvar[der_x_] > 0 ? -1 : 1;
	}

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetContinuousStates( fmi2Component c, fmi2Real states[], size_t nx )
{
	ModelInstance* fmu = (ModelInstance*) c;
	size_t i;
	for( i = 0; i < nx; i++ )
		states[i] = fmu->rvar[i];

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetNominalsOfContinuousStates( fmi2Component c, fmi2Real x_nominal[], size_t nx )
{

	return fmi2OK;
}
