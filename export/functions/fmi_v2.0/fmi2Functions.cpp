// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file fmi2Functions.cpp
 * Compile this file in order to generate an FMU CS (Version 2.0) that uses some instance derived
 * from class FMIComponentFrontEndBase.
 *
 * When compiling, define the following macros accordingly:
 *  - FRONT_END_TYPE: class name of the derived instance
 *  - FRONT_END_TYPE_INCLUDE: header file of the class of the derived instance
 *
 * Example (for GCC): -DFRONT_END_TYPE=FMIComponentFrontEnd -DFRONT_END_TYPE_INCLUDE="FMIComponentFrontEnd.h"
 */

#include <iostream>

#include "fmi2Functions.h"
#include FRONT_END_TYPE_INCLUDE

/***************************************************
	Common Functions
****************************************************/

const char* fmi2GetTypesPlatform()
{
	return fmi2TypesPlatform;
}

const char* fmi2GetVersion()
{
	return fmi2Version;
}

fmi2Status  fmi2SetDebugLogging( fmi2Component c,
		   fmi2Boolean loggingOn,
		   size_t nCategories,
		   const fmi2String categories[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	fe->setDebugFlag( loggingOn );
	return fmi2OK;
}

fmi2Component fmi2Instantiate( fmi2String instanceName,
		  fmi2Type fmuType,
		  fmi2String fmuGUID,
		  fmi2String fmuResourceLocation,
		  const fmi2CallbackFunctions* functions,
		  fmi2Boolean visible,
		  fmi2Boolean loggingOn )
{
	FMIComponentFrontEndBase* fe = new FRONT_END_TYPE;

	// FMI++ internally defines the callback functions as non-const.
	// This is a dirty workaround that makes things work anyway ...
	fmi2::fmi2CallbackFunctions* callbacks = new fmi2::fmi2CallbackFunctions;
	callbacks->logger = functions->logger;
	callbacks->allocateMemory = functions->allocateMemory;
	callbacks->freeMemory = functions->freeMemory;
	callbacks->stepFinished = functions->stepFinished;
	callbacks->componentEnvironment = functions->componentEnvironment;

	if ( false == fe->setCallbackFunctions( callbacks ) ) {
		delete callbacks;
		delete fe;
		return 0;
	}

	delete callbacks;

	fe->setDebugFlag( loggingOn );

	if ( fmi2OK != static_cast<fmi2Status>( fe->instantiate( instanceName, fmuGUID, fmuResourceLocation, visible ) ) ) {
		delete fe;
		return 0;
	}

	return static_cast<fmi2Component>( fe );
}

void fmi2FreeInstance( fmi2Component c )
{
	if ( 0 == c ) return;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	delete fe;
	return;
}

fmi2Status fmi2SetupExperiment( fmi2Component c,
		   fmi2Boolean toleranceDefined,
		   fmi2Real tolerance,
		   fmi2Real startTime,
		   fmi2Boolean stopTimeDefined,
		   fmi2Real stopTime )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->initializeSlave( startTime, stopTimeDefined, stopTime ) );
}

fmi2Status fmi2EnterInitializationMode( fmi2Component c )
{
	return fmi2OK;
}

fmi2Status fmi2ExitInitializationMode( fmi2Component c )
{
	return fmi2OK;
}

fmi2Status fmi2Terminate( fmi2Component c )
{
	return fmi2OK; // Nothing to be done here?
}

fmi2Status fmi2Reset( fmi2Component c )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->resetSlave() );
}

fmi2Status fmi2GetReal( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr, fmi2Real value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmi2Status result = fmi2OK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmi2OK != static_cast<fmi2Status>( fe->getReal( vr[i], value[i] ) ) ) result = fmi2Warning;
	}

	return result;
}

fmi2Status fmi2GetInteger( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr, fmi2Integer value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmi2Status result = fmi2OK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmi2OK != static_cast<fmi2Status>( fe->getInteger( vr[i], value[i] ) ) ) result = fmi2Warning;
	}

	return result;
}

