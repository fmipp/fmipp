/** --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------
 *
 * robertson test-fmu for fmi++
 *
 * this fmu corresponds to the following ODE
 *
 *   dot( x ) = -0.04 * x + 10^4 * y * z
 *   dot( y ) =  0.04 * x - 10^4 * y * z - 3 * 10^7 * y^2
 *   dot( z ) =                            3 * 10^7 * y^2
 *
 * with the initial conditions
 *
 *   x( 0 ) = 1, y( 0 ) = 0, z( 0 ) = 0
 *
 * The problem is known as the robertson chemical reaction. It is stiff.
 *
 **/

#define MODEL_IDENTIFIER robertson
#include "fmi2ModelFunctions.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#define x_     0
#define der_x_ 1
#define y_     2
#define der_y_ 3
#define z_     4
#define der_z_ 5

typedef struct ModelInstance
{
	fmi2String instanceName;
	fmi2Real   time;
	fmi2Real   rvar[6];
	fmi2Type   type;
	fmi2String GUID;
	const fmi2CallbackFunctions *functions;
	fmi2EventInfo eventInfo;
	ModelState state;
	fmi2ComponentEnvironment componentEnvironment;
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
	if ( fmuType == fmi2CoSimulation )
		return NULL;

	ModelInstance* fmu = NULL;

	if ( !strcmp( GUID, "{12345678-1234-1234-1234-12345678987f}" ) )
		return NULL;

	fmu = (ModelInstance*) functions->allocateMemory( 1, sizeof( ModelInstance ) );

	fmu->instanceName = instanceName;
	fmu->GUID = GUID;

	fmu->rvar[x_] = 1;
	fmu->rvar[y_] = 0;
	fmu->rvar[z_] = 0;

	fmu->functions = (fmi2CallbackFunctions*) functions;

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
					   fmi2Boolean   toleranceDefined, fmi2Real tolerance,
					   fmi2Real      startTime,
					   fmi2Boolean   stopTimeDefined , fmi2Real stopTime)
{
	ModelInstance* fmu = (ModelInstance*) c;

	if ( fmu->state != modelInstantiated )
		return fmi2Error;

	fmu->time = startTime;
	fmu->eventInfo.terminateSimulation = fmi2False;
	return fmi2OK;
}

FMI2_Export fmi2Status fmi2EnterInitializationMode(fmi2Component c) {
	ModelInstance *fmu = (ModelInstance*) c;

	if ( fmu->state != modelInstantiated )
		return fmi2Error;

	fmu->state = modelInitializationMode;
	return fmi2OK;
}

