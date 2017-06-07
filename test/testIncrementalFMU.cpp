// --------------------------------------------------------------
// Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#include <stdlib.h>
#include <common/fmi_v1.0/fmiModelTypes.h>
#include <import/utility/include/IncrementalFMU.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testIncrementalFMU
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>


BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
}

BOOST_AUTO_TEST_CASE( test_fmu_init )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 10.0, 1.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );
}

BOOST_AUTO_TEST_CASE( test_fmu_getrealoutputs )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 10.0, 1.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[2] = { "x", "der(x)" };

	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double* result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );
	BOOST_CHECK_EQUAL( result[1], 10.0 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_1 )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 1.0, 0.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[2] = { "x", "der(x)" };

	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double* result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );
	BOOST_CHECK_EQUAL( result[1], 1.0 );

	double time = starttime;
	double next = fmu.sync( -42.0, time );
	double oldnext;
	BOOST_REQUIRE_EQUAL( next, horizon );

	while ( time + stepsize - 1.0  < EPS_TIME ) {
		oldnext = next;
		next = fmu.sync( time, std::min( time + stepsize, next ) );
		result = fmu.getRealOutputs();
		time = std::min( time + stepsize, oldnext );
		if ( std::abs( time - 0.5 ) < 1e-6 ) {
			BOOST_CHECK_CLOSE( result[0], 0.5, 1e-4 );
		}
	}
	result = fmu.getRealOutputs();
	BOOST_CHECK_SMALL( time - 1.0, stepsize/2 );
	BOOST_CHECK_CLOSE( result[0], 1.0, 1e-4 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_2 )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 10.0, 0.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[2] = { "x", "der(x)" };

	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double* result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );
	BOOST_CHECK_EQUAL( result[1], 10.0 );

	double time = starttime;
	double next = fmu.sync( -42.0, time );
	double oldnext;
	BOOST_REQUIRE_EQUAL( next, horizon );

	double dx = 10.0;
	int eventctr = 0;

	while ( time - 1.0  < EPS_TIME ) {
		oldnext = next;
		next = fmu.sync( time, std::min( time + stepsize, next ) );
		result = fmu.getRealOutputs();
		time = std::min( time + stepsize, oldnext );
		if ( result[1] != dx ) {
			eventctr++;
			dx = result[1];
		}
	}
	BOOST_CHECK_EQUAL( eventctr, 5 );
	
	result = fmu.getRealOutputs();	
	BOOST_CHECK_SMALL( time - 1.0, stepsize/2 );
	BOOST_CHECK_SMALL( result[0], 1e-6 );
}


BOOST_AUTO_TEST_CASE( test_fmu_check_sync_times )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );

	const double start_time = 0.0;
	const double stop_time = 4.0;
	const double step_size = 0.3;

	const double horizon = 2 * step_size;
	const double int_step_size = step_size/2;

	std::string init_vars[2] = { "k", "x" };
	double init_vals[2] = { 1.0, 0.0 };

	std::string outputs[2] = { "x", "der(x)" };
	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", init_vars, init_vals, 2, start_time, horizon, step_size, int_step_size );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double time = start_time;
	double next = start_time;
	double old_next;

	std::vector<double> sync_times;

	while ( time - stop_time  < EPS_TIME ) {
		old_next = next;
		next = fmu.sync( time, std::min( time + step_size, next ) );

		std::cout << "t0 = " << time << "\t - t1 = " << std::min( time + step_size, old_next ) << "\t - next = " << next << std::endl;

		time = std::min( time + step_size, old_next );
		sync_times.push_back( time );
	}

	double expected_sync_times[15] = { 0., .3, .6, .9, 1., 1.3, 1.6, 1.9, 2.2, 2.5, 2.8, 3., 3.3, 3.6, 3.9 };

	for ( int i = 0; i < 15; ++i )
		BOOST_CHECK_CLOSE( sync_times[i], expected_sync_times[i], 1e-7 );
}


