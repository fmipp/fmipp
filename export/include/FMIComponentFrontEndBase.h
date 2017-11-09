// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMICOMPONENTFRONTENDBASE_H
#define _FMIPP_FMICOMPONENTFRONTENDBASE_H

#include <string>

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

	virtual fmi2Status setReal( const fmi2ValueReference& ref, const fmi2Real& val ) = 0;
	virtual fmi2Status setInteger( const fmi2ValueReference& ref, const fmi2Integer& val ) = 0;
	virtual fmi2Status setBoolean( const fmi2ValueReference& ref, const fmi2Boolean& val ) = 0;
	virtual fmi2Status setString( const fmi2ValueReference& ref, const fmi2String& val ) = 0;

	virtual fmi2Status getReal( const fmi2ValueReference& ref, fmi2Real& val ) = 0;
	virtual fmi2Status getInteger( const fmi2ValueReference& ref, fmi2Integer& val ) = 0;
	virtual fmi2Status getBoolean( const fmi2ValueReference& ref, fmi2Boolean& val ) = 0;
	virtual fmi2Status getString( const fmi2ValueReference& ref, fmi2String& val ) = 0;

	virtual fmi2Status getDirectionalDerivative( const fmi2ValueReference vUnknown_ref[],
		size_t nUnknown, const fmi2ValueReference vKnown_ref[], size_t nKnown,
		const fmi2Real dvKnown[], fmi2Real dvUnknown[] ) = 0;

	//
	//  Functions specific for FMI for Co-simulation.
	//

	/// For FMI Version 1.x.
	virtual fmi2Status instantiateSlave( const std::string& instanceName, const std::string& fmuGUID,
		const std::string& fmuLocation, fmi2Real timeout, fmi2Boolean visible ) = 0;

	/// For FMI Version 2.x.
	virtual fmi2Status instantiate( const std::string& instanceName, const std::string& fmuGUID,
		const std::string& fmuResourceLocation, fmi2Boolean visible ) = 0;

	virtual fmi2Status initializeSlave( fmi2Real tStart, fmi2Boolean StopTimeDefined, fmi2Real tStop ) = 0;

	virtual fmi2Status resetSlave() = 0;

	virtual fmi2Status setRealInputDerivatives( const fmi2ValueReference vr[], size_t nvr,
						const fmi2Integer order[], const fmi2Real value[]) = 0;
	virtual fmi2Status getRealOutputDerivatives( const fmi2ValueReference vr[], size_t nvr,
						const fmi2Integer order[], fmi2Real value[]) = 0;

	virtual fmi2Status doStep( fmi2Real comPoint, fmi2Real stepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint ) = 0;
	virtual fmi2Status cancelStep() = 0;

	virtual fmi2Status getStatus( const fmi2StatusKind s, fmi2Status* value ) = 0;
	virtual fmi2Status getRealStatus( const fmi2StatusKind s, fmi2Real* value ) = 0;
	virtual fmi2Status getIntegerStatus( const fmi2StatusKind s, fmi2Integer* value ) = 0;
	virtual fmi2Status getBooleanStatus( const fmi2StatusKind s, fmi2Boolean* value ) = 0;
	virtual fmi2Status getStringStatus( const fmi2StatusKind s, fmi2String* value ) = 0;


	//
	//  Optional functions for manipulating the FMU state.
	//

	virtual fmi2Status getFMUState( fmi2FMUstate* fmuState ) = 0;
	virtual fmi2Status setFMUState( fmi2FMUstate fmuState )	= 0;
	virtual fmi2Status freeFMUState( fmi2FMUstate* fmuState ) = 0;
	virtual fmi2Status serializedFMUStateSize( fmi2FMUstate fmuState, size_t* size ) = 0;
	virtual fmi2Status serializeFMUState( fmi2FMUstate fmuState, fmi2Byte serializedState[], size_t size ) = 0;
	virtual fmi2Status deserializeFMUState( const fmi2Byte serializedState[], size_t size, fmi2FMUstate* fmuState ) = 0;

	//
	// Handle callback functions and logging verbosity.
	//

	/// Set internal pointer to callback functions (FMI 1.0, backward compatibility).
	bool setCallbackFunctions( cs::fmiCallbackFunctions* functions );

	/// Set internal pointer to callback functions (FMI 1.0, backward compatibility).
	bool setCallbackFunctions( fmi2::fmi2CallbackFunctions* functions );

	/// Set internal debug flag.
	void setDebugFlag( fmi2Boolean loggingOn );

	/// Send a message to FMU logger.
	virtual void logger( fmi2Status status, const std::string& category, const std::string& msg ) = 0;

	/// Get MIME type (FMI 1.0 compatibility).
	virtual const std::string getMIMEType() const = 0;
	
protected:

	/// Call the user-supplied function "stepFinished(...)".
	void callStepFinished( fmi2Status status );

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
		std::string& preArguments,
		std::string& mainArguments,
		std::string& postArguments,
		std::string& executableURI,
		std::string& entryPointURI ) const;
		

	/** Copy additional input files (specified in XML description elements
	 *  of type  "Implementation.CoSimulation_Tool.Model.File").
	 **/
	bool copyAdditionalInputFiles( const ModelDescription* description,
		const std::string& fmuLocation );

	/// Internal pointer to callback functions (FMI 1.0, backward compatibility).
	cs::fmiCallbackFunctions* fmiFunctions_;

	/// Internal pointer to callback functions (FMI 2.0).
	fmi2::fmi2CallbackFunctions* fmi2Functions_;

	/// Flag indicating that debug logging is enabled.
	fmi2Boolean loggingOn_;

};


#endif // _FMIPP_FMICOMPONENTFRONTENDBASE_H