fmi2Status fmi2GetBoolean( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr, fmi2Boolean value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmi2Status result = fmi2OK;
	fmippBoolean val;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmi2OK != static_cast<fmi2Status>( fe->getBoolean( vr[i], val ) ) ) result = fmi2Warning;
		value[i] = static_cast<fmi2Boolean>( val );
	}

	return result;
}

fmi2Status fmi2GetString( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr, fmi2String value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmi2Status result = fmi2OK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmi2OK != static_cast<fmi2Status>( fe->getString( vr[i], value[i] ) ) ) {
			result = fmi2Warning;
		}
	}

	return result;
}

fmi2Status fmi2SetReal( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr,
	const fmi2Real value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmi2Status result = fmi2OK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmi2OK != static_cast<fmi2Status>( fe->setReal( vr[i], value[i] ) ) ) result = fmi2Warning;
	}

	return result;
}

fmi2Status fmi2SetInteger( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr,
	const fmi2Integer value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmi2Status result = fmi2OK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmi2OK != static_cast<fmi2Status>( fe->setInteger( vr[i], value[i] ) ) ) result = fmi2Warning;
	}

	return result;
}

fmi2Status fmi2SetBoolean( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr,
	const fmi2Boolean value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmi2Status result = fmi2OK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmi2OK != static_cast<fmi2Status>( fe->setBoolean( vr[i], value[i] ) ) ) result = fmi2Warning;
	}

	return result;
}

fmi2Status fmi2SetString( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr,
	const fmi2String value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmi2Status result = fmi2OK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmi2OK != static_cast<fmi2Status>( fe->setString( vr[i], value[i] ) ) ) result = fmi2Warning;
	}

	return result;
}

fmi2Status fmi2GetFMUstate( fmi2Component c, fmi2FMUstate* fmuState )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->getFMUState( fmuState ) );
}

fmi2Status fmi2SetFMUstate( fmi2Component c, fmi2FMUstate fmuState )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->setFMUState( fmuState ) );
}

fmi2Status fmi2FreeFMUstate( fmi2Component c, fmi2FMUstate* fmuState )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->freeFMUState( fmuState ) );
}

fmi2Status fmi2SerializedFMUstateSize( fmi2Component c,
	fmi2FMUstate fmuState,
	size_t* size )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->serializedFMUStateSize( fmuState, size ) );
}

fmi2Status fmi2SerializeFMUstate( fmi2Component c,
	fmi2FMUstate fmuState,
	fmi2Byte serializedState[],
	size_t size )
{
	if ( 0 == c ) return fmi2Fatal;

	fmippByte* state = new fmippByte[size];
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	fmi2Status status = static_cast<fmi2Status>( fe->serializeFMUState( fmuState, state, size ) );

	for ( size_t i = 0; i < size; ++i ) {
		serializedState[i] = static_cast<fmi2Byte>( state[i] );
	}

	delete state;
	return status;
}

fmi2Status fmi2DeSerializeFMUstate( fmi2Component c,
	const fmi2Byte serializedState[],
	size_t size,
	fmi2FMUstate* fmuState )
{
	if ( 0 == c ) return fmi2Fatal;

	fmippByte* state = new fmippByte[size];
	for ( size_t i = 0; i < size; ++i ) {
		state[i] = static_cast<fmi2Byte>( serializedState[i] );
	}

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	fmi2Status status = static_cast<fmi2Status>( fe->deserializeFMUState( state, size, fmuState ) );

	delete state;
	return status;
}

fmi2Status fmi2GetDirectionalDerivative( fmi2Component c,
	const fmi2ValueReference vUnknown_ref[],
	size_t nUnknown,
	const fmi2ValueReference vKnown_ref[],
	size_t nKnown,
	const fmi2Real dvKnown[],
	fmi2Real dvUnknown[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->getDirectionalDerivative( vUnknown_ref, nUnknown, vKnown_ref, nKnown, dvKnown, dvUnknown ) );
}

/**********************************************************
	Functions for FMI2 for Model Exchange (not used here).
***********************************************************/

