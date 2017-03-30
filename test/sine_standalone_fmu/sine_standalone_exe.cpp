// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#include <string>
#include <sstream>
#include <cmath>

#include "BackEndApplicationBase.h"


namespace {
	const double twopi = 6.28318530718;
}


// To implement an FMI backend, inherit from class 'BackEndApplicationBase'.
class SineStandalone : public BackEndApplicationBase
{
public:

	// The following three functions have to be implemented:
	virtual void initializeScalarVariables();
	virtual int initializeBackEnd( int argc, const char* argv[] );
	virtual void initializeParameterValues();
	virtual int doStep( const fmi2Real& syncTime, const fmi2Real& lastSyncTime );

private:

	// Define all FMI input/output variables and parameters as class members:
	fmi2Real omega;
	fmi2Real x;
	fmi2Integer cycles;
	fmi2Boolean positive;
};


// This function initializes the backend's scalar variables (parameters, inputs, outputs),
// which have to be class member variables (or global variables).
// Only calls to 'addRealInput(...)', 'addRealOutput(...)', etc. are allowed.
void
SineStandalone::initializeScalarVariables()
{
	// Define real FMI input variable.
	addRealInput( omega );

	// Define real FMI output variable.
	addRealOutput( x );

	// Define integer FMI output variable.
	addIntegerOutput( cycles );
	
	// Define boolean FMI output variable.
	addBooleanOutput( positive );
}


// This function initializes the backend (everything except the scalar variables).
// The input arguments are the command line input arguments when the backend is 
// started (compare 'Capabilities' and 'VendorAnnotations' in modelDescription.xml).
//
// Here, the function is mostly used to perform checks on the input. In general it 
// could be used load external files or initialize an internal model.
int
SineStandalone::initializeBackEnd( int argc, const char* argv[] )
{
	// Specify expected initialization inputs.
	int expectedNumberOfInitInputs = 4;
#ifdef WIN32
	std::string expectedEntryPoint = std::string( "\\\\entry\\point" );
#else
	std::string expectedEntryPoint = std::string( "entry/point" );
#endif
	std::string expectedPreArgument = std::string( "pre" );
	std::string expectedPostArgument = std::string( "post" );

	// Check number of initialization inputs.
	if ( argc != expectedNumberOfInitInputs ) {
		std::stringstream ss;
		ss << "Wrong number of inputs at initialization - expected "  << expectedNumberOfInitInputs
		   << ", but got " << + argc;
		logger( fmi2Fatal, "ABORT", ss.str() );
		return -1;
	}

	// Check first initialization input argument.
	if ( std::string( argv[1] ) != expectedPreArgument ) {
		std::string err =
			std::string( "Wrong input argument - expected \"" ) + expectedPreArgument +
			std::string( "\", but got " ) + std::string( argv[1] );
		logger( fmi2Fatal, "ABORT", err );
		return -1;
	}

	// Check second initialization input argument.
	if ( std::string( argv[2] ) != expectedEntryPoint ) {
		std::string err =
			std::string( "Wrong input argument - expected \"" ) + expectedEntryPoint +
			std::string( "\", but got " ) + std::string( argv[2] );
		logger( fmi2Fatal, "ABORT", err );
		return -1;
	}

	// Check third initialization input argument.
	if ( std::string( argv[3] ) != expectedPostArgument ) {
		std::string err =
			std::string( "Wrong input argument - expected \"" ) + expectedPostArgument +
			std::string( "\", but got " ) + std::string( argv[3] );
		logger( fmi2Fatal, "ABORT", err );
		return -1;
	}

	// Enforce fixed time steps.
	fmi2Real fixedTimeStep = 1.;
	enforceTimeStep( fixedTimeStep );

	return 0;
}


void
SineStandalone::initializeParameterValues() {}


// This function is called whenever the frontend's doStep(...) function is called.
int
SineStandalone::doStep( const fmi2Real& syncTime, const fmi2Real& lastSyncTime )
{
	// When this function is called, the variables defined as inputs during 
	// initialization have already been synchronized to the latest input from 
	// the frontend. Here, this is variable 'omega'.
	
	x = sin( omega*syncTime );
	cycles = int( omega*syncTime/twopi );
	positive = ( x > 0. ) ? fmi2True : fmi2False;
		
	// After this function is called, the backend will be synchronized with 
	// the values of the variables defined as outputs during the initialization. 
	// Here, these are variables 'x', 'cycles' and 'positive'

	// Provide debug information.
	std::stringstream ss;
	ss << "syncTime = " << syncTime << " - x = " << x << " - cycles = " << cycles;
	logger( fmi2OK, "DEBUG", ss.str() );

	return 0; // No errors, return value 0.
}


// Using the class defined above, the next line creates a stand-alone application that utilizes the generic FMU backend.
CREATE_BACKEND_APPLICATION( SineStandalone )
