/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#include "fmiFunctions.h"
#include "FMIComponentFrontEnd.h"


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
	FMIComponentFrontEnd* fe = static_cast<FMIComponentFrontEnd*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->getReal( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}



fmiStatus fmiGetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[] )
{
	return fmiFatal;
}



fmiStatus fmiGetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[] )
{
	return fmiFatal;
}



fmiStatus fmiGetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString value[] )
{
	return fmiFatal;
}



fmiStatus fmiSetReal( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[] )
{
	FMIComponentFrontEnd* fe = static_cast<FMIComponentFrontEnd*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != fe->setReal( vr[i], value[i] ) ) result = fmiWarning;
	}

	return result;
}



fmiStatus fmiSetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[] )
{
	return fmiFatal;
}



fmiStatus fmiSetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[] )
{
	return fmiFatal;
}



fmiStatus fmiSetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString value[] )
{
	return fmiFatal;
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
	return static_cast<fmiComponent>( new FMIComponentFrontEnd( instanceName, fmuGUID,
								    fmuLocation, mimeType,
								    timeout, visible ) );
}



fmiStatus fmiInitializeSlave( fmiComponent c, fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop )
{
	FMIComponentFrontEnd* fe = static_cast<FMIComponentFrontEnd*>( c );
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
	FMIComponentFrontEnd* fe = static_cast<FMIComponentFrontEnd*>( c );
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
	FMIComponentFrontEnd* fe = static_cast<FMIComponentFrontEnd*>( c );

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
