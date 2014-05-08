#include <InterpolatingFixedStepSizeFMU.h>

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
	InterpolatingFixedStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );
}


BOOST_AUTO_TEST_CASE( test_fmu_init )
{
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string modelName( "sine_standalone" );
	InterpolatingFixedStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );

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
	InterpolatingFixedStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );

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
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string modelName( "sine_standalone" );
	InterpolatingFixedStepSizeFMU fmu( std::string( FMU_URI_PRE ) + modelName, modelName );

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
	while ( time <= stopTime )
	{
		fmu.sync( time, time + deltaTime );
		time += deltaTime;

		double* result = fmu.getRealOutputs();

		double t0 = stepSize * std::floor( time/stepSize );
		double t1 = t0 + stepSize;
		double x0 = std::sin( 0.1 * M_PI * t0 );
		double x1 = std::sin( 0.1 * M_PI * t1 );
		double reference = x0 + ( time - t0 )*( x1 - x0 )/( t1 - t0 );

		BOOST_REQUIRE_MESSAGE( std::fabs( result[0] - reference ) < 1e-8,
				       "result mismatch: deltaResult = " << ( result[0] - reference ) );
	}
}
