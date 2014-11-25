#include <stdlib.h>
#include <common/fmi_v1.0/fmiModelTypes.h>
#include <import/utility/include/IncrementalFMU.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testIncrementalFMU
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <iostream>



BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
}

BOOST_AUTO_TEST_CASE( test_fmu_init )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 10.0, 1.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE( status == 1 );
}

BOOST_AUTO_TEST_CASE( test_fmu_getrealoutputs )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 10.0, 1.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[2] = { "x", "der(x)" };

	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE( status == 1 );

	double* result = fmu.getRealOutputs();
	BOOST_REQUIRE( result[0] == 0.0 && result[1] == 10.0 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_1 )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 1.0, 0.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[2] = { "x", "der(x)" };

	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE( status == 1 );

	double* result = fmu.getRealOutputs();
	BOOST_REQUIRE( result[0] == 0.0 && result[1] == 1.0 );

	double time = starttime;
	double next = fmu.sync( -42.0, time );
	double oldnext;
	BOOST_REQUIRE( next == horizon );

	while ( time + stepsize - 1.0  < EPS_TIME ) {
		oldnext = next;
		next = fmu.sync( time, ( time+stepsize ) > next ? next : ( time+stepsize ) );
		result = fmu.getRealOutputs();
		time = ( time+stepsize ) > oldnext ? oldnext : ( time+stepsize );
		if ( std::abs( time - 0.5 ) < 1e-6 ) {
			BOOST_REQUIRE( std::abs( result[0] - 0.5 ) < 1e-6 );
		}
	}
	result = fmu.getRealOutputs();	
	BOOST_REQUIRE( std::abs( time - 1.0 ) < stepsize/2 );
	BOOST_REQUIRE( std::abs( result[0] - 1.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_2 )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
	std::string vars[2] = { "k", "x" };
	double vals[2] = { 10.0, 0.0 };
	const double starttime = 0.0;
	const double stepsize = 0.0025;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[2] = { "x", "der(x)" };

	fmu.defineRealOutputs( outputs, 2 );

	int status = fmu.init( "zigzag1", vars, vals, 2, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE( status == 1 );

	double* result = fmu.getRealOutputs();
	BOOST_REQUIRE( result[0] == 0.0 && result[1] == 10.0 );

	double time = starttime;
	double next = fmu.sync( -42.0, time );
	double oldnext;
	BOOST_REQUIRE( next == horizon );

	double dx = 10.0;
	int eventctr = 0;

	while ( time - 1.0  < EPS_TIME ) {
		oldnext = next;
		next = fmu.sync( time, ( time+stepsize ) > next ? next : ( time+stepsize ) );
		result = fmu.getRealOutputs();
		time = ( time+stepsize ) > oldnext ? oldnext : ( time+stepsize );
		if ( result[1] != dx ) {
			eventctr++;
			dx = result[1];
		}
	}
	BOOST_REQUIRE( eventctr == 5 );
	
	result = fmu.getRealOutputs();	
	BOOST_REQUIRE( std::abs( time - 1.0 ) < stepsize/2 );
	BOOST_REQUIRE( std::abs( result[0] - 0.0 ) < 1e-6 );
}

/**
 * @brief Tests the time event handling
 */
BOOST_AUTO_TEST_CASE( test_fmu_time_event )
{
	std::string MODELNAME( "step_t0" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
	std::string vars[1] = { "t0" };
	double vals[1] = { 0.5 };
	const double starttime = 0.0;
	const double stepsize = 0.3;

	const double horizon = 2 * stepsize;
	const double intstepsize = stepsize/2;

	std::string outputs[1] = { "x" };

	fmu.defineRealOutputs( outputs, 1 );

	int status = fmu.init( "step_t0", vars, vals, 1, starttime, horizon, stepsize, intstepsize );
	BOOST_REQUIRE( status == 1 );

	double* result = fmu.getRealOutputs();
	BOOST_REQUIRE( result[0] == 0.0 );

	double time = fmu.sync( -4711.0, 0.0 );
	BOOST_REQUIRE( std::abs( time - 0.5 ) < EPS_TIME );

	result = fmu.getRealOutputs();
	BOOST_REQUIRE( result[0] == 0.0 );

	time = fmu.sync( 0.0 , time );
	BOOST_REQUIRE( std::abs( time - 0.5 - horizon ) < EPS_TIME );

	result = fmu.getRealOutputs();
	BOOST_REQUIRE( result[0] == 1.0 );

}

/**
 * @brief Test the init function's error handling
 */
BOOST_AUTO_TEST_CASE( test_init_error_handling_real )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
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
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
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
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
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
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME, EPS_TIME );
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
		varsStr, valsStr, 0, 
		starttime, horizon, stepsize, intstepsize );

	BOOST_CHECK_EQUAL( status , 0 );

}