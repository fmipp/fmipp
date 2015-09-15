// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#include <import/utility/include/RollbackFMU.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testRollbackFMU
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <iostream>



BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string MODELNAME( "zigzag" );
	RollbackFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_without_rollback )
{
	std::string MODELNAME( "zigzag" );
	RollbackFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
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
	}

	t = fmu.getTime();
	BOOST_REQUIRE_MESSAGE( std::abs( t - tstop ) < stepsize/2, "t = " << t );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "status = " << status );
	// with an eventsearchprecision of 1.0e-4, require the same accuracy for x.
	BOOST_REQUIRE_MESSAGE( std::abs( x - 1.0 ) < 1e-4, "x = " << x );

}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_with_rollback_1 )
{
	std::string MODELNAME( "zigzag" );
	RollbackFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 1.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.025;
	fmiReal tstop = 0.5;
	fmiReal x;

	// Integrate.
	while ( ( t + stepsize ) - tstop < EPS_TIME ) {

		// Make integration step.
		fmu.integrate( t + stepsize );

		// Enforce rollback.
		fmu.integrate( t + 0.5*stepsize );
		t = fmu.integrate( t + stepsize );

		status = fmu.getValue( "x", x );
	}

	// Check integration results.
	t = fmu.getTime();
	BOOST_REQUIRE_MESSAGE( std::abs( t - tstop ) < stepsize/2, "t = " << t );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "status = " << status );
	BOOST_REQUIRE_MESSAGE( std::abs( x - 0.5 ) < 1e-6, "x = " << x );
}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_with_rollback_2 )
{
	std::string MODELNAME( "zigzag" );
	RollbackFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 1.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.0025;
	fmiReal tstop = 0.5;
	fmiReal x;

	// Save initial state as rollback state.
	fmu.saveCurrentStateForRollback();

	// Integrate.
	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
	}

	// Check integration results.
	t = fmu.getTime();
	BOOST_REQUIRE_MESSAGE( std::abs( t - tstop ) < stepsize/2, "t = " << t );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "status = " << status );
	BOOST_REQUIRE_MESSAGE( std::abs( x - 0.5 ) < 1e-6, "x = " << x );

	// Enforce rollback.
	t = 0.0;

	// Redo integration.
	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
	}

	// Check integration results again.
	t = fmu.getTime();
	BOOST_REQUIRE_MESSAGE( std::abs( t - tstop ) < stepsize/2, "t = " << t );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "status = " << status );
	BOOST_REQUIRE_MESSAGE( std::abs( x - 0.5 ) < 1e-6, "x = " << x );

}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_without_rollback2 )
{
	std::string MODELNAME( "zigzag2" );
	RollbackFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
	fmiStatus status = fmu.instantiate( "zigzag21" );
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
	}

	t = fmu.getTime();
	BOOST_REQUIRE_MESSAGE( std::abs( t - tstop ) < stepsize/2, "t = " << t );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "status = " << status );
	// with an eventsearchprecision of 1.0e-4, require the same accuracy for x.
	BOOST_REQUIRE_MESSAGE( std::abs( x - 1.0 ) < 1e-4, "x = " << x );

}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_with_rollback_11 )
{
	std::string MODELNAME( "zigzag2" );
	RollbackFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 1.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.025;
	fmiReal tstop = 0.5;
	fmiReal x;

	// Integrate.
	while ( ( t + stepsize ) - tstop < EPS_TIME ) {

		// Make integration step.
		fmu.integrate( t + stepsize );

		// Enforce rollback.
		fmu.integrate( t + 0.5*stepsize );
		t = fmu.integrate( t + stepsize );

		status = fmu.getValue( "x", x );
	}

	// Check integration results.
	t = fmu.getTime();
	BOOST_REQUIRE_MESSAGE( std::abs( t - tstop ) < stepsize/2, "t = " << t );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "status = " << status );
	BOOST_REQUIRE_MESSAGE( std::abs( x - 0.5 ) < 1e-6, "x = " << x );
}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_with_rollback_21 )
{
	std::string MODELNAME( "zigzag2" );
	RollbackFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
	fmiStatus status = fmu.instantiate( "zigzag1" );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.setValue( "k", 1.0 );
	BOOST_REQUIRE( status == fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );

	fmiReal t = 0.0;
	fmiReal stepsize = 0.0025;
	fmiReal tstop = 0.5;
	fmiReal x;

	// Save initial state as rollback state.
	fmu.saveCurrentStateForRollback();

	// Integrate.
	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
	}

	// Check integration results.
	t = fmu.getTime();
	BOOST_REQUIRE_MESSAGE( std::abs( t - tstop ) < stepsize/2, "t = " << t );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "status = " << status );
	BOOST_REQUIRE_MESSAGE( std::abs( x - 0.5 ) < 1e-6, "x = " << x );

	// Enforce rollback.
	t = 0.0;

	// Redo integration.
	while ( ( t + stepsize ) - tstop < EPS_TIME ) {
		t = fmu.integrate( t + stepsize );
		status = fmu.getValue( "x", x );
	}

	// Check integration results again.
	t = fmu.getTime();
	BOOST_REQUIRE_MESSAGE( std::abs( t - tstop ) < stepsize/2, "t = " << t );
	status = fmu.getValue( "x", x );
	BOOST_REQUIRE_MESSAGE( status == fmiOK, "status = " << status );
	BOOST_REQUIRE_MESSAGE( std::abs( x - 0.5 ) < 1e-6, "x = " << x );
}