/** @brief Check the event's timing using FMU zigzag */
BOOST_AUTO_TEST_CASE( test_fmu_indicated_event_timing_0 )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 1.0, 0.0 };
	const double starttime = 0.0;
	const double stepsize = 0.11;

	const double horizon = 10 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[2] = { "x", "der(x)" };

	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double* result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );
	BOOST_CHECK_EQUAL( result[1], 1.0 );

	// Get first event at t=1.0
	double time = fmu.sync( -42.0, starttime );
	BOOST_CHECK_CLOSE( time, 1.0, 1.0*100*EPS_TIME );

	// Get end of horizon event at t=2.1
	time = fmu.sync( starttime, time );
	BOOST_CHECK_CLOSE( time, 2.1, 2*2.1*100*EPS_TIME );
}

/** 
 * @brief Check the event's timing using FMU zerocrossing 
 * @details The test case focuses on input change events
 */
BOOST_AUTO_TEST_CASE( test_fmu_indicated_event_timing_1 )
{
	std::string MODELNAME( "zerocrossing" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[2] = { "u", "threshold" };
	double vals[] = { 1.0, 0.0 };
	const double starttime = 0.0;
	const double stepsize = 0.1;

	const double horizon = 10 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[] = { "domain" };
	fmu.defineIntegerOutputs( outputs, sizeof(outputs)/sizeof(outputs[0]) );

	std::string inputs[] = { "u" };
	fmu.defineRealInputs(inputs, sizeof(inputs)/sizeof(inputs[0]));

	int status = fmu.init( "zerocrossing", vars, vals, 
		sizeof(vars)/sizeof(vars[0]), starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	// Set the input to trigger a domain change
	vals[0] = -1.0;
	fmu.syncState(0.0, vals, NULL, NULL, NULL);

	fmiInteger* intOutputs = fmu.getIntegerOutputs();
	BOOST_CHECK_EQUAL(intOutputs[0], -1);

	// Predict one step
	fmiTime time = fmu.predictState(0.0);
	BOOST_CHECK_CLOSE( time, 1.0, 1.0*100*EPS_TIME );

	// Check outputs again
	intOutputs = fmu.getIntegerOutputs();
	BOOST_CHECK_EQUAL(intOutputs[0], -1);

	// Set the input to trigger a domain change
	vals[0] = 1.0;
	fmu.syncState(1.0, vals, NULL, NULL, NULL);

	// Check outputs again -> The domain change needs to be visible immediately
	intOutputs = fmu.getIntegerOutputs();
	BOOST_CHECK_EQUAL(intOutputs[0], 1);
}

/** 
 * @brief Check the event's timing using FMU zerocrossing 
 * @details The test case focuses on indicated events
 */
BOOST_AUTO_TEST_CASE( test_fmu_indicated_event_timing_2 )
{
	std::string MODELNAME( "zerocrossing" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[] = { "u", "tOn", "tOff" }; // u is just a dummy input
	double vals[] = { 1.0, 0.5, 2.0 };
	const double starttime = 0.0;
	const double stepsize = 0.1;

	const double horizon = 10 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[] = { "timeZero" };
	fmu.defineIntegerOutputs( outputs, sizeof(outputs)/sizeof(outputs[0]) );

	std::string inputs[] = { "u" };
	fmu.defineRealInputs(inputs, sizeof(inputs)/sizeof(inputs[0]));

	int status = fmu.init( "zerocrossing", vars, vals, 
		sizeof(vars)/sizeof(vars[0]), starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	// Set the input to trigger no domain change
	vals[0] = 1.0;
	fmu.syncState(0.0, vals, NULL, NULL, NULL);

	fmiInteger* intOutputs = fmu.getIntegerOutputs();
	BOOST_CHECK_EQUAL(intOutputs[0], 0);

	// Predict one step -> on event
	fmiTime time = fmu.predictState(0.0);
	BOOST_CHECK_CLOSE( time, 0.5, 1.0*100*EPS_TIME );

	time = fmu.updateStateFromTheRight(0.5);
	BOOST_CHECK_CLOSE( time, 0.5, 1.0*100*EPS_TIME );

	// Check outputs
	intOutputs = fmu.getIntegerOutputs();
	BOOST_CHECK_EQUAL(intOutputs[0], 1);

	// Predict another step -> end-of-horizon event
	time = fmu.predictState(0.5);
	BOOST_CHECK_CLOSE( time, 1.5, 1.0*100*EPS_TIME );

	time = fmu.updateStateFromTheRight(1.5);
	BOOST_CHECK_CLOSE( time, 1.5, 1.0*100*EPS_TIME );

	// Check outputs again
	intOutputs = fmu.getIntegerOutputs();
	BOOST_CHECK_EQUAL(intOutputs[0], 1);

	// Predict final step -> off event
	time = fmu.predictState(1.5);
	BOOST_CHECK_CLOSE( time, 2.0, 1.0*100*EPS_TIME );

	time = fmu.updateStateFromTheRight(2.0);
	BOOST_CHECK_CLOSE( time, 2.0, 1.0*100*EPS_TIME );

	// Check outputs another time
	intOutputs = fmu.getIntegerOutputs();
	BOOST_CHECK_EQUAL(intOutputs[0], 0);
}

/**
 * @brief Tests the time event handling
 */
BOOST_AUTO_TEST_CASE( test_fmu_time_event )
{
	std::string MODELNAME( "step_t0" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[1] = { "t0" };
	double vals[1] = { 0.5 };
	const double starttime = 0.0;
	const double stepsize = 0.3;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[1] = { "x" };

	fmu.defineRealOutputs( outputs, 1 );

	int status = fmu.init( "step_t0", vars, vals, 1, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double* result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );

	double time = fmu.sync( -4711.0, 0.0 );
	BOOST_CHECK_SMALL( time - 0.5, EPS_TIME );

	result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );

	time = fmu.sync( 0.0 , time );
	BOOST_CHECK_SMALL( time - 0.5 - horizon, EPS_TIME );

	result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );
}

/**
 * @brief predicts the next event and calculates the limit from the right
 * @details After the event predition a prediction step without any events is
 * performed.
 */
BOOST_AUTO_TEST_CASE( test_updateStateFromTheRight )
{
	std::string MODELNAME( "step_t0" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[1] = { "t0" };
	double vals[1] = { 0.5 };
	const double starttime = 0.0;
	const double stepsize = 0.3;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[1] = { "x" };

	fmu.defineRealOutputs( outputs, 1 );

	int status = fmu.init( "step_t0", vars, vals, 1, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double* result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );

	double time = fmu.predictState( starttime );
	BOOST_CHECK_SMALL( time - 0.5, EPS_TIME );

	time = fmu.updateStateFromTheRight( time );
	BOOST_CHECK_SMALL( time - 0.5, 2 * EPS_TIME );

	result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 1.0 );

}

/**
 * @brief Test the init function's error handling
 */
BOOST_AUTO_TEST_CASE( test_init_error_handling_real )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string vars[3] = { "k", "x", "ERR" };
	double vals[3] = { 10.0, 0.0, 0.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	int status = fmu.init( "zigzag1", vars, vals, 3, starttime, horizon, stepsize, intstepsize );
	BOOST_CHECK_EQUAL( status , 0 );
}

/**
 * @brief Test the init function's error handling
 */
BOOST_AUTO_TEST_CASE( test_init_error_handling_integer )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string varsReal[2] = { "k", "x" };
	double valsReal[2] = { 10.0, 0.0 };

	std::string varsInt[1] = { "ERR" };
	fmiInteger valsInt[1] = { 1 };

	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	int status = fmu.init( "zigzag1", 
		varsReal, valsReal, 2, 
		varsInt, valsInt, 1, 
		NULL, NULL, 0, 
		NULL, NULL, 0, 
		starttime, horizon, stepsize, intstepsize );

	BOOST_CHECK_EQUAL( status , 0 );

}

/**
 * @brief Test the init function's error handling
 */
BOOST_AUTO_TEST_CASE( test_init_error_handling_boolean )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string varsReal[2] = { "k", "x" };
	double valsReal[2] = { 10.0, 0.0 };

	std::string varsBoolean[1] = { "ERR" };
	fmiBoolean valsBoolean[1] = { 1 };

	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	int status = fmu.init( "zigzag1", 
		varsReal, valsReal, 2, 
		NULL, NULL, 0, 
		varsBoolean, valsBoolean, 1, 
		NULL, NULL, 0, 
		starttime, horizon, stepsize, intstepsize );

	BOOST_CHECK_EQUAL( status , 0 );

}

/**
 * @brief Test the init function's error handling
 */
BOOST_AUTO_TEST_CASE( test_init_error_handling_string )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	std::string varsReal[2] = { "k", "x" };
	double valsReal[2] = { 10.0, 0.0 };

	std::string varsStr[1] = { "ERR" };
	std::string valsStr[1] = { std::string("Nope") };

	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	int status = fmu.init( "zigzag1", 
		varsReal, valsReal, 2, 
		NULL, NULL, 0, 
		NULL, NULL, 0, 
		varsStr, valsStr, 1, 
		starttime, horizon, stepsize, intstepsize );

	BOOST_CHECK_EQUAL( status , 0 );

}

