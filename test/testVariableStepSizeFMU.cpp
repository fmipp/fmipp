// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#include <stdlib.h>
#include <common/fmi_v1.0/fmiModelTypes.h>
#include <import/utility/include/VariableStepSizeFMU.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMU
#include <boost/test/unit_test.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef WIN32
#include <signal.h>
void dummy_signal_handler( int ) {} // Dummy signal handler function.
#endif


BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string modelName( "sine_standalone" );
	VariableStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );
}


BOOST_AUTO_TEST_CASE( test_fmu_init )
{
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string modelName( "sine_standalone" );
	VariableStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );

	std::string initRealInputNames[1] = { "omega" };
	double initRealInputVals[1] = { 0.1 * M_PI };

	const double startTime = 0.0;
	const double stepSize = 1.0; // NB: fixed step size enforced by FMU!

	int status = fmu.init( "test_sine", initRealInputNames, initRealInputVals, 1, startTime, stepSize );
	BOOST_REQUIRE_MESSAGE( 1 == status, "init(...) FAILED" );
}


BOOST_AUTO_TEST_CASE( test_fmu_getrealoutputs )
{
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string modelName( "sine_standalone" );
	VariableStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );

	std::string initRealInputNames[1] = { "omega" };
	double initRealInputVals[1] = { 0.1 * M_PI };

	const double startTime = 0.0;
	const double stepSize = 1.0; // NB: fixed step size enforced by FMU!

	std::string realOutputNames[1] = { "x" };

	fmu.defineRealOutputs( realOutputNames, 1 );

	int status = fmu.init( "test_sine", initRealInputNames, initRealInputVals, 1, startTime, stepSize );
	BOOST_REQUIRE_MESSAGE( 1 == status, "init(...) FAILED" );

	double* result = fmu.getRealOutputs();
	BOOST_REQUIRE( result[0] == 0.0 );
}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation )
{
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string modelName( "sine_standalone" );
	VariableStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );

	std::string initRealInputNames[1] = { "omega" };
	double initRealInputVals[1] = { 0.1 * M_PI };

	const double startTime = 0.0;
	const double stepSize = 1.0; // NB: fixed step size enforced by FMU!

	std::string realOutputNames[1] = { "x" };

	fmu.defineRealOutputs( realOutputNames, 1 );

	int status = fmu.init( "test_sine", initRealInputNames, initRealInputVals, 1, startTime, 2 * stepSize );
	BOOST_REQUIRE_MESSAGE( 1 == status, "init(...) FAILED" );

	const double stopTime = 5.0;
	double time = startTime;
	double* result;
	double reference;
	while ( time <= stopTime )
	{
		fmu.sync( time, time + stepSize );
		time += stepSize;

		result = fmu.getRealOutputs();
		reference = std::sin( 0.1 * M_PI * stepSize * time );

		BOOST_REQUIRE_MESSAGE( std::fabs( result[0] - reference ) < 1e-8,
				       "result mismatch: deltaResult = " << ( result[0] - reference ) );
	}
}
