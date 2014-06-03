#include <import/base/include/FMUCoSimulation.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMIExportUtilities

#include <boost/test/unit_test.hpp>
#include <cmath>


#ifndef WIN32
#include <signal.h>
void dummy_signal_handler( int ) {} // Dummy signal handler function.
#endif


namespace {
	const double twopi = 6.28318530718;
}



BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string MODELNAME( "sine_standalone" );
	FMUCoSimulation fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
}


BOOST_AUTO_TEST_CASE( test_fmu_instantiate )
{
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string MODELNAME( "sine_standalone" );
	FMUCoSimulation fmu( FMU_URI_PRE + MODELNAME, MODELNAME );

	fmiStatus status = fmu.instantiate( "sine_standalone1", 0., fmiFalse, fmiFalse, fmiFalse );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "instantiate(...) failed: status = " << status );
}


BOOST_AUTO_TEST_CASE( test_fmu_initialize )
{
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string MODELNAME( "sine_standalone" );
	FMUCoSimulation fmu( FMU_URI_PRE + MODELNAME, MODELNAME );

	fmiStatus status = fmu.instantiate( "sine_standalone1", 0., fmiFalse, fmiFalse, fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	fmu.initialize( 0., fmiTrue, 10. );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "initialize(...) failed: status = " << status  );
}


BOOST_AUTO_TEST_CASE( test_fmu_getvalue )
{
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string MODELNAME( "sine_standalone" );
	FMUCoSimulation fmu( FMU_URI_PRE + MODELNAME, MODELNAME );

	fmiStatus status = fmu.instantiate( "sine_standalone1", 0., fmiFalse, fmiFalse, fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	fmu.initialize( 0., fmiTrue, 10. );
	BOOST_REQUIRE( status == fmiOK );

	fmiReal testReal;

	status = fmu.getValue( "omega", testReal );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "getValue(...) failed: status = " << status );
	BOOST_REQUIRE_MESSAGE( testReal == 1., "getValue(...) failed: return value = " << testReal );

	status = fmu.getValue( "x", testReal );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "getValue(...) failed: status = " << status );
	BOOST_REQUIRE_MESSAGE( testReal == 0., "getValue(...) failed: return value = " << testReal );
}


BOOST_AUTO_TEST_CASE( test_fmu_setvalue )
{
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string MODELNAME( "sine_standalone" );
	FMUCoSimulation fmu( FMU_URI_PRE + MODELNAME, MODELNAME );

	fmiStatus status = fmu.instantiate( "sine_standalone1", 0., fmiFalse, fmiFalse, fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "omega", 0.123 );
	BOOST_REQUIRE( status == fmiOK );

	fmiReal testReal;
	status = fmu.getValue( "omega", testReal );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "setValue(...) failed: status = " << status );
	BOOST_REQUIRE_MESSAGE( testReal == 0.123, "setValue(...) failed: return value = " << testReal );

	fmu.initialize( 0., fmiTrue, 10. );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "initialize(...) failed: status = " << status  );

}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_1 )
{
#ifndef WIN32
	// Avoid that BOOST treats SIGCHLD signal as error.
	BOOST_REQUIRE( signal( SIGCHLD, dummy_signal_handler ) != SIG_ERR );
#endif

	std::string MODELNAME( "sine_standalone" );
	FMUCoSimulation fmu( FMU_URI_PRE + MODELNAME, MODELNAME );

	fmiStatus status = fmu.instantiate( "sine_standalone1", 0., fmiFalse, fmiFalse, fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	fmiReal omega = 0.628318531; // Corresponds to a period of 10s.
	status = fmu.setValue( "omega", omega );
	BOOST_REQUIRE( status == fmiOK );

	fmu.initialize( 0., fmiTrue, 10. );
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.;
	fmiReal stepsize = 1.;
	fmiReal tstop = 10.;
	fmiReal x = 0.;
	fmiInteger cycles = 0;
	fmiBoolean positive = fmiFalse;

	while ( ( t + stepsize ) - tstop < EPS_TIME )
	{
		// Make co-simulation step.
		status = fmu.doStep( t, stepsize, fmiTrue );
		BOOST_REQUIRE_MESSAGE( status == fmiOK, "doStep(...) failed: status = " << status );

		// Advance time.
		t += stepsize;
		BOOST_REQUIRE_MESSAGE( std::abs( t - fmu.getTime() ) < EPS_TIME,
				       "advance time failed: time = " << fmu.getTime() <<
				       " -> should be " << t );

		// Retrieve result.
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE_MESSAGE( status == fmiOK,
				       "getValue(...) for fmiReal failed: status = " << status );

		status = fmu.getValue( "cycles", cycles );
		BOOST_REQUIRE_MESSAGE( status == fmiOK,
				       "getValue(...) for fmiInteger failed: status = " << status );

		status = fmu.getValue( "positive", positive );
		BOOST_REQUIRE_MESSAGE( status == fmiOK,
				       "getValue(...) for fmiBoolean failed: status = " << status );

		BOOST_REQUIRE_MESSAGE( std::abs( x - sin( omega*t ) ) < 1e-9,
				       "wrong simulation results for x : return value = " << x <<
				       " -> should be " << sin( omega*t ) );

		BOOST_REQUIRE_MESSAGE( cycles == int( omega*t/twopi ),
				       "wrong simulation results for cycles : return value = " << cycles <<
				       " -> should be " << int( omega*t/twopi ) );

		BOOST_REQUIRE_MESSAGE( positive == ( ( x > 0. ) ? fmiTrue : fmiFalse ),
				       "wrong simulation results for cycles : return value = " << positive <<
				       " -> should be " << ( ( x > 0. ) ? fmiTrue : fmiFalse ) );

	}

	BOOST_REQUIRE( std::abs( tstop - fmu.getTime() ) < EPS_TIME );
}
