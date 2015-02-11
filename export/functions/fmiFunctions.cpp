/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file fmiFunctions.cpp
 * Compile this file in order to generate an FMU CS that uses some instance derived
 * from class FMIComponentFrontEndBase.
 *
 * When compiling, define the following macros accordingly:
 *  - FRONT_END_TYPE: class name of the derived instance
 *  - FRONT_END_TYPE_INCLUDE: header file of the class of the derived instance
 *
 * Example (for GCC): -DFRONT_END_TYPE=FMIComponentFrontEnd -DFRONT_END_TYPE_INCLUDE="FMIComponentFrontEnd.h"
 */ 


#include "fmiFunctions.h"
#include FRONT_END_TYPE_INCLUDE


/***************************************************
Common Functions
****************************************************/

/* Inquire version numbers of header files */

const char* fmiGetTypesPlatform()
{
	return fmiPlatform;
}



const char* fmiGetVersion()
{
	return fmiVersion;
}



fmiStatus fmiSetDebugLogging( fmiComponent c, fmiBoolean loggingOn )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	fe->setDebugFlag( loggingOn );
	return fmiOK;
}


/* Data Exchange Functions*/

fmiStatus fmiGetReal( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->getReal( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}



fmiStatus fmiGetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->getInteger( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}



fmiStatus fmiGetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->getBoolean( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}



fmiStatus fmiGetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->getString( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}



fmiStatus fmiSetReal( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->setReal( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}



fmiStatus fmiSetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->setInteger( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}



fmiStatus fmiSetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->setBoolean( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}



fmiStatus fmiSetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->setString( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}


/***************************************************
Functions for FMI for Co-Simulation
****************************************************/

/* Creation and destruction of slave instances and setting debug status */

fmiComponent fmiInstantiateSlave( fmiString instanceName, fmiString fmuGUID,
				  fmiString fmuLocation, fmiString mimeType,
				  fmiReal timeout, fmiBoolean visible, fmiBoolean interactive,
				  fmiCallbackFunctions functions, fmiBoolean loggingOn )
{
	FMIComponentFrontEndBase* fe = new FRONT_END_TYPE;

	// The reinterpret_cast in the next line of code looks very brutal, but in the end it is just
	// a cast between exactly the same things defined at two different poistions in the code ...
	cs::fmiCallbackFunctions* callbacks = reinterpret_cast<cs::fmiCallbackFunctions*>( &functions );

	if ( false == fe->setCallbackFunctions( callbacks ) ) {
		delete fe;
		return 0;
	}

	fe->setDebugFlag( loggingOn );

	if ( fmiOK != fe->instantiateSlave( instanceName, fmuGUID, fmuLocation, mimeType, timeout, visible ) ) {
		delete fe;
		return 0;
	}

	return static_cast<fmiComponent>( fe );
}



fmiStatus fmiInitializeSlave( fmiComponent c, fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->initializeSlave( tStart, StopTimeDefined, tStop );
}



fmiStatus fmiTerminateSlave( fmiComponent c )
{
	return fmiOK; // Nothing to be done here?
}



fmiStatus fmiResetSlave( fmiComponent c )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->resetSlave();
}



void fmiFreeSlaveInstance( fmiComponent c )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	delete fe;
	return;
}



fmiStatus fmiSetRealInputDerivatives( fmiComponent c, const  fmiValueReference vr[],
				      size_t nvr, const fmiInteger order[], const fmiReal value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->setRealInputDerivatives( vr, nvr, order, value );
}



fmiStatus fmiGetRealOutputDerivatives( fmiComponent c, const fmiValueReference vr[],
				       size_t nvr, const fmiInteger order[], fmiReal value[] )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->getRealOutputDerivatives( vr, nvr, order, value );
}



fmiStatus fmiCancelStep( fmiComponent c )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->cancelStep();
}



fmiStatus fmiDoStep( fmiComponent c, fmiReal currentCommunicationPoint,
		     fmiReal communicationStepSize, fmiBoolean newStep )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->doStep( currentCommunicationPoint,
			   communicationStepSize,
			   newStep );
}



fmiStatus fmiGetStatus( fmiComponent c, const fmiStatusKind s, fmiStatus*  value )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	return fe->getStatus( s, value );
}



fmiStatus fmiGetRealStatus( fmiComponent c, const fmiStatusKind s, fmiReal* value )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->getRealStatus( s, value );
}



fmiStatus fmiGetIntegerStatus( fmiComponent c, const fmiStatusKind s, fmiInteger* value )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->getIntegerStatus( s, value );
}



fmiStatus fmiGetBooleanStatus( fmiComponent c, const fmiStatusKind s, fmiBoolean* value )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->getBooleanStatus( s, value );
}



fmiStatus fmiGetStringStatus( fmiComponent c, const fmiStatusKind s, fmiString*  value )
{
	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return fe->getStringStatus( s, value );
}
