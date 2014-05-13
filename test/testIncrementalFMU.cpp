#include <stdlib.h>
#include <common/fmi_v1.0/fmiModelTypes.h>
#include <utility/include/IncrementalFMU.h>

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