/** @brief Check the event's timing using FMU zigzag2 */
BOOST_AUTO_TEST_CASE( test_fmu_indicated_event_timing2 )
{
	std::string MODELNAME( "zigzag2" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME,
			    IntegratorType::dp );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 1.0, 0.0 };
	const double starttime = 0.0;
	const double stepsize = 0.11;

	const double horizon = 10 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[2] = { "x", "der(x)" };

	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double* result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );
	BOOST_CHECK_EQUAL( result[1], 1.0 );

	// Get first event at t=1.0
	double time = fmu.sync( -42.0, starttime );
	BOOST_CHECK_CLOSE( time, 1.0, 1.0*100*EPS_TIME );

	// Get end of horizon event at t=2.1
	time = fmu.sync( starttime, time );
	BOOST_CHECK_CLOSE( time, 2.1, 2*2.1*100*EPS_TIME );
}

/** @brief: Simulate zigzag2 from t = 0 to t = 1 */
BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_3 )
{
	std::string MODELNAME( "zigzag2" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME,
			    IntegratorType::dp );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 1.0, 0.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[2] = { "x", "der(x)" };

	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double* result = fmu.getRealOutputs();
	BOOST_CHECK_EQUAL( result[0], 0.0 );
	BOOST_CHECK_EQUAL( result[1], 1.0 );

	double time = starttime;
	double next = fmu.sync( -42.0, time );
	double oldnext;
	BOOST_REQUIRE_EQUAL( next, horizon );

	while ( time + stepsize - 1.0  < EPS_TIME ) {
		oldnext = next;
		next = fmu.sync( time, std::min( time + stepsize, next ) );
		result = fmu.getRealOutputs();
		time = std::min( time + stepsize, oldnext );
		if ( std::abs( time - 0.5 ) < 1e-6 ) {
			BOOST_CHECK_CLOSE( result[0], 0.5, 1e-4 );
		}
	}
	result = fmu.getRealOutputs();
	BOOST_CHECK_SMALL( time - 1.0, stepsize/2 );
	BOOST_CHECK_CLOSE( result[0], 1.0, 1e-4 );
}


