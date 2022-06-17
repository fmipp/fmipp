// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMICOMPONENTFRONTENDBASE_H
#define _FMIPP_FMICOMPONENTFRONTENDBASE_H

// General FMI++ configuration.
#include "common/FMIPPConfig.h"

// FMI 2.0 specification.
#include "common/fmi_v2.0/fmi2ModelTypes.h"
#include "common/fmi_v2.0/fmi_2.h"

// FMI 1.0 specification (for backward compatibility)
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/fmi_v1.0/fmi_cs.h"

class ModelDescription;

/**
 * \file FMIComponentFrontEndBase.h
 * \class FMIComponentFrontEndBase FMIComponentFrontEndBase.h
 * The front end component is the actual gateway for a master algorithm to communicate and exchange
 * data with an external simulation application.
 *
 * This abstract interface is designed such that it can be easily used to implement an actual
 * FMI component (FMI model type fmiComponent), implementing functionalities close to the
 * requirements of the FMI specification, e.g., functions initializeSlave(...), doStep(...) or setReal(...).
 */

class __FMI_DLL FMIComponentFrontEndBase
{

public:

	/// Constructor.
	FMIComponentFrontEndBase();

	/// Destructor.
	virtual ~FMIComponentFrontEndBase();

	//
	//  Functions for data exchange.
	//

	virtual fmippStatus setReal( const fmippValueReference& ref, const fmippReal& val ) = 0;
	virtual fmippStatus setInteger( const fmippValueReference& ref, const fmippInteger& val ) = 0;
	virtual fmippStatus setBoolean( const fmippValueReference& ref, const fmippBoolean& val ) = 0;
	virtual fmippStatus setString( const fmippValueReference& ref, const fmippChar* val ) = 0;

	virtual fmippStatus getReal( const fmippValueReference& ref, fmippReal& val ) = 0;
	virtual fmippStatus getInteger( const fmippValueReference& ref, fmippInteger& val ) = 0;
	virtual fmippStatus getBoolean( const fmippValueReference& ref, fmippBoolean& val ) = 0;
	virtual fmippStatus getString( const fmippValueReference& ref, const fmippChar*& val ) = 0;

	virtual fmippStatus getDirectionalDerivative( const fmippValueReference vUnknown_ref[],
		size_t nUnknown, const fmippValueReference vKnown_ref[], size_t nKnown,
		const fmippReal dvKnown[], fmippReal dvUnknown[] ) = 0;

	//
	//  Functions specific for FMI for Co-simulation.
	//

	/// For FMI Version 1.x.
	virtual fmippStatus instantiateSlave( const fmippString& instanceName, const fmippString& fmuGUID,
		const fmippString& fmuLocation, fmippTime timeout, fmippBoolean visible ) = 0;

	/// For FMI Version 2.x.
	virtual fmippStatus instantiate( const fmippString& instanceName, const fmippString& fmuGUID,
		const fmippString& fmuResourceLocation, fmippBoolean visible ) = 0;

	virtual fmippStatus initializeSlave( fmippTime tStart, fmippBoolean StopTimeDefined, fmippTime tStop ) = 0;

	virtual fmippStatus resetSlave() = 0;

	virtual fmippStatus setRealInputDerivatives( const fmippValueReference vr[], size_t nvr,
						const fmippInteger order[], const fmippReal value[]) = 0;
	virtual fmippStatus getRealOutputDerivatives( const fmippValueReference vr[], size_t nvr,
						const fmippInteger order[], fmippReal value[]) = 0;

	virtual fmippStatus doStep( fmippTime comPoint, fmippTime stepSize, fmippBoolean noSetFMUStatePriorToCurrentPoint ) = 0;
	virtual fmippStatus cancelStep() = 0;

	virtual fmippStatus getStatus( const fmippStatusKind s, fmippStatus* value ) = 0;
	virtual fmippStatus getRealStatus( const fmippStatusKind s, fmippReal* value ) = 0;
	virtual fmippStatus getIntegerStatus( const fmippStatusKind s, fmippInteger* value ) = 0;
	virtual fmippStatus getBooleanStatus( const fmippStatusKind s, fmippBoolean* value ) = 0;
	virtual fmippStatus getStringStatus( const fmippStatusKind s, const fmippChar* value ) = 0;

	//
	//  Optional functions for manipulating the FMU state.
	//

	virtual fmippStatus getFMUState( fmippFMUState* fmuState ) = 0;
	virtual fmippStatus setFMUState( fmippFMUState fmuState )	= 0;
	virtual fmippStatus freeFMUState( fmippFMUState* fmuState ) = 0;
	virtual fmippStatus serializedFMUStateSize( fmippFMUState fmuState, size_t* size ) = 0;
	virtual fmippStatus serializeFMUState( fmippFMUState fmuState, fmippByte serializedState[], size_t size ) = 0;
	virtual fmippStatus deserializeFMUState( const fmippByte serializedState[], size_t size, fmippFMUState* fmuState ) = 0;

	//
	// Handle callback functions and logging verbosity.
	//

	/// Set internal pointer to callback functions (FMI 1.0, backward compatibility).
	bool setCallbackFunctions( cs::fmiCallbackFunctions* functions );

	/// Set internal pointer to callback functions (FMI 1.0, backward compatibility).
	bool setCallbackFunctions( fmi2::fmi2CallbackFunctions* functions );

	/// Set internal debug flag.
	void setDebugFlag( fmippBoolean loggingOn );

	/// Send a message to FMU logger.
	virtual void logger( fmippStatus status, const fmippString& category, const fmippString& msg ) = 0;

	/// Get MIME type (FMI 1.0 compatibility).
	virtual const fmippString getMIMEType() const = 0;

protected:

	/// Call the user-supplied function "stepFinished(...)".
	void callStepFinished( fmippStatus status );

	/** Check for additional command line arguments (as part of optional vendor
	 *  annotations). Get command line arguments that are supposed to come
	 *  between the applications name and the main input file (entry point).
	 *  Get command line arguments that are supposed to come after the main
	 *  input file (entry point). Get explicit path to the executable (URI).
	 *  A main argument can be specified, which should then be used instead
	 *  of just the filename as main command line argument when starting the
	 *  external application.
	 **/
	bool parseAdditionalArguments( const ModelDescription* description,
		fmippString& preArguments,
		fmippString& mainArguments,
		fmippString& postArguments,
		fmippString& executableURI,
		fmippString& entryPointURI ) const;

	/** Copy additional input files (specified in XML description elements
	 *  of type  "Implementation.CoSimulation_Tool.Model.File").
	 **/
	bool copyAdditionalInputFiles( const ModelDescription* description,
		const fmippString& fmuLocation );

	/// Internal pointer to callback functions (FMI 1.0, backward compatibility).
	cs::fmiCallbackFunctions* fmiFunctions_;

	/// Internal pointer to callback functions (FMI 2.0).
	fmi2::fmi2CallbackFunctions* fmi2Functions_;

	/// Flag indicating that debug logging is enabled.
	fmippBoolean loggingOn_;

};

#endif // _FMIPP_FMICOMPONENTFRONTENDBASE_H
