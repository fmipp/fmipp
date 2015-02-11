// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#include <import/base/include/FMUModelExchange.h>
#include <import/base/include/CallbackFunctions.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMUModelExchange
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <iostream>

namespace {

	unsigned int iLogger = 0;

	void customLogger( fmiComponent c, fmiString instanceName, fmiStatus status,
			   fmiString category, fmiString message, ... )
	{
		iLogger++;
	}

}


BOOST_AUTO_TEST_CASE( test_fmu_load_faulty )
{
	std::string MODELNAME( "XYZ" );
	FMUModelExchange fmu( "ABC", MODELNAME, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "xyz", fmiFalse );	
	BOOST_REQUIRE( status == fmiError );
}

BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
}

BOOST_AUTO_TEST_CASE( test_fmu_instantiate )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );
}

BOOST_AUTO_TEST_CASE( test_fmu_initialize )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );
	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );
}

BOOST_AUTO_TEST_CASE( test_fmu_getvalue )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );
	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );
	fmiReal x;
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_REQUIRE( x == 0.0 );
}

BOOST_AUTO_TEST_CASE( test_fmu_setvalue )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );
	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );
	status = fmu.setValue( "x0", 0.5 );
	BOOST_REQUIRE( status == fmiOK );
	fmiReal x0;
	status = fmu.getValue( "x0", x0 );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_REQUIRE( x0 == 0.5 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_1 )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 1.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.0025;
	fmiReal tstop = 1.0;
	fmiReal x;

	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmiOK );
	}

	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_REQUIRE( std::abs( x - 1.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_1_stop_before_event )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 1.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.0025;
	fmiReal tstop = 1.0;
	fmiReal x;

	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmiOK );
	}

	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_REQUIRE( std::abs( x - 1.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_2_stop_before_event )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 2.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.025;
	fmiReal tstop = 1.0;
	fmiReal x;
	fmiReal dx;

	while ( t < tstop ) {
		t = fmu.integrate( std::min( t + stepsize, tstop ) );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmiOK );
		status = fmu.getValue( "der(x)", dx);
		BOOST_REQUIRE( status == fmiOK );
		if ( fmu.getEventFlag() ) {
			t = fmu.getTime();
			BOOST_REQUIRE( std::abs( t - 0.5 ) < EPS_TIME );
			BOOST_REQUIRE( dx == 2.0 );
			fmu.stepOverEvent();
			t = fmu.getTime();
			BOOST_REQUIRE( std::abs( t - 0.5 ) < EPS_TIME );
			status = fmu.getValue( "der(x)", dx);
			BOOST_REQUIRE( status == fmiOK );
			BOOST_REQUIRE( dx == -2.0 );
			fmu.setEventFlag( fmiFalse );
		}
	}

	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_REQUIRE( std::abs( x - 0.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_10 )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 10.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.025;
	fmiReal tstop = 1.0;
	fmiReal x;
	fmiReal dx;
	int eventctr = 0;

	while ( t < tstop ) {
		t = fmu.integrate( std::min( t + stepsize, tstop ) );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmiOK );
		status = fmu.getValue( "der(x)", dx);
		BOOST_REQUIRE( status == fmiOK );
		if ( fmu.getEventFlag() ) {
			eventctr++;
			fmu.setEventFlag( fmiFalse );
		}
	}

	BOOST_REQUIRE( eventctr == 5 );
	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_REQUIRE( std::abs( x - 0.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_10_stop_before_event )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 10.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.025;
	fmiReal tstop = 1.0;
	fmiReal x;
	fmiReal dx;
	int eventctr = 0;

	while ( t < tstop ) {
		t = fmu.integrate( std::min( t + stepsize, tstop ) );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmiOK );
		status = fmu.getValue( "der(x)", dx);
		BOOST_REQUIRE( status == fmiOK );
		if ( fmu.getEventFlag() ) {
			eventctr++;
			fmu.setEventFlag( fmiFalse );
		}
	}

	BOOST_REQUIRE( eventctr == 5 );
	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_REQUIRE( std::abs( x - 0.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_find_event )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 2.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.0025;
	fmiReal tstop = 1.0;
	fmiReal x;
	int eventctr = 0;

	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		if ( fmu.getEventFlag() ) {
			BOOST_REQUIRE( std::abs( t - 0.5 ) < 0.0025 );
			eventctr++;
			fmu.setEventFlag( fmiFalse );
		}
	}

	BOOST_REQUIRE( eventctr == 1 );
	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_REQUIRE( std::abs( x - 0.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_find_time_event )
{
	std::string MODELNAME( "step_t0" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "step_t01", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "t0", 0.5 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.0025;
	fmiReal tstop = 1.0;
	fmiReal x;
	int ctr = 0;
	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmiOK );
		if ( t < 0.5 ) {
			BOOST_REQUIRE( x == 0 );
		} else {
			BOOST_REQUIRE( x == 1 );
		}
		if ( ctr++ > 450 ) break;
	}
}

BOOST_AUTO_TEST_CASE( test_fmu_find_time_event_stop_before_event )
{
	std::string MODELNAME( "step_t0" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "step_t01", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "t0", 0.5 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.0025;
	fmiReal tstop = 1.0;
	fmiReal x;

	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmiOK );
		if ( t < 0.5 ) {
			BOOST_REQUIRE( x == 0 );
		} else if ( t== 0.5 ) {
			BOOST_REQUIRE( x == 0 );
			fmu.stepOverEvent();
			status = fmu.getValue( "x", x );
			BOOST_REQUIRE( status == fmiOK );
			BOOST_REQUIRE( x == 1 );
			t = fmu.getTime();
			BOOST_REQUIRE( t == 0.5 );
		} else {
			BOOST_REQUIRE( x == 1 );
		}
	}
}


BOOST_AUTO_TEST_CASE( test_fmu_find_exact_time_event )
{
	std::string MODELNAME( "step_t0" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "step_t01", fmiFalse );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "t0", 0.5 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal x;

	t = fmu.integrate( 0.6 );
	BOOST_REQUIRE( std::abs( t - 0.5 ) < EPS_TIME );

	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_REQUIRE( x == 0.0 );

}


BOOST_AUTO_TEST_CASE( test_fmu_logger )
{
	std::string MODELNAME( "step_t0" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );

	fmu.setCallbacks( customLogger,
			  callback::allocateMemory,
			  callback::freeMemory );

	fmiStatus status = fmu.instantiate( "step_t01", fmiTrue );
	BOOST_REQUIRE( status == fmiOK );

	for ( unsigned int checkLogger = 1; checkLogger < 10; ++checkLogger ) {
		fmu.setTime( 0. );
		BOOST_REQUIRE( checkLogger == iLogger );
	}
}
