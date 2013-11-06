/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

#include "TRNSYS.h" // TRNSYS acess functions (allow to acess TIME etc.).

#include "FMIComponentBackEnd.h"
#include "HelperFunctions.h"

using namespace std;

namespace {

	FMIComponentBackEnd* backend;

	const double hoursToSeconds = 3600.;
}


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

	// Do all of the "very first call of the simulation” manipulations here.
	if ( getIsFirstCallOfSimulation() )
	{
		// Tell the TRNSYS engine how this type works.
		int nParameters = 2;   // Number of parameters we expect (n_inputs, n_outputs).
		int nInputs = par[0];   // Number of inputs.
		int nOutputs = par[1]; // Number of outputs.
		int nDerivatives = 0;   // Number of derivatives.

		setNumberOfParameters( &nParameters );
		setNumberOfInputs( &nInputs );
		setNumberOfOutputs( &nOutputs );
		setNumberOfDerivatives( &nDerivatives );

		int iterationMode = 1;
		setIterationMode( &iterationMode );
		
		int nStoreStatic = 0;
		int nStoreDynamic = 0;
		setNumberStoredVariables( &nStoreStatic, &nStoreDynamic );

		int currentUnit = getCurrentUnit();
		int maxLabelLength = getMaxLabelLength();

		char* label = new char[maxLabelLength];
		int iLabel;

		iLabel = 1;
		getLabel( label, &maxLabelLength, &currentUnit, &iLabel );
		vector<string> inputLabels;
		HelperFunctions::splitAndTrim( label, inputLabels, ",;" );
		// Sanity check (type output == FMI inputs).
		if ( nOutputs != inputLabels.size() ) return 0; // FIXME: Return error message. 

		iLabel = 2;
		getLabel( label, &maxLabelLength, &currentUnit, &iLabel );
		vector<string> outputLabels;
		HelperFunctions::splitAndTrim( label, outputLabels, ",;" );
		// Sanity check (type input == FMI outputs).
		if ( nInputs != outputLabels.size() ) return 0; // Sanity check. FIXME: Return error message. 

		delete label;

		backend = new FMIComponentBackEnd;

		backend->startInitialization();

		fmiStatus init;

		if ( fmiOK != ( init = backend->initializeRealInputs( inputLabels ) ) ) {
			cout << "initializeRealInputs returned " << init << endl;
		}

		if ( fmiOK != ( init = backend->initializeRealOutputs( outputLabels ) ) ) {
			cout << "initializeRealOutputs returned " << init << endl;
		}

		backend->enforceTimeStep( hoursToSeconds * getSimulationTimeStep() ); // TRNSYS can't do dynmic steps!

		backend->endInitialization();

		return 1;
	}

	// Do all of the “last call” manipulations that may be required.
	if ( getIsLastCallOfSimulation() )
	{
		delete backend;
		return 1;
	}

	// Do all of the "first timestep manipulations" here - there are no iterations at the intial time.
	if ( getIsFirstTimestep() )
	{
		backend->waitForMaster();
		backend->getRealInputs( xout, par[1] ); // FMU inputs are Type outputs!
		return 1;
	}

	// Perform any "end of timestep manipulations" that may be required.
	if ( getIsConvergenceReached() )
	{
		backend->setRealOutputs( xin, par[0] ); // Type inputs are FMU outputs.

		backend->enforceTimeStep( hoursToSeconds * getSimulationTimeStep() ); // TRNSYS can't do dynmic steps!

		backend->signalToMaster();
		backend->waitForMaster();

		return 1;
	}

	// Re-read the parameters in case another unit of this type has been called last.
	if ( getIsReReadParameters() )
	{
		//cout << "re-read parameters" << endl;
	}

        backend->getRealInputs( xout, par[1] ); // FMU inputs are Type outputs!

	// EVERYTHING IS DONE - RETURN FROM THIS SUBROUTINE AND MOVE ON.
	return 1;
}
