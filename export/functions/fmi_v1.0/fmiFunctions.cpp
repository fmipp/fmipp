// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

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
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	fe->setDebugFlag( loggingOn );
	return fmiOK;
}

/* Data Exchange Functions*/

fmiStatus fmiGetReal( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != static_cast<fmiStatus>( fe->getReal( vr[i], value[i] ) ) ) result = fmiWarning;
	}

	return result;
}

fmiStatus fmiGetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != static_cast<fmiStatus>( fe->getInteger( vr[i], value[i] ) ) ) result = fmiWarning;
	}

	return result;
}

fmiStatus fmiGetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;
	fmippBoolean val;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != static_cast<fmiStatus>( fe->getBoolean( vr[i], val ) ) ) result = fmiWarning;
		value[i] = static_cast<fmiBoolean>( val );
	}

	return result;
}

fmiStatus fmiGetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != static_cast<fmiStatus>( fe->getString( vr[i], value[i] ) ) ) {
			result = fmiWarning;
		}
	}

	return result;
}

fmiStatus fmiSetReal( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != static_cast<fmiStatus>( fe->setReal( vr[i], value[i] ) ) ) result = fmiWarning;
	}

	return result;
}

fmiStatus fmiSetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != static_cast<fmiStatus>( fe->setInteger( vr[i], value[i] ) ) ) result = fmiWarning;
	}

	return result;
}

fmiStatus fmiSetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != static_cast<fmiStatus>( fe->setBoolean( vr[i], value[i] ) ) ) result = fmiWarning;
	}

	return result;
}

fmiStatus fmiSetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmiStatus result = fmiOK;

	for ( size_t i = 0; i < nvr; ++i )
	{
		if ( fmiOK != static_cast<fmiStatus>( fe->setString( vr[i], value[i] ) ) ) result = fmiWarning;
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

	fe->setDebugFlag( ( fmiTrue == loggingOn ) ? fmippTrue : fmippFalse );

	fmiStatus status = static_cast<fmiStatus>( fe->instantiateSlave( instanceName, fmuGUID, fmuLocation, timeout, visible ) );

	// Check if MIME type is consistent.
	if ( fe->getMIMEType() != mimeType ) {
		std::string warning = std::string( "Wrong MIME type: " ) + mimeType
			+ std::string( " --- expected: " ) + fe->getMIMEType();
		fe->logger( fmippWarning, "MIME-TYPE", warning );
	}

	if ( fmiOK != status ) {
		delete fe;
		return 0;
	}

	return static_cast<fmiComponent>( fe );
}

fmiStatus fmiInitializeSlave( fmiComponent c, fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmiStatus>( fe->initializeSlave( tStart, StopTimeDefined, tStop ) );
}

fmiStatus fmiTerminateSlave( fmiComponent c )
{
	return fmiOK; // Nothing to be done here?
}

fmiStatus fmiResetSlave( fmiComponent c )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmiStatus>( fe->resetSlave() );
}

void fmiFreeSlaveInstance( fmiComponent c )
{
	if ( 0 == c ) return;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	delete fe;
	return;
}

fmiStatus fmiSetRealInputDerivatives( fmiComponent c, const  fmiValueReference vr[],
				      size_t nvr, const fmiInteger order[], const fmiReal value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmiStatus>( fe->setRealInputDerivatives( vr, nvr, order, value ) );
}

fmiStatus fmiGetRealOutputDerivatives( fmiComponent c, const fmiValueReference vr[],
				       size_t nvr, const fmiInteger order[], fmiReal value[] )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmiStatus>( fe->getRealOutputDerivatives( vr, nvr, order, value ) );
}

fmiStatus fmiCancelStep( fmiComponent c )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmiStatus>( fe->cancelStep() );
}

fmiStatus fmiDoStep( fmiComponent c, fmiReal currentCommunicationPoint,
		     fmiReal communicationStepSize, fmiBoolean newStep )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmiStatus>( fe->doStep( currentCommunicationPoint, communicationStepSize, static_cast<fmippBoolean>( newStep ) ) );
}

fmiStatus fmiGetStatus( fmiComponent c, const fmiStatusKind s, fmiStatus* value )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmippStatus* val = 0;
	fmiStatus status = static_cast<fmiStatus>( fe->getStatus( static_cast<const fmippStatusKind>( s ), val ) );
	*value = static_cast<fmiStatus>( *val );

	return status;
}

fmiStatus fmiGetRealStatus( fmiComponent c, const fmiStatusKind s, fmiReal* value )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmiStatus>( fe->getRealStatus( static_cast<const fmippStatusKind>( s ), value ) );
}

fmiStatus fmiGetIntegerStatus( fmiComponent c, const fmiStatusKind s, fmiInteger* value )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmiStatus>( fe->getIntegerStatus( static_cast<const fmippStatusKind>( s ), value ) );
}

fmiStatus fmiGetBooleanStatus( fmiComponent c, const fmiStatusKind s, fmiBoolean* value )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );

	fmippBoolean* val = 0;
	fmiStatus status = static_cast<fmiStatus>( fe->getBooleanStatus( static_cast<const fmippStatusKind>( s ), val ) );
	*value = static_cast<fmiBoolean>( *val );

	return status;
}

fmiStatus fmiGetStringStatus( fmiComponent c, const fmiStatusKind s, fmiString*  value )
{
	if ( 0 == c ) return fmiFatal;

	FMIComponentFrontEndBase* fe = static_cast<FMIComponentFrontEndBase*>( c );
	return static_cast<fmiStatus>( fe->getStringStatus( static_cast<fmippStatusKind>( s ), static_cast<const fmippChar*>( *value ) ) );
}