BOOST_AUTO_TEST_CASE( test_fmu_check_sync_times_2 )
{
	std::string MODELNAME( "zigzag2" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );

	const double start_time = 0.0;
	const double stop_time = 4.0;
	const double step_size = 0.3;

	const double horizon = 2 * step_size;
	const double int_step_size = step_size/2;

	std::string init_vars[2] = { "k", "x" };
	double init_vals[2] = { 1.0, 0.0 };

	std::string outputs[2] = { "x", "der(x)" };
	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", init_vars, init_vals, 2, start_time, horizon, step_size, int_step_size );
	BOOST_REQUIRE_EQUAL( status, 1 );

	double time = start_time;
	double next = start_time;
	double old_next;

	std::vector<double> sync_times;

	while ( time - stop_time  < EPS_TIME ) {
		old_next = next;
		next = fmu.sync( time, std::min( time + step_size, next ) );
		time = std::min( time + step_size, old_next );
		sync_times.push_back( time );
	}

	double expected_sync_times[15] = { 0., .3, .6, .9, 1., 1.3, 1.6, 1.9, 2.2, 2.5, 2.8, 3., 3.3, 3.6, 3.9 };

	for ( int i = 0; i < 15; ++i )
		BOOST_CHECK_CLOSE( sync_times[i], expected_sync_times[i], 1e-7 );
}

/** 
 * @brief Tests the standard operation of syncStates 
 * @brief The operation doesn't rely on the sync operation shortcut. Hence, 
 * the update operation needs to be called manually. 
 */
