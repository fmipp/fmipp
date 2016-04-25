/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMICOMPONENTFRONTENDBASE_H
#define _FMIPP_FMICOMPONENTFRONTENDBASE_H

#include <string>

#include "common/FMIPPConfig.h"
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

	virtual fmiStatus setReal( const fmiValueReference& ref, const fmiReal& val ) = 0;
	virtual fmiStatus setInteger( const fmiValueReference& ref, const fmiInteger& val ) = 0;
	virtual fmiStatus setBoolean( const fmiValueReference& ref, const fmiBoolean& val ) = 0;
	virtual fmiStatus setString( const fmiValueReference& ref, const fmiString& val ) = 0;

	virtual fmiStatus getReal( const fmiValueReference& ref, fmiReal& val ) = 0;
	virtual fmiStatus getInteger( const fmiValueReference& ref, fmiInteger& val ) = 0;
	virtual fmiStatus getBoolean( const fmiValueReference& ref, fmiBoolean& val ) = 0;
	virtual fmiStatus getString( const fmiValueReference& ref, fmiString& val ) = 0;


	//
	//  Functions specific for FMI for Co-simulation.
	//

	virtual fmiStatus instantiateSlave( const std::string& instanceName, const std::string& fmuGUID,
					    const std::string& fmuLocation, const std::string& mimeType,
					    fmiReal timeout, fmiBoolean visible ) = 0;
	virtual fmiStatus initializeSlave( fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop ) = 0;
	//virtual fmiStatus terminateSlave() = 0; // NOT NEEDED HERE? -> fmiFunctions.cpp
	virtual fmiStatus resetSlave() = 0;
	//virtual fmiStatus freeSlaveInstance() = 0; // NOT NEEDED HERE? -> fmiFunctions.cpp

	virtual fmiStatus setRealInputDerivatives( const fmiValueReference vr[], size_t nvr,
						   const fmiInteger order[], const fmiReal value[]) = 0;
	virtual fmiStatus getRealOutputDerivatives( const fmiValueReference vr[], size_t nvr,
						    const fmiInteger order[], fmiReal value[]) = 0;

	virtual fmiStatus doStep( fmiReal comPoint, fmiReal stepSize, fmiBoolean newStep ) = 0;
	virtual fmiStatus cancelStep() = 0;

	virtual fmiStatus getStatus( const fmiStatusKind s, fmiStatus* value ) = 0;
	virtual fmiStatus getRealStatus( const fmiStatusKind s, fmiReal* value ) = 0;
	virtual fmiStatus getIntegerStatus( const fmiStatusKind s, fmiInteger* value ) = 0;
	virtual fmiStatus getBooleanStatus( const fmiStatusKind s, fmiBoolean* value ) = 0;
	virtual fmiStatus getStringStatus( const fmiStatusKind s, fmiString* value ) = 0;


	//
	// Handle callback functions and logging verbosity.
	//

	/// Set internal pointer to callback functions.
	bool setCallbackFunctions( cs::fmiCallbackFunctions* functions );

	/// Set internal debug flag.
	void setDebugFlag( fmiBoolean loggingOn );

	/// Send a message to FMU logger.
	virtual void logger( fmiStatus status, const std::string& category, const std::string& msg ) = 0;

protected:

	/// Call the user-supplied function "stepFinished(...)".
	void callStepFinished( fmiStatus status );

	/** A file URI may start with "fmu://". In that case the
	 *  FMU's location has to be prepended to the URI accordingly.
	 **/
	void processURI( std::string& uri, const std::string& fmuLocation ) const;

	/** Check for additional command line arguments (as part of optional vendor
	 *  annotations). Get command line arguments that are supposed to come
	 *  between the applications name and the main input file (entry point).
	 *  Get command line arguments that are supposed to come after the main
	 *  input file (entry point). Get explicit path to the executable (URI).
	 *  A main argument can be specified, which should then be used instead
	 *  of just the filename as main command line argument when starting the
	 *  external application.
	 **/
	void parseAdditionalArguments( const ModelDescription* description,
				       std::string& preArguments,
					   std::string& mainArguments,
				       std::string& postArguments,
				       std::string& executableURI ) const;


	/** Copy additional input files (specified in XML description elements
	 *  of type  "Implementation.CoSimulation_Tool.Model.File").
	 **/
	bool copyAdditionalInputFiles( const ModelDescription* description,
				       const std::string& fmuLocation );

	/// Internal pointer to callback functions.
	cs::fmiCallbackFunctions* functions_;

	/// Flag indicating that debug logging is enabled.
	fmiBoolean loggingOn_;

};


#endif // _FMIPP_FMICOMPONENTFRONTENDBASE_H
