// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#include <import/base/include/FMUModelExchange_v1.h>
#include <import/base/include/CallbackFunctions.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMUModelExchange
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <iostream>

using namespace fmi_1_0;

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
	FMUModelExchange fmu( "ABC", MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "xyz" );	
	BOOST_REQUIRE( status == fmiError );
}

BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
}

BOOST_AUTO_TEST_CASE( test_fmu_instantiate )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmiOK );
}

BOOST_AUTO_TEST_CASE( test_fmu_initialize )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmiOK );
	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );
}

BOOST_AUTO_TEST_CASE( test_fmu_getvalue )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "step_t01" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "step_t01" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiTrue, EPS_TIME );
	fmiStatus status = fmu.instantiate( "step_t01" );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );

	fmu.setCallbacks( customLogger,
			  callback::allocateMemory,
			  callback::freeMemory );

	fmiStatus status = fmu.instantiate( "step_t01" );
	BOOST_REQUIRE( status == fmiOK );

	for ( unsigned int checkLogger = 1; checkLogger < 10; ++checkLogger ) {
		fmu.setTime( 0. );
		BOOST_REQUIRE( checkLogger == iLogger );
	}
}


BOOST_AUTO_TEST_CASE( test_fmu_jacobian_linear_stiff )
{
	std::string MODELNAME( "linear_stiff" );
	std::string fmuPath( "numeric/" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME,
			      fmiFalse, fmiFalse, EPS_TIME );

	fmu.instantiate( "linear_stiff1" );
	fmu.initialize();

	// since the FMU is of type 1.0, expect the Jacobian not to be available
	BOOST_CHECK( fmu.providesJacobian() == false );

	// since the rhs of the FMU does not depend on the time and state, expect the
	// numerical jacobian to be the same as the actual jacobian
	double* Jac  = new double[ 4 ];
	double* x    = new double[ 2 ];
	double* dfdt = new double[ 2 ];

	// use the starting values of the fmu for testing
	double t = fmu.getTime();
	fmu.getContinuousStates( x );
	fmu.getNumericalJacobian( Jac, x, dfdt, t );

	// test with a tolerance of 1.0e-9 percent ( roundoff errors )
	BOOST_CHECK_CLOSE( Jac[0],   998, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[1],  1998, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[2],  -999, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[3], -1999, 1.0e-9 );

	// since the rhs is not time dependend, expect dfdt to be zero
	BOOST_CHECK_SMALL( dfdt[0], 1.0e-8 );
	BOOST_CHECK_SMALL( dfdt[1], 1.0e-8 );

	delete Jac;
	delete x;
	delete dfdt;
}


BOOST_AUTO_TEST_CASE( test_fmu_intergrator_properties )
{
	// load the zigzag FMU
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	status = fmu.initialize();
	BOOST_REQUIRE_EQUAL( status, fmiOK );

	// get the properties of the defualt integrator
	Integrator::Properties properties = fmu.getIntegratorProperties();

	// check if the type is right
#ifdef USE_SUNDIALS
	BOOST_CHECK_EQUAL( properties.type, IntegratorType::bdf );
#else
	BOOST_CHECK_EQUAL( properties.type, IntegratorType::dp );
#endif
	// change the integrator to euler
	properties.type = IntegratorType::eu;
	fmu.setIntegratorProperties( properties );

	// check whether the set was sucessfull
	BOOST_CHECK_EQUAL( properties.type, IntegratorType::eu );

	// since euler is non adaptive, the tolerances should be set to infinity
	// during the call to setIntegratorProperties (passed by reference)
	BOOST_CHECK( isinf( properties.abstol ) );
	BOOST_CHECK( isinf( properties.reltol ) );

	// check whether the name has been set right
	BOOST_CHECK_EQUAL( properties.name, "Euler" );

	// set yet another integrator with custom tolerance
	properties.type   = IntegratorType::ck;
	properties.abstol = 1.0e-13;
	fmu.setIntegratorProperties( properties );

	// check whether abstol still has the custom tolerance specified above
	BOOST_CHECK_EQUAL( properties.abstol, 1.0e-13 );
	// expect reltol to be the default value since it was inf during the call to setProperties
	BOOST_CHECK_EQUAL( properties.reltol, 1.0e-6  );
}
