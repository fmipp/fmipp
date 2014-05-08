#include <FixedStepSizeFMU.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMU
#include <boost/test/unit_test.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string modelName( "sine_standalone" );
	FixedStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );
}


BOOST_AUTO_TEST_CASE( test_fmu_init )
{
	std::string modelName( "sine_standalone" );
	FixedStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );

	std::string initRealInputNames[1] = { "omega" };
	double initRealInputVals[1] = { 0.1 * M_PI };

	const double startTime = 0.0;
	const double stepSize = 1.0; // NB: fixed step size enforced by FMU!

	int status = fmu.init( "test_sine", initRealInputNames, initRealInputVals, 1, startTime, stepSize );
	BOOST_REQUIRE_MESSAGE( 1 == status, "init(...) FAILED" );
}


BOOST_AUTO_TEST_CASE( test_fmu_getrealoutputs )
{
	std::string modelName( "sine_standalone" );
	FixedStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );

	std::string initRealInputNames[1] = { "omega" };
	double initRealInputVals[1] = { 0.1 * M_PI };

	const double startTime = 0.0;
	const double stepSize = 1.0; // NB: fixed step size enforced by FMU!

	std::string realOutputNames[1] = { "phi" };

	fmu.defineRealOutputs( realOutputNames, 1 );

	int status = fmu.init( "test_sine", initRealInputNames, initRealInputVals, 1, startTime, stepSize );
	BOOST_REQUIRE_MESSAGE( 1 == status, "init(...) FAILED" );

	double* result = fmu.getRealOutputs();
	BOOST_REQUIRE( result[0] == 0.0 );
}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation )
{
	std::string modelName( "sine_standalone" );
	FixedStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );

	std::string initRealInputNames[1] = { "omega" };
	double initRealInputVals[1] = { 0.1 * M_PI };

	const double startTime = 0.0;
	const double stepSize = 1.0; // NB: fixed step size enforced by FMU!

	std::string realOutputNames[1] = { "phi" };

	fmu.defineRealOutputs( realOutputNames, 1 );

	int status = fmu.init( "test_sine", initRealInputNames, initRealInputVals, 1, startTime, stepSize );
	BOOST_REQUIRE_MESSAGE( 1 == status, "init(...) FAILED" );

	const double stopTime = 5.0;
	const double deltaTime = 0.2;
	double time = startTime;
	double* result;
	double reference;
	while ( time <= stopTime )
	{
		fmu.sync( time, time + deltaTime );
		time += deltaTime;

		result = fmu.getRealOutputs();
		reference = std::sin( 0.1 * M_PI * stepSize * std::floor( time/stepSize ) );

		BOOST_REQUIRE_MESSAGE( std::fabs( result[0] - reference ) < 1e-8,
				       "result mismatch: deltaResult = " << ( result[0] - reference ) );
	}
}
