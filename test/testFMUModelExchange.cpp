// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#include <import/base/include/FMUModelExchange_v1.h>
#include <import/base/include/ModelDescription.h>
#include <import/base/include/CallbackFunctions.h>
#include <import/base/include/LogBuffer.h>

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
	FMUModelExchange fmu( "ABC", MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "xyz" );	
	BOOST_REQUIRE( status == fmippError );
}

BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
}

BOOST_AUTO_TEST_CASE( test_fmu_load_wrong_model_id )
{
	FMUModelExchange fmu( FMU_URI_PRE + std::string( "zigzag" ), std::string( "foo" ), fmippTrue, fmippFalse, EPS_TIME );
	BOOST_REQUIRE( fmu.getLastStatus() == fmippWarning );
}

BOOST_AUTO_TEST_CASE( test_fmu_getModelDescription_success )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	BOOST_REQUIRE_EQUAL(fmu.getLastStatus(), fmippOK);
	const ModelDescription* ptr = fmu.getModelDescription();
	BOOST_REQUIRE(ptr != NULL);
	BOOST_CHECK_EQUAL(
		ptr->getModelAttributes().get<std::string>("modelName"), 
		"zigzag");
}

BOOST_AUTO_TEST_CASE( test_fmu_getModelDescription_failure )
{
	std::string MODELNAME( "no-fmu-today-my-love-is-gone-away" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	BOOST_REQUIRE_NE(fmu.getLastStatus(), fmippOK);
	const ModelDescription* ptr = fmu.getModelDescription();
	BOOST_CHECK(ptr == NULL);
}


BOOST_AUTO_TEST_CASE( test_fmu_instantiate )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );
}

BOOST_AUTO_TEST_CASE( test_fmu_initialize )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );
	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );
}

BOOST_AUTO_TEST_CASE( test_fmu_getvalue )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );
	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );
	fmippReal x;
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( x == 0.0 );
}