BOOST_AUTO_TEST_CASE(test_standard_sync_states) {
	std::string MODELNAME("dxiskx");
	IncrementalFMU fmu(FMU_URI_PRE + MODELNAME, MODELNAME);

	std::string varIn[] = { "u" };
	fmiReal varInImage[] = { 1.0 };
	std::string varOut[] = { "x" };

	fmu.defineRealInputs(varIn, sizeof(varIn) / sizeof(varIn[0]));
	fmu.defineRealOutputs(varOut, sizeof(varOut) / sizeof(varOut[0]));

	int status = fmu.init("dxiskx", varIn, varInImage, sizeof(varIn) / sizeof(varIn[0]), 0.0, 1.0, 0.1, 0.1);
	BOOST_REQUIRE_EQUAL(status, 1);

	fmiTime predictedEvTime = fmu.predictState(0.0);
	BOOST_CHECK_CLOSE(predictedEvTime, 1.0, 0.01);

	fmiTime ctime = fmu.updateState(0.5);
	BOOST_CHECK_CLOSE(ctime, 0.5, 0.01);

	// Fetch the output and check it
	fmiReal *varOutImage = fmu.getRealOutputs();
	BOOST_REQUIRE(varOutImage != NULL);
	BOOST_CHECK_CLOSE(varOutImage[0], 1.5, 0.1);

	// Set some new inputs at the current time of the model (0.5)
	varInImage[0] = -1.0;
	fmu.syncState(0.5, varInImage, NULL, NULL, NULL);

	predictedEvTime = fmu.predictState(0.5);
	BOOST_CHECK_CLOSE(predictedEvTime, 1.5, 0.01);

	ctime = fmu.updateStateFromTheRight(1.5);
	BOOST_CHECK_CLOSE(ctime, 1.5, 0.01);

	// Fetch the output and check it
	varOutImage = fmu.getRealOutputs();
	BOOST_REQUIRE(varOutImage != NULL);
	BOOST_CHECK_CLOSE(varOutImage[0], 0.5, 0.1);
}

/**
 * @brief Tests the syncSate operation which sets the state to the beginning.
 * @details Initializes the dxiskx FMU with a slope of 0.0 and starts a 
 * prediction. The FMU is still at a time of 0.0 and new inputs arrive. 
 * Afterwards, a new prediction is conducted which is finally taken.
 */
BOOST_AUTO_TEST_CASE(test_sync_state_to_beginning) {
	std::string MODELNAME("dxiskx");
	IncrementalFMU fmu(FMU_URI_PRE + MODELNAME, MODELNAME);

	std::string varIn[] = { "u" };
	fmiReal varInImage[] = { 0.0 };
	std::string varOut[] = { "x" };

	fmu.defineRealInputs(varIn, sizeof(varIn) / sizeof(varIn[0]));
	fmu.defineRealOutputs(varOut, sizeof(varOut) / sizeof(varOut[0]));

	int status = fmu.init("dxiskx", varIn, varInImage, sizeof(varIn) / sizeof(varIn[0]), 0.0, 1.0, 0.1, 0.1);
	BOOST_REQUIRE_EQUAL(status, 1);

	fmiTime predictedEvTime = fmu.predictState(0.0);
	BOOST_CHECK_CLOSE(predictedEvTime, 1.0, 0.01);

	fmiTime ctime = fmu.updateState(0.0);
	BOOST_CHECK_CLOSE(ctime, 0.0, 0.01);

	// Set some new inputs at the current time of the model (0.0)
	varInImage[0] = -1.0;
	fmu.syncState(0.0, varInImage, NULL, NULL, NULL);

	predictedEvTime = fmu.predictState(0.0);
	BOOST_CHECK_CLOSE(predictedEvTime, 1.0, 0.01);

	ctime = fmu.updateState(1.0);
	BOOST_CHECK_CLOSE(ctime, 1.0, 0.01);

	// Fetch the output and check it
	fmiReal *varOutImage = fmu.getRealOutputs();
	BOOST_REQUIRE(varOutImage != NULL);
	// Use "+ 1.0" to overcome numerical errors
	BOOST_CHECK_CLOSE(varOutImage[0] + 1.0, 0.0 + 1.0, 0.1);
}