/**-------------------------------------------------------------------
 * Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * -------------------------------------------------------------------
 *
 * stiff2 test-fmu for fmi++
 *
 * this fmu corresponds to the following ODE
 *
 *	dot(y) =  k*y*(1-y)     , t <= ts
 *	       = -k*y*(1-y)     , t >  ts
 *	  y(0) = 1/(1+exp(k/2))
 *
 * with a configurable parameter ts. The default value is
 *
 *	ts = 0.5.
 *
 * The solution of this system is
 *
 *	y(t) =	exp( k*    t    )/( 1+exp(k*   t    ) )    , t <  ts,
 *		exp( k*(2*ts-t) )/( 1+exp(k*(2*ts-t)) )	   , t >= ts.
 *
 * The event ( t = ts ) is implemented as an int event even tough a
 * time event implementation would be more natural.
 *
 *
 * PS: The fmi2GetDirectionalDerivatives() function only works as intended if it gets called to get
 * the jacobian.
 *
 * \TODO: make the fmi2GetDirectionalDerivatives() more flexible
 *
 **/

#define MODEL_IDENTIFIER stiff2
#include "fmi2ModelFunctions.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#define x_     0
#define der_x_ 1
#define k_     2
#define x0_    3
#define sgn_   4
#define ts_    5

typedef struct ModelInstance
{
	fmi2String instanceName;
	fmi2Real time;
	fmi2Real rvar[6];
	fmi2Real ind[1];
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

	if ( !strcmp( GUID, "{12345678-1234-1234-1234-12345678910f}" ) )
		return NULL;

	fmu = (ModelInstance*) functions->allocateMemory( 1, sizeof( ModelInstance ) );

	fmu->instanceName = instanceName;
	fmu->GUID = GUID;

	fmu->rvar[k_] = 100;
	fmu->rvar[ts_] = 0.5;
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
	fmi2Real k = fmu->rvar[k_];
	fmu->rvar[x0_] = 1.0/( 1.0 + exp( k/2.0 ) );
	fmu->rvar[x_] = fmu->rvar[x0_];
	fmu->time = 0.0;
	fmu->rvar[sgn_] = 1;


	//eventInfo->upcomingTimeEvent = fmi2False;
	fmu->eventInfo.terminateSimulation = fmi2False;

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
	if ( nUnknown==1 && nKnown == 1 ){
		// assume we want the derivative of the RHS i.e. the function
		// is called during continuousTimeMode with "vUnknown=dx, vKnown=x".

		ModelInstance* fmu = (ModelInstance*) c;

		// Get the Values from the Model
		fmi2Real x   = fmu->rvar[x_];
		fmi2Real k   = fmu->rvar[k_];
		fmi2Real sgn = fmu->rvar[sgn_];

		fmi2Real df  = sgn*k*(1.0-2.0*x);

		dvUnknown[0] = dvKnown[0]*df;
		return fmi2OK;
	}
	return fmi2Error;
}


//************ CoSimulation Functions *********************

FMI2_Export fmi2Status fmi2SetRealInputDerivatives(fmi2Component c,
						const fmi2ValueReference vr[],
						size_t nvr,
						const fmi2Integer order[],
						const fmi2Real value[])
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2GetRealOutputDerivatives(fmi2Component c,
						const fmi2ValueReference vr[],
						size_t nvr,
						const fmi2Integer order[],
						fmi2Real value[])
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2DoStep(fmi2Component c,
						fmi2Real currentCommunicationPoint,
						fmi2Real communicationPointStepSize,
						fmi2Boolean noSetFMUStatePriorToCurrentPoint)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2CancelStep (fmi2Component c)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2GetStatus(fmi2Component c, const fmi2StatusKind s, fmi2Status*  value)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2GetRealStatus(fmi2Component c, const fmi2StatusKind s, fmi2Real* vlaue)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2GetIntegerStatus(fmi2Component c, const fmi2StatusKind s, fmi2Integer* value)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2GetBooleanStatus(fmi2Component c, const fmi2StatusKind s, fmi2Boolean* value)
{
	return fmi2Error;
}


