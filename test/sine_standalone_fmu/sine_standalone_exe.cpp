
#include <math.h>
#include <string>
#include <vector>
#include <iostream>

#include "common/fmi_v1.0/fmiModelTypes.h"

#include "FMIComponentBackEnd.h"


int main( int argc, const char* argv[] )
{
	fmiReal time = 0.;
	fmiReal fixedTimeStep = 1.;

	fmiReal omega;
	fmiReal phi;

	std::vector<std::string> inputLabels( 1, "omega" );
	std::vector<std::string> outputLabels( 1, "phi" );

	std::vector<fmiReal*> inputs( 1, &omega );
	std::vector<fmiReal*> outputs( 1, &phi );

	// Init backend.
	FMIComponentBackEnd backend;

	try { backend.startInitialization(); } catch (...) { return -1; }

	fmiStatus init;

	if ( fmiOK != ( init = backend.initializeRealInputs( inputLabels ) ) ) {
		std::cout << "initializeRealInputs returned " << init << std::endl;
	}

	if ( fmiOK != ( init = backend.initializeRealOutputs( outputLabels ) ) ) {
		std::cout << "initializeRealOutputs returned " << init << std::endl;
	}

	backend.enforceTimeStep( fixedTimeStep ); // Let's do fixed time steps!
	backend.endInitialization();

	// Pseudo simulation loop.
	while ( true )
	{
		backend.waitForMaster();
		backend.getRealInputs( inputs );

		time += fixedTimeStep;
		phi = sin( omega*time );

		backend.setRealOutputs( outputs );
		backend.enforceTimeStep( fixedTimeStep );
		backend.signalToMaster();
	}

	return 0;
}
