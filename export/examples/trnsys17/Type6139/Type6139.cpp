/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 *  \file Type6139.cpp
 *  TRNSYS back end component type.
 **/

#define _CRT_SECURE_NO_WARNINGS

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

#include "TRNSYS.h" // TRNSYS acess functions (allow to acess TIME etc.).

#include "FMIComponentBackEnd.h"
#include "HelperFunctions.h"


using namespace std;

namespace {

	FMIComponentBackEnd* backend = 0;

	int inputInterfaceUnit = -1;
	int outputInterfaceUnit = -1;

	bool backendInitialized = false;

	const double hoursToSeconds = 3600.;
}


int initializeFMIInputInterface();
int initializeFMIOutputInterface();



extern "C" __declspec(dllexport) 
int TYPE6139( double &time,  // the simulation time
	      double xin[],  // the array containing the component INPUTS
	      double xout[], // the array which the component fills with its appropriate OUTPUTS
	      double &t,     // the array containing the dependent variables for which the derivatives are evaluated 
	      double &dtdt,  // the array containing the derivatives of T which are evaluated 
	      double par[],  // the array containing the PARAMETERS of the component
	      int info[],    // the information array described in Section 3.3.3 of the manual
	      int icntrl )   // the control array described in Section 3.3.4 of the manual
{
	// Set the version number for this Type.
	if ( getIsVersionSigningTime() )
	{
		int typeVersion = 16;
		setTypeVersion( &typeVersion );
		return 1;
	}

	// Get current unit number.
	int currentUnit = getCurrentUnit();

	// Do all of the "very first call of the simulation” manipulations here.
	if ( getIsFirstCallOfSimulation() )
	{

		// If backend has not been initialized by another unit, do so now ...
		if ( 0 == backend )
		{
			backend = new FMIComponentBackEnd;
			backend->startInitialization();
		}

		// Get label describing the inteface type (from the type's special cards).
		int maxLabelLength = getMaxLabelLength();
		char* label = new char[maxLabelLength + 1];
		label[maxLabelLength] = 0;
		int iLabel = 1;
		getLabel( label, &maxLabelLength, &currentUnit, &iLabel );
		string interfaceType;
		HelperFunctions::trim( label, interfaceType );
		delete label;

		if ( 0 == interfaceType.compare( "FMI input interface" ) )
		{
			// Initialize unit as FMI input interface.
			int initResult = initializeFMIInputInterface();
			if ( 0 != initResult ) return initResult; // Return in case of errors.
		}
		else if ( 0 == interfaceType.compare( "FMI output interface" ) )
		{
			// Initialize unit as FMI output interface.
			int initResult = initializeFMIOutputInterface();
			if ( 0 != initResult ) return initResult; // Return in case of errors.
		}
		else  // Invalid type.
		{
			int errorCode = -1;
			char severity[] = "Fatal";
			int currentType = getCurrentType();

			string strMessage = string( "Unsupported interface type: '" ) + interfaceType + string( "'" );
			char *message = new char[strMessage.length() + 1];
			message[strMessage.length()] = 0;
			strcpy( message, strMessage.c_str() );

			Messages( &errorCode, message, severity,
				  &currentUnit, &currentType,
				  strlen(message), strlen(severity) );

			return 1;
		}

		// No stored variables.
		int nStoreStatic = 0;
		int nStoreDynamic = 0;
		setNumberStoredVariables( &nStoreStatic, &nStoreDynamic );

		return 1;
	}

	// Do all of the “last call” manipulations that may be required.
	if ( getIsLastCallOfSimulation() )
	{
		if ( 0 != backend ) { // Delete the backend at the last simulation call.
			delete backend;
			backend = 0; // Just to be on the safe side ...
		}

		return 1;
	}

	// Do all of the "first timestep manipulations" here - there are no iterations at the intial time.
	if ( getIsStartTime() )
	{
		// At this point in the simulation, the FMI input and/or output interfaces
		// have been initialized. Hence it is save to end the backend initialization.
		if ( false == backendInitialized )
		{
			backendInitialized = true;
			backend->enforceTimeStep( hoursToSeconds * getSimulationTimeStep() ); // TRNSYS can't do dynamic steps!
			backend->endInitialization();
			backend->waitForMaster();
		}

		return 1;
	}

	// Perform any "end of timestep manipulations" that may be required.
	if ( getIsEndOfTimestep() && ( currentUnit == outputInterfaceUnit ) )
	{
		// In case the unit is an output interface, set the current outputs for the FMU instance.
		backend->setRealOutputs( xin, static_cast<size_t>( par[0] ) ); // Set type inputs as FMU outputs.
		backend->enforceTimeStep( hoursToSeconds * getSimulationTimeStep() ); // TRNSYS can't do dynmic steps!

		// Signal to master and wait to resume execution.
		backend->signalToMaster();
		backend->waitForMaster();

		return 1;
	}

	backend->getRealInputs( xout, static_cast<size_t>( par[1] ) ); // Set FMU inputs as type outputs.

	return 1;
}