BOOST_AUTO_TEST_CASE( test_fmu_setvalue )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );
	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );
	status = fmu.setValue( "x0", 0.5 );
	BOOST_REQUIRE( status == fmippOK );
	fmippReal x0;
	status = fmu.getValue( "x0", x0 );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( x0 == 0.5 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_1 )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.setValue( "k", 1.0 );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t = 0.0;
	fmippReal stepsize = 0.0025;
	fmippReal tstop = 1.0;
	fmippReal x;

	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmippOK );
	}

	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( std::abs( x - 1.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_1_stop_before_event )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippFalse, fmippTrue, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.setValue( "k", 1.0 );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t = 0.0;
	fmippReal stepsize = 0.0025;
	fmippReal tstop = 1.0;
	fmippReal x;

	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmippOK );
	}

	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( std::abs( x - 1.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_2_stop_before_event )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippFalse, fmippTrue, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.setValue( "k", 2.0 );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t = 0.0;
	fmippReal stepsize = 0.025;
	fmippReal tstop = 1.0;
	fmippReal x;
	fmippReal dx;

	while ( t < tstop ) {
		t = fmu.integrate( std::min( t + stepsize, tstop ) );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmippOK );
		status = fmu.getValue( "der(x)", dx);
		BOOST_REQUIRE( status == fmippOK );
		if ( fmu.getEventFlag() ) {
			t = fmu.getTime();
			BOOST_REQUIRE( std::abs( t - 0.5 ) < EPS_TIME );
			BOOST_REQUIRE( dx == 2.0 );
			fmu.stepOverEvent();
			t = fmu.getTime();
			BOOST_REQUIRE( std::abs( t - 0.5 ) < EPS_TIME );
			status = fmu.getValue( "der(x)", dx);
			BOOST_REQUIRE( status == fmippOK );
			BOOST_REQUIRE( dx == -2.0 );
			fmu.setEventFlag( fmippFalse );
		}
	}

	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( std::abs( x - 0.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_10 )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.setValue( "k", 10.0 );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t = 0.0;
	fmippReal stepsize = 0.025;
	fmippReal tstop = 1.0;
	fmippReal x;
	fmippReal dx;
	int eventctr = 0;

	while ( t < tstop ) {
		t = fmu.integrate( std::min( t + stepsize, tstop ) );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmippOK );
		status = fmu.getValue( "der(x)", dx);
		BOOST_REQUIRE( status == fmippOK );
		if ( fmu.getEventFlag() ) {
			eventctr++;
			fmu.setEventFlag( fmippFalse );
		}
	}

	BOOST_REQUIRE( eventctr == 5 );
	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( std::abs( x - 0.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_10_stop_before_event )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippFalse, fmippTrue, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.setValue( "k", 10.0 );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t = 0.0;
	fmippReal stepsize = 0.025;
	fmippReal tstop = 1.0;
	fmippReal x;
	fmippReal dx;
	int eventctr = 0;

	while ( t < tstop ) {
		t = fmu.integrate( std::min( t + stepsize, tstop ) );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmippOK );
		status = fmu.getValue( "der(x)", dx);
		BOOST_REQUIRE( status == fmippOK );
		if ( fmu.getEventFlag() ) {
			eventctr++;
			fmu.setEventFlag( fmippFalse );
		}
	}

	BOOST_REQUIRE( eventctr == 5 );
	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( std::abs( x - 0.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulations_inputs )
{
	std::string MODELNAME( "dxiskx" );
	FMUModelExchange fmu1( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	FMUModelExchange fmu2( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu1.instantiate( "dxiskx1" );
	BOOST_REQUIRE( status == fmippOK );
	status = fmu2.instantiate( "dxiskx2" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu1.setValue( "k", 1.0 );
	BOOST_REQUIRE( status == fmippOK );
	status = fmu1.setValue( "x0", 1.0 );
	BOOST_REQUIRE( status == fmippOK );
	status = fmu1.initialize();
	BOOST_REQUIRE( status == fmippOK );

	status = fmu2.setValue( "k", -1.0 );
	BOOST_REQUIRE( status == fmippOK );
	status = fmu2.setValue( "x0", 0.0 );
	BOOST_REQUIRE( status == fmippOK );
	status = fmu2.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t1 = 0.0, t2 = 0.0;
	fmippReal stepsize = 1e-5;
	fmippReal tstop = 1.0;
	fmippReal x1 = 1.0, x2 = 0.0;
	fmippReal dx1, dx2;

	while ( t1 < tstop && t2 < tstop ) {
		status = fmu1.setValue( "u", x2 );
		BOOST_REQUIRE( status == fmippOK );
		t1 = fmu1.integrate( std::min( t1 + stepsize, tstop ) );
		status = fmu1.getValue( "x", x1 );
		BOOST_REQUIRE( status == fmippOK );
		status = fmu1.getValue( "der(x)", dx1);
		BOOST_REQUIRE( status == fmippOK );
		status = fmu2.setValue( "u", x1 );
		BOOST_REQUIRE( status == fmippOK );
		t2 = fmu2.integrate( std::min( t2 + stepsize, tstop ) );
		status = fmu2.getValue( "x", x2 );
		BOOST_REQUIRE( status == fmippOK );
		status = fmu2.getValue( "der(x)", dx2);
		BOOST_REQUIRE( status == fmippOK );
	}

	t1 = fmu1.getTime();
	BOOST_REQUIRE( std::abs( t1 - tstop ) < stepsize/2 );
	t2 = fmu2.getTime();
	BOOST_REQUIRE( std::abs( t2 - tstop ) < stepsize/2 );
	status = fmu1.getValue( "x", x1 );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( std::abs( x1 - std::cos( tstop ) ) < 1e-5 );
	status = fmu2.getValue( "x", x2 );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( std::abs( x2 - (-std::sin( tstop )) ) < 1e-5 );
}

BOOST_AUTO_TEST_CASE( test_fmu_find_event )
{
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.setValue( "k", 2.0 );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t = 0.0;
	fmippReal stepsize = 0.0025;
	fmippReal tstop = 1.0;
	fmippReal x;
	int eventctr = 0;

	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		if ( fmu.getEventFlag() ) {
			BOOST_REQUIRE( std::abs( t - 0.5 ) < 0.0025 );
			eventctr++;
			fmu.setEventFlag( fmippFalse );
		}
	}

	BOOST_REQUIRE( eventctr == 1 );
	t = fmu.getTime();
	BOOST_REQUIRE( std::abs( t - tstop ) < stepsize/2 );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( std::abs( x - 0.0 ) < 1e-6 );
}

BOOST_AUTO_TEST_CASE( test_fmu_find_time_event )
{
	std::string MODELNAME( "step_t0" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippFalse, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "step_t01" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.setValue( "t0", 0.5 );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t = 0.0;
	fmippReal stepsize = 0.0025;
	fmippReal tstop = 1.0;
	fmippReal x;
	int ctr = 0;
	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmippOK );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippFalse, fmippTrue, EPS_TIME );
	fmippStatus status = fmu.instantiate( "step_t01" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.setValue( "t0", 0.5 );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t = 0.0;
	fmippReal stepsize = 0.0025;
	fmippReal tstop = 1.0;
	fmippReal x;

	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE( status == fmippOK );
		if ( t < 0.5 ) {
			BOOST_REQUIRE( x == 0 );
		} else if ( t== 0.5 ) {
			BOOST_REQUIRE( x == 0 );
			fmu.stepOverEvent();
			status = fmu.getValue( "x", x );
			BOOST_REQUIRE( status == fmippOK );
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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippFalse, fmippTrue, EPS_TIME );
	fmippStatus status = fmu.instantiate( "step_t01" );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.setValue( "t0", 0.5 );
	BOOST_REQUIRE( status == fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmippOK );

	fmippReal t = 0.0;
	fmippReal x;

	t = fmu.integrate( 0.6 );
	BOOST_REQUIRE( std::abs( t - 0.5 ) < EPS_TIME );

	status = fmu.getValue( "x", x );
	BOOST_REQUIRE( status == fmippOK );
	BOOST_REQUIRE( x == 0.0 );
}

BOOST_AUTO_TEST_CASE( test_fmu_logger )
{
	std::string MODELNAME( "step_t0" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );

	fmu.setCallbacks( customLogger,
			  callback::allocateMemory,
			  callback::freeMemory );

	fmippStatus status = fmu.instantiate( "step_t01" );
	BOOST_REQUIRE( status == fmippOK );

	for ( unsigned int checkLogger = 1; checkLogger < 10; ++checkLogger ) {
		fmu.setTime( 0. );
		BOOST_REQUIRE( checkLogger == iLogger );
	}
}

BOOST_AUTO_TEST_CASE( test_fmu_log_buffer )
{
	// Retrieve the global instance of the log buffer.
	LogBuffer& logBuffer = LogBuffer::getLogBuffer();

	// Activate the global log buffer.
	logBuffer.activate();
	BOOST_REQUIRE_EQUAL( logBuffer.isActivated(), true );

	// load the zigzag FMU
	std::string MODELNAME( "zigzag" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );

	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE_EQUAL( status, fmippOK );

	status = fmu.initialize();
	BOOST_REQUIRE_EQUAL( status, fmippOK );

	std::string logMessage = logBuffer.readFromBuffer();
	std::string expectedMessage( "zigzag1 [INSTANTIATE_MODEL]: instantiation successful\nzigzag1 [INITIALIZE]: initialization successful\n");

	BOOST_REQUIRE_MESSAGE( logMessage == expectedMessage,
			       "log message:\n>>>" << logMessage << "<<<\n" <<
			       "expected message:\n>>" << expectedMessage << "<<<\n" );

	// Clear the global log buffer.
	logBuffer.clear();
	BOOST_REQUIRE_MESSAGE( logBuffer.readFromBuffer() == std::string(),
			       "global log buffer has not been cleared properly" );

	// Deactivate the global log buffer.
	logBuffer.deactivate();
	BOOST_REQUIRE_EQUAL( logBuffer.isActivated(), false );
}

BOOST_AUTO_TEST_CASE( test_fmu_jacobian_linear_stiff )
{
	std::string MODELNAME( "linear_stiff" );
	std::string fmuPath( "numeric/" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME,
			      fmippFalse, fmippFalse, EPS_TIME );

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
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmippTrue, fmippFalse, EPS_TIME );
	fmippStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE_EQUAL( status, fmippOK );
	status = fmu.initialize();
	BOOST_REQUIRE_EQUAL( status, fmippOK );

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
	BOOST_CHECK_EQUAL( properties.abstol, std::numeric_limits<double>::infinity() );
	BOOST_CHECK_EQUAL( properties.reltol, std::numeric_limits<double>::infinity() );

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
