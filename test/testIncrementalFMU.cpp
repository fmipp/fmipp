// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
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
BOOST_AUTO_TEST_CASE( test_fmu_indicated_event_timing )
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