/// Initialize the FMI input interface.
int initializeFMIInputInterface()
{
	// Get current unit number.
	int currentUnit = getCurrentUnit();

	if ( -1 == inputInterfaceUnit )
	{
		inputInterfaceUnit = currentUnit; // Identify current unit as FMI input interface.

		int errorCode = -1;
		char severity[] = "Notice";
		int currentType = getCurrentType();
		stringstream strMessage;
		strMessage << "Initializing FMI input interface (unit #" << currentUnit << ")";
		char *message = new char[strMessage.str().length() + 1];
		strcpy( message, strMessage.str().c_str() );
		message[strMessage.str().length()] = 0;

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );
	}
	else
	{
		int errorCode = -1;
		char message[] = "Only one FMI input interface (Type6139a) is allowed per model.";
		char severity[] = "Fatal";
		int currentType = getCurrentType();

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );

		backend->logger( fmiFatal, "ABORT", message );

		return 1;
	}

	// Tell the TRNSYS engine how this type works.
	int nParameters = 1; // Expect exactly one parameter: number of FMI inputs (equals to the number of type outputs).
	setNumberOfParameters( &nParameters );

	int nInputs = 0;   // Number of type inputs.
	setNumberOfInputs( &nInputs );

	int iParameter = 1;
	int nOutputs = static_cast<int>( getParameterValue( &iParameter ) ); // Number of type outputs.
	setNumberOfOutputs( &nOutputs );

	int nDerivatives = 0;   // Number of type derivatives.
	setNumberOfDerivatives( &nDerivatives );

	// The FMI input interface should be called like a default TRNSYS type.
	int iterationMode = 1;
	setIterationMode( &iterationMode );

	// Get comma separated list with input names (from type's special cards).
	int maxLabelLength = getMaxLabelLength();
	char* label = new char[maxLabelLength + 1];
	label[maxLabelLength] = 0;
	int iLabel = 2;
	getLabel( label, &maxLabelLength, &currentUnit, &iLabel );
	vector<string> inputLabels;
	HelperFunctions::splitAndTrim( label, inputLabels, ",;" );
	delete label;

	// Check if the string is actually empty. If it is empty, delete it.
	if ( ( inputLabels.size() == 1 ) && inputLabels[0].empty() ) inputLabels.clear();

	// Sanity check (type output == FMI inputs).
	if ( static_cast<size_t>( nOutputs ) != inputLabels.size() )
	{
		int errorCode = -1;
		char message[] =
			"The number of type outputs does not correspond to the number of FMI inputs";
		char severity[] = "Fatal";
		int currentType = getCurrentType();

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );

		backend->logger( fmiFatal, "ABORT", message );

		return 1;
	}

	// Initialize input variables in the backend.
	fmiStatus init;
	if ( fmiOK != ( init = backend->initializeRealInputs( inputLabels ) ) ) {
		int errorCode = -1;
		char message[] = "initializeRealInputs failed";
		char severity[] = "Fatal";
		int currentType = getCurrentType();

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );

		backend->logger( init, "ABORT", message );

		return 1;
	}

	return 0;
}


/// Initialize the FMI output interface.
int initializeFMIOutputInterface()
{
	// Get current unit number.
	int currentUnit = getCurrentUnit();

	if ( -1 == outputInterfaceUnit )
	{
		outputInterfaceUnit = currentUnit; // Identify current unit as FMI output interface.

		int errorCode = -1;
		char severity[] = "Notice";
		int currentType = getCurrentType();

		stringstream strMessage;
		strMessage << "Initialized FMI output interface (unit #" << currentUnit << ")";
		char *message = new char[strMessage.str().length() + 1];
		strcpy( message, strMessage.str().c_str() );
		message[strMessage.str().length()] = 0;

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );

	}
	else
	{
		int errorCode = -1;
		char message[] = "Only one FMI output interface (Type6139b) is allowed per model.";
		char severity[] = "Fatal";
		int currentType = getCurrentType();

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );

		backend->logger( fmiFatal, "ABORT", message );

		return 1;
	}

	// Tell the TRNSYS engine how this type works.
	int nParameters = 1; // Expect exactly one parameter: number of FMI outputs (equals to the number of type inputs).
	setNumberOfParameters( &nParameters );

	int iParameter = 1;
	int nInputs = static_cast<int>( getParameterValue( &iParameter ) ); // Number of type inputs.
	setNumberOfInputs( &nInputs );

	int nOutputs = 0; // Number of type outputs.
	setNumberOfOutputs( &nOutputs );

	int nDerivatives = 0;   // Number of type derivatives.
	setNumberOfDerivatives( &nDerivatives );

	// The FMI output interface should be called after the components have converged and after the integrators.
	int iterationMode = 2;
	setIterationMode( &iterationMode );

	// Get comma separated list with output names (from type's special cards).
	int maxLabelLength = getMaxLabelLength();
	char* label = new char[maxLabelLength + 1];
	label[maxLabelLength] = 0;
	int iLabel = 2;
	getLabel( label, &maxLabelLength, &currentUnit, &iLabel );
	vector<string> outputLabels;
	HelperFunctions::splitAndTrim( label, outputLabels, ",;" );
	delete label;

	// Check if the string is actually empty. If it is empty, delete it.
	if ( ( outputLabels.size() == 1 ) && outputLabels[0].empty() ) outputLabels.clear();

	// Sanity check (type input == FMI outputs).
	if ( static_cast<size_t>( nInputs ) != outputLabels.size() )
	{
		int errorCode = -1;
		char message[] =
			"The number of type inputs does not correspond to the number of FMI outputs";
		char severity[] = "Fatal";
		int currentType = getCurrentType();

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );

		backend->logger( fmiFatal, "ABORT", message );

		return 1;
	}

	// Initialize output variables in the backend.
	fmiStatus init;
	if ( fmiOK != ( init = backend->initializeRealOutputs( outputLabels ) ) )
	{
		int errorCode = -1;
		char message[] = "initializeRealOutputs failed";
		char severity[] = "Fatal";
		int currentType = getCurrentType();

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );

		backend->logger( init, "ABORT", message );

		return 1;
	}

	return 0;
}