FMI2_Export fmi2Status fmi2ExitInitializationMode(fmi2Component c) {
	ModelInstance *fmu = (ModelInstance*) c;

	if ( fmu->state != modelInitializationMode )
		return fmi2Error;

	fmu->state = modelEventMode;
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2Terminate(fmi2Component c)
{
	ModelInstance *fmu = (ModelInstance*) c;

	if ( fmu->state != modelContinuousTimeMode && fmu->state != modelEventMode )
		return fmi2Error;

	fmu->state = modelTerminated;
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2Reset( fmi2Component c )
{
	ModelInstance *fmu = (ModelInstance*) c;

	if ( fmu->state == modelStartAndEnd && fmu->state == modelFatal )
		return fmi2Error;

	// go back to the initial setup of the model i.e. the starting values for x, y, z
	fmu->rvar[x_] = 1;
	fmu->rvar[y_] = 0;
	fmu->rvar[z_] = 0;

	// set back the event info ...
	fmu->eventInfo.newDiscreteStatesNeeded = fmi2False;
	fmu->eventInfo.terminateSimulation = fmi2False;
	fmu->eventInfo.nominalsOfContinuousStatesChanged = fmi2False;
	fmu->eventInfo.valuesOfContinuousStatesChanged = fmi2False;
	fmu->eventInfo.nextEventTimeDefined = fmi2False;
	fmu->eventInfo.nextEventTime = 0;

	// ... and the model state
	fmu->state = modelInstantiated;

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
	// no integers in the model, just return
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetBoolean( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[] )
{
	// no bools in the model, just return
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetString( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String  value[] )
{
	// no strings in the model, jsut return
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
	// no integers it the model, just return
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2SetBoolean( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[] )
{
	// no bools in the model, just return
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2SetString( fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String  value[] )
{
	// no strings in the model, jsut return
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

double J( fmi2ValueReference output, fmi2ValueReference input , fmi2Component c, fmi2Status* status )
{
	ModelInstance* fmu = (ModelInstance*) c;
	if ( output == der_x_ && input == x_ )
		return -0.04;
	else if ( output == der_x_ && input == y_ )
		return 1.0e4 * fmu->rvar[ z_ ];
	else if ( output == der_x_ && input == z_ )
		return 1.0e4 * fmu->rvar[ y_ ];

	else if ( output == der_y_ && input == x_ )
		return 0.04;
	else if ( output == der_y_ && input == y_ )
		return -1.0e4 * fmu->rvar[ z_ ] - 6.0e7 * fmu->rvar[ y_ ];
	else if ( output == der_y_ && input == z_ )
		return -1.0e4 * fmu->rvar[ y_ ];

	else if ( output == der_z_ && input == x_ )
		return 0.0;
	else if ( output == der_z_ && input == y_ )
		return 6.0e7 * fmu->rvar[ y_ ];
	else if ( output == der_z_ && input == z_ )
		return 0.0;
	else{
		// return an error signal
		*status = fmi2Discard;
		return 0;
	}
}

FMI2_Export fmi2Status fmi2GetDirectionalDerivative(fmi2Component c,
						    const fmi2ValueReference vUnknown_ref[], size_t nUnknown,
						    const fmi2ValueReference vKnown_ref[]  , size_t nKnown,
						    const fmi2Real dvKnown[], fmi2Real dvUnknown[])
{
	ModelInstance *fmu = (ModelInstance*) c;
	fmi2Status status = fmi2OK;
	int i,j;
	double jacElement;

	if ( fmu->state != modelContinuousTimeMode )
		return fmi2Discard;

	// assume vUnknown_ref is a subset of all derivatives (1,3,5) and vKnown_ref is a subset of all
	// states (0,2,4)
	for ( i = 0; i < nUnknown; i++ ){
		dvUnknown[i] = 0;
		// calculate the derivative of vUnknown with respect to vKnown_ref[i]
		for ( j = 0; j < nKnown; j++ ){
			// calculate the derivative of vUnknown_ref[j] with respect to vKnown_ref[i]
			jacElement = J( vUnknown_ref[i], vKnown_ref[j], c, &status );
			if ( status != fmi2OK )
				return status;
			dvUnknown[i] += dvKnown[j] * jacElement;
		}
	}
	return fmi2OK;
}


//************ CoSimulation Functions *********************

FMI2_Export fmi2Status fmi2EnterEventMode(fmi2Component c)
{
	// change mode
	ModelInstance *fmu = (ModelInstance*) c;
	fmu->state = modelEventMode;

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo *eventInfo)
{	ModelInstance *fmu = (ModelInstance*) c;

	if ( fmu->state != modelEventMode )
		return fmi2Error;

	// event iterations are unnecessary for this fmu. Tell this the environment and
	// do nothing otherwise.
	fmu->eventInfo.newDiscreteStatesNeeded = fmi2False;
	fmu->eventInfo.terminateSimulation = fmi2False;
	fmu->eventInfo.nominalsOfContinuousStatesChanged = fmi2False;
	fmu->eventInfo.valuesOfContinuousStatesChanged = fmi2False;

	// copy internal eventInfo to output eventInfo
	*eventInfo = fmu->eventInfo;

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c)
{
	ModelInstance *fmu = (ModelInstance*) c;

	if ( fmu->state != modelEventMode )
		return fmi2Error;

	// change the model state
	fmu->state = modelContinuousTimeMode;

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2CompletedIntegratorStep(fmi2Component c,
						   fmi2Boolean noSetFMUStatePriorToCurrentPoint,
						   fmi2Boolean* enterEventMode,
						   fmi2Boolean* terminateSimulation)
{
	*enterEventMode      = fmi2False;
	*terminateSimulation = fmi2False;
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
		fmu->rvar[2*i] = x[i];

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetDerivatives( fmi2Component c, fmi2Real derivatives[], size_t nx )
{
	ModelInstance* fmu = (ModelInstance*) c;
	fmi2Real x = fmu->rvar[x_];
	fmi2Real y = fmu->rvar[y_];
	fmi2Real z = fmu->rvar[z_];

	derivatives[0] = fmu->rvar[der_x_] = -0.04*x + 1.0e4*y*z;
	derivatives[1] = fmu->rvar[der_y_] =  0.04*x - 1.0e4*y*z - 3.0e7*y*y;
	derivatives[2] = fmu->rvar[der_z_] =                       3.0e7*y*y;

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetEventIndicators( fmi2Component c, fmi2Real eventIndicators[], size_t ni )
{
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetContinuousStates( fmi2Component c, fmi2Real states[], size_t nx )
{
	ModelInstance* fmu = (ModelInstance*) c;
	size_t i;
	for( i = 0; i < nx; i++ )
		states[i] = fmu->rvar[2*i];

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetNominalsOfContinuousStates( fmi2Component c, fmi2Real x_nominal[], size_t nx )
{
	int i;
	for ( i = 0; i < nx; i++ )
		x_nominal[i] = 1.0;
	return fmi2OK;
}