FMI2_Export fmi2Status fmi2GetStringStatus(fmi2Component c, const fmi2StatusKind s, fmi2String* value)
{
	return fmi2Error;
}


//************ ModelExchange Functions *********************

FMI2_Export fmi2Status fmi2EnterEventMode(fmi2Component c)
{
	ModelInstance *fmu = (ModelInstance*) c;
	fmu->state = modelEventMode;
	return fmi2OK;
}


FMI2_Export fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo *eventInfo)
{	ModelInstance *fmu = (ModelInstance*) c;

	fmu->eventInfo.newDiscreteStatesNeeded = fmi2False;
	fmu->eventInfo.terminateSimulation = fmi2False;
	fmu->eventInfo.nominalsOfContinuousStatesChanged = fmi2False;
	fmu->eventInfo.valuesOfContinuousStatesChanged = fmi2False;

	// copy internal eventInfo of component to output eventInfo
	eventInfo->newDiscreteStatesNeeded = fmu->eventInfo.newDiscreteStatesNeeded;
	eventInfo->terminateSimulation = fmu->eventInfo.terminateSimulation;
	eventInfo->nominalsOfContinuousStatesChanged = fmu->eventInfo.nominalsOfContinuousStatesChanged;
	eventInfo->valuesOfContinuousStatesChanged = fmu->eventInfo.valuesOfContinuousStatesChanged;
	eventInfo->nextEventTimeDefined = fmu->eventInfo.nextEventTimeDefined;
	eventInfo->nextEventTime = fmu->eventInfo.nextEventTime;


	return fmi2OK;
} // ????


FMI2_Export fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c)
{
	ModelInstance *fmu = (ModelInstance*) c;

	fmu->state = modelContinuousTimeMode;

	// from fmiEventUpdate
  	//fmu->eventInfo.iterationConverged = fmi2True;
  	//fmu->eventInfo.upcomingTimeEvent = fmi2False;
  	fmu->eventInfo.terminateSimulation = fmi2False;


	return fmi2OK;
}


FMI2_Export fmi2Status fmi2CompletedIntegratorStep(fmi2Component c,
						   fmi2Boolean noSetFMUStatePriorToCurrentPoint,
						   fmi2Boolean* enterEventMode,
						   fmi2Boolean* terminateSimulation)
{
        ModelInstance* fmu = (ModelInstance*) c;
	if ( (fmu->time) >= (fmu->rvar[ts_]) ) {
		fmu->rvar[sgn_] = -1;
		*enterEventMode = fmi2False;
	} else {
	        *enterEventMode = fmi2False;
		fmu->rvar[sgn_] = 1;
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
	fmi2Real y = fmu->rvar[x_];
	fmi2Real k = fmu->rvar[k_];
	fmi2Real sgn = fmu->rvar[sgn_];
	derivatives[0] = fmu->rvar[der_x_] = sgn*y*( 1-y )*k;

	return fmi2OK;
}


FMI2_Export fmi2Status fmi2GetEventIndicators( fmi2Component c, fmi2Real eventIndicators[], size_t ni )
{
        ModelInstance* fmu = (ModelInstance*) c;
	if ( (fmu->time) < (fmu->rvar[ts_]) ){
		eventIndicators[0] =  1;
        }else {
		eventIndicators[0] = -1;
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


//*********************** functions from version 1.0 which are not used any more ***********



/*

FMI2_Export fmiStatus fmiEventUpdate( fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo* eventInfo )
{
        //ModelInstance* fmu = (ModelInstance*) c;

  	eventInfo->iterationConverged = fmiTrue;
  	eventInfo->upcomingTimeEvent = fmiFalse;
  	eventInfo->terminateSimulation = fmiFalse;

	return fmiOK;
}



		



FMI2_Export fmiStatus fmiGetStateValueReferences( fmiComponent c, fmiValueReference vrx[], size_t nx )
{
	//ModelInstance* fmu = (ModelInstance*) c;
	size_t i;
	for( i = 0; i < nx; i++ )
		vrx[i] = fmiUndefinedValueReference;

	return fmiOK;
}

*/
