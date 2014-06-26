
#include <math.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fstream>

#include "common/fmi_v1.0/fmiModelTypes.h"

#include "FMIComponentBackEnd.h"

namespace {
	const double twopi = 6.28318530718;
}



int main( int argc, const char* argv[] )
{
	if ( 4 != argc ) {
		std::ostringstream ss;
		ss << ( argc - 1 );
		std::string err =
			std::string( "Wrong number of input arguments - expected 3, but got " ) + ss.str();
		std::cerr << err << std::endl;
		throw std::runtime_error( err ); /// \FIXME Call logger.
	}


#ifdef WIN32
	std::string expectedEntryPoint = std::string( "\\\\entry\\point" );
#else
	std::string expectedEntryPoint = std::string( "entry/point" );
#endif
	std::string expectedPreArgument = std::string( "pre" );
	std::string expectedPostArgument = std::string( "post" );


	if ( std::string( argv[1] ) != expectedPreArgument ) {
		std::string err =
			std::string( "Wrong input argument - expected \"" ) + expectedPreArgument +
			std::string( "\", but got " ) + std::string( argv[1] );
		std::cerr << err << std::endl;
		throw std::runtime_error( err ); /// \FIXME Call logger.
	}

	if ( std::string( argv[2] ) != expectedEntryPoint ) {
		std::string err =
			std::string( "Wrong input argument - expected \"" ) + expectedEntryPoint +
			std::string( "\", but got " ) + std::string( argv[2] );
		std::cerr << err << std::endl;
		throw std::runtime_error( err ); /// \FIXME Call logger.
	}

	if ( std::string( argv[3] ) != expectedPostArgument ) {
		std::string err =
			std::string( "Wrong input argument - expected \"" ) + expectedPostArgument +
			std::string( "\", but got " ) + std::string( argv[3] );
		std::cerr << err << std::endl;
		throw std::runtime_error( err ); /// \FIXME Call logger.
	}

	fmiReal time = 0.;
	fmiReal fixedTimeStep = 1.;

	fmiReal omega;
	fmiReal x;
	fmiInteger cycles;
	fmiBoolean positive;


	std::vector<std::string> realInputLabels( 1, "omega" );
	std::vector<std::string> realOutputLabels( 1, "x" );
	std::vector<std::string> integerOutputLabels( 1, "cycles" );
	std::vector<std::string> booleanOutputLabels( 1, "positive" );

	std::vector<fmiReal*> realInputs( 1, &omega );
	std::vector<fmiReal*> realOutputs( 1, &x );
	std::vector<fmiInteger*> integerOutputs( 1, &cycles );
	std::vector<fmiBoolean*> booleanOutputs( 1, &positive );

	// Init backend.
	FMIComponentBackEnd backend;

	try { backend.startInitialization(); } catch (...) { return -1; }

	fmiStatus init;

	if ( fmiOK != ( init = backend.initializeRealInputs( realInputLabels ) ) ) {
		std::cout << "initializeRealInputs returned " << init << std::endl;
	}

	if ( fmiOK != ( init = backend.initializeRealOutputs( realOutputLabels ) ) ) {
		std::cout << "initializeRealOutputs returned " << init << std::endl;
	}

	if ( fmiOK != ( init = backend.initializeIntegerOutputs( integerOutputLabels ) ) ) {
		std::cout << "initializeBoolOutputs returned " << init << std::endl;
	}

	if ( fmiOK != ( init = backend.initializeBooleanOutputs( booleanOutputLabels ) ) ) {
		std::cout << "initializeBoolOutputs returned " << init << std::endl;
	}

	backend.enforceTimeStep( fixedTimeStep ); // Let's do fixed time steps!
	backend.endInitialization();

	// Pseudo simulation loop.
	while ( true )
	{
		backend.waitForMaster();
		backend.getRealInputs( realInputs );

		time += fixedTimeStep;
		x = sin( omega*time );
		cycles = int( omega*time/twopi );
		positive = ( x > 0. ) ? fmiTrue : fmiFalse;

		backend.setRealOutputs( realOutputs );
		backend.setIntegerOutputs( integerOutputs );
		backend.setBooleanOutputs( booleanOutputs );
		backend.enforceTimeStep( fixedTimeStep );
		backend.signalToMaster();
	}

	return 0;
}