fmi2Status fmi2EnterEventMode( fmi2Component c )
{
	return fmi2Fatal;
}

fmi2Status fmi2NewDiscreteStates( fmi2Component c, fmi2EventInfo* fmi2EventInfo )
{
	return fmi2Fatal;
}

fmi2Status fmi2EnterContinuousTimeMode( fmi2Component c )
{
	return fmi2Fatal;
}

fmi2Status fmi2CompletedIntegratorStep( fmi2Component c,
			  fmi2Boolean noSetFMUStatePriorToCurrentPoint,
			  fmi2Boolean* enterEventMode,
			  fmi2Boolean* terminateSimulation )
{
	return fmi2Fatal;
}

fmi2Status fmi2SetTime( fmi2Component c, fmi2Real time )
{
	return fmi2Fatal;
}

fmi2Status fmi2SetContinuousStates( fmi2Component c, const fmi2Real x[], size_t nx )
{
	return fmi2Fatal;
}

fmi2Status fmi2GetDerivatives( fmi2Component c, fmi2Real derivatives[], size_t nx )
{
	return fmi2Fatal;
}

fmi2Status fmi2GetEventIndicators( fmi2Component c, fmi2Real eventIndicators[], size_t ni )
{
	return fmi2Fatal;
}

fmi2Status fmi2GetContinuousStates( fmi2Component c, fmi2Real x[], size_t nx )
{
	return fmi2Fatal;
}

fmi2Status fmi2GetNominalsOfContinuousStates( fmi2Component c, fmi2Real x_nominal[], size_t nx )
{
	return fmi2Fatal;
}

/***************************************************
	Functions for FMI2 for Co-Simulation
****************************************************/

fmi2Status fmi2SetRealInputDerivatives( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr,
	const fmi2Integer order[],
	const fmi2Real value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->setRealInputDerivatives( vr, nvr, order, value ) );
}

fmi2Status fmi2GetRealOutputDerivatives( fmi2Component c,
	const fmi2ValueReference vr[],
	size_t nvr,
	const fmi2Integer order[],
	fmi2Real value[] )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->getRealOutputDerivatives( vr, nvr, order, value ) );
}

fmi2Status fmi2DoStep( fmi2Component c,
			  fmi2Real currentCommunicationPoint,
			  fmi2Real communicationPointStepSize,
			  fmi2Boolean noSetFMUStatePriorToCurrentPoint )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->doStep( currentCommunicationPoint, communicationPointStepSize, noSetFMUStatePriorToCurrentPoint ) );
}

fmi2Status fmi2CancelStep ( fmi2Component c )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->cancelStep() );
}

fmi2Status fmi2GetStatus( fmi2Component c, const fmi2StatusKind s, fmi2Status* value )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	fmippStatus val;

	fmi2Status status = static_cast<fmi2Status>( fe->getStatus( static_cast<fmippStatusKind>( s ), &val ) );
	
	*value = static_cast<fmi2Status>( val );
	return status;
}

fmi2Status fmi2GetRealStatus( fmi2Component c, const fmi2StatusKind s, fmi2Real* value )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->getRealStatus( static_cast<fmippStatusKind>( s ), value ) );
}

fmi2Status fmi2GetIntegerStatus( fmi2Component c, const fmi2StatusKind s, fmi2Integer* value )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->getIntegerStatus( static_cast<fmippStatusKind>( s ), value ) );
}

fmi2Status fmi2GetBooleanStatus( fmi2Component c, const fmi2StatusKind s, fmi2Boolean* value )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmippBoolean* val;
	fmi2Status status = static_cast<fmi2Status>( fe->getBooleanStatus( static_cast<fmippStatusKind>( s ), val ) );
	*value = static_cast<fmi2Boolean>( *val );

	return status;
}

fmi2Status fmi2GetStringStatus( fmi2Component c, const fmi2StatusKind s, fmi2String* value )
{
	if ( 0 == c ) return fmi2Fatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmi2Status>( fe->getStringStatus( static_cast<fmippStatusKind>( s ), static_cast<const fmippChar*>( *value ) ) );
}
