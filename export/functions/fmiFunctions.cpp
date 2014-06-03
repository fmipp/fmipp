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
	return fmiOK; // FIXME.
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
	FMIComponentFrontEndBase* fe = 0;

	try {
		fe = create<FRONT_END_TYPE>( instanceName, fmuGUID, fmuLocation,
					     mimeType, timeout, visible );
	} catch (...) {
		/// \FIXME Call logger.
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
	return fmiFatal;
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
	return fmiFatal;
}



fmiStatus fmiGetRealOutputDerivatives( fmiComponent c, const fmiValueReference vr[],
				       size_t nvr, const fmiInteger order[], fmiReal value[] )
{
	return fmiFatal;
}



fmiStatus fmiCancelStep( fmiComponent c )
{
	return fmiFatal;
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
	return fmiFatal;
}



fmiStatus fmiGetRealStatus( fmiComponent c, const fmiStatusKind s, fmiReal* value )
{
	return fmiFatal;
}



fmiStatus fmiGetIntegerStatus( fmiComponent c, const fmiStatusKind s, fmiInteger* value )
{
	return fmiFatal;
}



fmiStatus fmiGetBooleanStatus( fmiComponent c, const fmiStatusKind s, fmiBoolean* value )
{
	return fmiFatal;
}



fmiStatus fmiGetStringStatus( fmiComponent c, const fmiStatusKind s, fmiString*  value )
{
	return fmiFatal;
}
