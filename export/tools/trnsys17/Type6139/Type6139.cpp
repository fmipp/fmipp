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
#include <sstream>

#include "TRNSYS.h" // TRNSYS acess functions (allow to acess TIME etc.).

#include "FMIComponentBackEnd.h"
#include "HelperFunctions.h"


using namespace std;

namespace {

	FMIComponentBackEnd* backend = 0; // Pointer to FMI component backend.

	int inputInterfaceUnit = -1; // Unit number of FMI input interface.

	int outputInterfaceUnit = -1; // Unit number of FMI output interface.

	const double hoursToSeconds = 3600.; // Conversion from hours (TRNSYS time unit) to seconds (FMU time unit).
}


// Helper function for initializing the FMI input interface.
int initializeFMIInputInterface();


// Helper function for initializing the FMI output interface.
int initializeFMIOutputInterface();


// Main function implementing the TRNSYS Type.
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

			int errorCode = -1;
			char severity[] = "Notice";
			int currentType = getCurrentType();
			string strMessage = string( "Initialized FMI component backend, log file will be written to: " ) + backend->getLogFileName();
			char *message = new char[strMessage.length() + 1];
			message[strMessage.length()] = 0;
			strcpy( message, strMessage.c_str() );

			Messages( &errorCode, message, severity,
				  &currentUnit, &currentType,
				  strlen(message), strlen(severity) );
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
		if ( currentUnit == outputInterfaceUnit )
		{
			// In case the unit is an output interface, set the current outputs for the FMU instance.
			backend->setRealOutputs( xin, static_cast<size_t>( par[0] ) ); // Set type inputs as FMU outputs.
			backend->enforceTimeStep( hoursToSeconds * getSimulationTimeStep() ); // TRNSYS can't do dynmic steps!

			// At this point in the simulation, the FMI input and/or output interfaces
			// have been initialized. Hence it is save to end the backend initialization
			// and wait to resume execution.
			backend->endInitialization();
			backend->waitForMaster();

			return 1;
		}

		if ( currentUnit == inputInterfaceUnit )
		{
			backend->getRealInputs( xout, static_cast<size_t>( par[0] ) ); // Set FMU inputs as type outputs.
			return 1;
		}
	}

	// Perform any "end of timestep manipulations" that may be required.
	if ( getIsEndOfTimestep() && ( currentUnit == outputInterfaceUnit ) )
	{
		// In case the unit is an output interface, set the current outputs for the FMU instance.
		backend->setRealOutputs( xin, static_cast<size_t>( par[0] ) ); // Set type inputs as FMU outputs.
		backend->enforceTimeStep( hoursToSeconds * getSimulationTimeStep() ); // TRNSYS can't do dynamic steps!
 
		backend->signalToMaster();
		backend->waitForMaster();

		return 1;
	}

	if ( currentUnit == inputInterfaceUnit )
	{
		// Check if TRNSYS is in sync with the external master algorithm. 
		double externalSimTime =
			backend->getCurrentCommunicationPoint()	+ backend->getCommunicationStepSize();
		double trnsysSimTime = hoursToSeconds * getSimulationTime();
		if ( externalSimTime != trnsysSimTime )
		{
			std::stringstream message;
			message << "TRNSYS simulation time (" << trnsysSimTime << ") does not match with "
				<< "external simulation time (current communication point + communication "
				<< "step size = " << backend->getCurrentCommunicationPoint() << " + "
				<< backend->getCommunicationStepSize() << " = " << externalSimTime << ")"
				<< std::endl;

			backend->logger( fmi2OK, "DEBUG", message.str() );
		}

		backend->getRealInputs( xout, static_cast<size_t>( par[0] ) ); // Set FMU inputs as type outputs.

	}

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

		backend->logger( fmi2Fatal, "ABORT", message );

		return 1;
	}

	// Tell the TRNSYS engine how this type works.
	int nParameters = 1; // Expect exactly one parameter: number of FMI inputs (equals to the number of type outputs).
	setNumberOfParameters( &nParameters );

	int nInputInterfaceInputs = 0;   // Number of type inputs.
	setNumberOfInputs( &nInputInterfaceInputs );

	int iParameter = 1;
	int nInputInterfaceOutputs = static_cast<int>( getParameterValue( &iParameter ) ); // Number of type outputs.
	setNumberOfOutputs( &nInputInterfaceOutputs );

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
	vector<string> fmiInputLabels;
	HelperFunctions::splitAndTrim( label, fmiInputLabels, ",;" );
	delete label;

	// Check if the string is actually empty. If it is empty, delete it.
	if ( ( fmiInputLabels.size() == 1 ) && fmiInputLabels[0].empty() ) fmiInputLabels.clear();

	// If there is only one input argument, check whether it is an input file. If it is, parse 
	// input labels from file.
	if ( fmiInputLabels.size() == 1 )
	{
		const string fileName = fmiInputLabels[0];
		if ( true == HelperFunctions::readDataFromFile( fileName, fmiInputLabels ) )
		{
			stringstream message;
			message << "retrieved input labels from file (" << fileName << "): " << std::endl;

			vector<string>::iterator it;
			for ( it = fmiInputLabels.begin(); it != fmiInputLabels.end(); ++it )
				message << (*it) << std::endl;

			backend->logger( fmi2OK, "DEBUG", message.str() );
		}
	}

	// Sanity check (type output == FMI inputs).
	if ( static_cast<size_t>( nInputInterfaceOutputs ) != fmiInputLabels.size() )
	{
		int errorCode = -1;
		char message[] =
			"The number of type outputs does not correspond to the number of FMI inputs";
		char severity[] = "Fatal";
		int currentType = getCurrentType();

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );

		backend->logger( fmi2Fatal, "ABORT", message );

		return 1;
	}

	// Initialize input variables in the backend.
	fmi2Status init;
	if ( fmi2OK != ( init = backend->initializeRealInputs( fmiInputLabels ) ) ) {
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

		backend->logger( fmi2Fatal, "ABORT", message );

		return 1;
	}

	// Tell the TRNSYS engine how this type works.
	int nParameters = 1; // Expect exactly one parameter: number of FMI outputs (equals to the number of type inputs).
	setNumberOfParameters( &nParameters );

	int iParameter = 1;
	int nOutputInterfaceInputs = static_cast<int>( getParameterValue( &iParameter ) ); // Number of type inputs.
	setNumberOfInputs( &nOutputInterfaceInputs );

	int nOutputInterfaceOutputs = 0; // Number of type outputs.
	setNumberOfOutputs( &nOutputInterfaceOutputs );

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
	vector<string> fmiOutputLabels;
	HelperFunctions::splitAndTrim( label, fmiOutputLabels, ",;" );
	delete label;

	// Check if the string is actually empty. If it is empty, delete it.
	if ( ( fmiOutputLabels.size() == 1 ) && fmiOutputLabels[0].empty() ) fmiOutputLabels.clear();

	// If there is only one input argument, check whether it is an input file. If it is, parse 
	// output labels from file.
	if ( fmiOutputLabels.size() == 1 )
	{
		const string fileName = fmiOutputLabels[0];
		if ( true == HelperFunctions::readDataFromFile( fileName, fmiOutputLabels ) )
		{
			stringstream message;
			message << "retrieved output labels from file (" << fileName << "): " << std::endl;

			vector<string>::iterator it;
			for ( it = fmiOutputLabels.begin(); it != fmiOutputLabels.end(); ++it )
				message << (*it) << std::endl;

			backend->logger( fmi2OK, "DEBUG", message.str() );
		}
	}

	// Sanity check (type input == FMI outputs).
	if ( static_cast<size_t>( nOutputInterfaceInputs ) != fmiOutputLabels.size() )
	{
		int errorCode = -1;
		char message[] =
			"The number of type inputs does not correspond to the number of FMI outputs";
		char severity[] = "Fatal";
		int currentType = getCurrentType();

		Messages( &errorCode, message, severity,
			  &currentUnit, &currentType,
			  strlen(message), strlen(severity) );

		backend->logger( fmi2Fatal, "ABORT", message );

		return 1;
	}

	// Initialize output variables in the backend.
	fmi2Status init;
	if ( fmi2OK != ( init = backend->initializeRealOutputs( fmiOutputLabels ) ) )
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
