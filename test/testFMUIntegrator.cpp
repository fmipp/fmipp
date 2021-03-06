// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#include <import/base/include/FMUModelExchange_v1.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMUIntegrator

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <time.h>

#if defined( WIN32 ) // Windows.
#include <algorithm>
#define fmin min
#define fmax max
#else // Linux, Unix, etc.
#include <cmath>
#endif

using namespace std;
using namespace boost; // for std::cout << boost::format( ... ) % ... % ... ;
using namespace fmi_1_0;

string fmuPath( "numeric/" );

/**
 * runs a simulation of the asymptotic_euler fmu.
 *
 * @param[in ] integratorType 		the integrator to be used
 * @param[in ] integratorName		the name of the integrator as string
 *					( to be printed to terminal )
 * @param[in ] lambda			stiffnes parameter. Big values are more challenging for the
 *					integrator
 * @param[in ] dt			starting step size for the integrator
 * @param[in ] tstop			end time of the simulation
 */
void simulate_asymptotic_sine( IntegratorType integratorType,
			       fmippReal lambda = 10000,
			       fmippReal dt = 0.001,
			       fmippReal tstop = 1.0,
			       fmippReal tolerance = 1.5e-4
			       )
{
	string MODELNAME( "asymptotic_sine" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME,
			      fmippFalse, fmippFalse, EPS_TIME, integratorType );
	string integratorName = fmu.getIntegratorProperties().name;
	fmu.instantiate( "asymptotic_euler1" );
	fmu.initialize();
	fmu.setValue( "lambda", lambda );
	fmippTime time = clock();
	fmu.integrate( tstop, dt );
	time = clock() - time;
	fmippReal x,y;
	fmu.getValue( "x", x );
	fmu.getValue( "y", y );
	fmippReal error = fmax( fabs( x - sin( tstop ) ) , fabs( y - cos( tstop ) ) );

	cout << format("%-20s %-20E %-20s %-20E\n")
		% integratorName % error % "" % time;
	BOOST_CHECK( error < tolerance );

}

BOOST_AUTO_TEST_CASE( test_asymptotic_sine ){
	std::cout << "integrating asymptotic_sine from t = 0 to t = 1. The errors are measured at t = 1.\n\n";

	cout <<	format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "error" % "" % "CPU Time(clock ticks)";

	for ( int i = 0; i < IntegratorType::NSTEPPERS; i++ ){
		IntegratorType type = (IntegratorType) i;
		if ( type == IntegratorType::eu || type == IntegratorType::rk )
			// use easier stiffnes parameter for non adaptive steppers
			simulate_asymptotic_sine( type,  1.0e2 );
		else if ( type == abm ){
			// skip the abm stepper because of a bug in the boost version
			// this skip is not necessary if you use the odeint version from
			// github.
		}
		else
			simulate_asymptotic_sine( type );
	}

	std::cout << "\n";
}

int estimateOrder( IntegratorType integratorType, int nSteps = 1 )
{
	/*
	 * test the accuracy of a stepper by solvoing an ode which has a polynomial solution
	 *
	 * from the general thory about *linear* steppers it follows that the correlation between
	 * exact solutions for polynomials and global trunaciation errors is very high
	 *
	 * to be more precise: whenever a linear stepper is able to solve the ode x'(t) = t^p exactly
	 * for p <= k, then the global error is of order O( h^{k+1} )
	 *
	 * Note however that this test is not very useful for steppers with controlled step sizes
	 * since the low error could also be caused by step size control.
	 *
	 * \FIXME add a *force_const_integration* option for adaptive steppers?
	 *
	 */
	fmippReal error = 0;
	fmippReal x;
	fmippReal tolerance = 1e-15;
	fmippReal p = -1;
	string MODELNAME( "polynomial" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME,
			      fmippFalse, fmippFalse, EPS_TIME, integratorType );

	fmu.instantiate( "polynomial1" );
	fmu.initialize();

	string integratorName = fmu.getIntegratorProperties().name;

	while ( error < tolerance ){
		p++;
		fmu.setValue( "p", p );
		fmu.setTime( 0.0 );
		fmu.setValue( "x", 0.0 );
		for ( int i = 0; i < nSteps; i++ )
			fmu.integrate( 1.0, (unsigned int)nSteps );	// step_size == deltaT means the stepper
									// actually performs just one step
		fmu.getValue( "x", x );
		error = fabs( x - 1.0 );
	}
	int estimatedOrder = p;
	int definedOrder = 0;//fmu.integratorOrder();
	BOOST_CHECK_MESSAGE( definedOrder <= estimatedOrder,
			     "order of " << integratorName << " is too high: "
			     << estimatedOrder << " < " << definedOrder );
	return estimatedOrder;

	return 1;
}

BOOST_AUTO_TEST_CASE( test_polynomial_estimate_order )
{
	for ( int i = 0; i < IntegratorType::NSTEPPERS; i++ ){
		if ( i == IntegratorType::abm ){
			// skip this integrator since the version of odeint which comes with boost 1.57
			// will fail the test otherwise. The test is passed with the github version of
			// odeint tough. ( https://github.com/headmyshoulder/odeint-v2 ).
		}
		else
			estimateOrder( (IntegratorType) i );
	}
}



// simulates the model stiff from tstart to tstop and prints the integration-error
// as well as the cpu time to the console. ts is a parameter of the model which
// determines when the event (including a discontinuouty of the RHS) happens.
void runSimulation( IntegratorType integratorType,
	fmippReal ts, fmippReal tolerance,
	fmippReal k      = 10,
	fmippTime tstart   = 0,
	fmippTime tstop    = 1,
	fmippTime stepsize = 0.0025,
	fmippReal eventSearchPrecision = 1e-15 )
{
	string MODELNAME( "stiff" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME,
			      fmippFalse, fmippFalse, eventSearchPrecision , integratorType );
	fmu.instantiate( "stiff1" );
	fmu.setValue( "ts", ts );
	fmu.setValue( "k" , k  );
	fmu.initialize();

	string integratorName = fmu.getIntegratorProperties().name;

	fmippReal   x, error, maxError;
	fmippStatus status;
	fmippTime   t, t2, tMaxError;

	t        = tstart;
	maxError = 0;

	// measure CPU time by calling clock directly before and after the simulation
	fmippTime time = clock(); // \FIXME probably won't work on windows

	while ( t < tstop ){

		// perform Integrator step
		t2 = t;
		t  = fmu.integrate( fmin( t + stepsize, tstop ) );

		// get values (time and state) after integrator step
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE_MESSAGE( status == fmippOK ,
				       "Could not get Value of x after the integrator step from t = "
				       << t2 << " to t = " << t << " with Integrator "
				       << integratorName << "."
				       );

		// calculate analytical solution and measure the (integration) error
		t2    = fmin( t, 2*ts - t );
		error = fabs( exp( k*t2 )/( exp( k*0.5 ) + exp( k*t2 ) ) - x );

		// if the error is bigger than all previous errors in the simulation, store the time as
		// tMaxError and the error as maxError.
		if ( error > maxError ){
			maxError  = error;
			tMaxError = t;
			if ( error > tolerance ){
				BOOST_CHECK_MESSAGE( error <= tolerance,
						     "Maximum error for Integrator "<< integratorName
						     << " is too big at t = " << t << ". Stopping simulation."
						     );
				return;
			}
		}
	}

	time = clock() - time; // time now stores the length of the simulation in
	                       // clock ticks.

	cout << format("%-20s %-20E %-20E %-20E\n")
		% integratorName % maxError % tMaxError % time;

	BOOST_REQUIRE( fabs( t - tstop ) <= stepsize  );
}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation )
{
	fmippTime ts        = 2.00;       // If ts is bigger than one, there are no events.
	fmippReal tolerance = 1e-4;       // if the difference of the numerical and analytical
	                                // is bigger than this tolerance at one of the
	                                // communication step points, the test fails.

	cout << "Integrating stiff from t = 0 to t = 1 with different integrators."
	     << " The errors are measured at all the communication step points."
	     << endl << endl;

	cout << format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "maxError" % "time of maxError" % "CPU time (clock ticks)";

	for ( int i = 0; i < IntegratorType::NSTEPPERS; i++ )
		runSimulation( (IntegratorType)i, ts, tolerance);
}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_with_events )
{
	fmippReal s         = 0.6;
	fmippTime ts        = 1.0/2.0 + log( s/(1 - s) )/10;
	                                // at time ts, the RHS instantainously swiches its sign.
                                        // It can be expected that the biggest errors happen
	                                // shortly after the event.
	fmippReal tolerance = 1e-4;       // if the difference of the numerical and analytical
	                                // is bigger than this tolerance at one of the
	                                // communication step points, the test fails.

	cout << endl << "Including events into the simulation ( s = "
	     << s << ", ts = " << ts
	     << " ). Errors are again measured at all communication step points."
	     << endl << endl;

	cout << format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "maxError" % "time of maxError" % "CPU time (clock ticks)";

	for ( int i = 0; i < IntegratorType::NSTEPPERS; i++ )
		runSimulation( (IntegratorType)i, ts, tolerance);
}


/**
 *
 * Test a certain integratorStepper with the problem
 *
 * dot( x ) =  998 * x + 1998 * y
 * dot( y ) = -999 * x - 1999 * y,
 *
 * x( t = tstart ) = 1
 * y( t = tstart ) = 0.
 *
 * from tstart ( = 0 ) to tstop ( configurable with default value of 100 ).
 *
 * the errors are measured at the following times:
 *     tstart, tstart+stepsize, tstart+2*stepsize, ... , tstop.
 *
 * The maximum errror as well as the time of the maximum error (tMaxError) are returned to the console using
 * the print functionalities of boost::format
 *
 */
void simulate_linear_stiff( IntegratorType integratorType,
			    fmippReal tolerance,
			    fmippTime dt = .001,
			    fmippTime tstop    = 100,
			    fmippTime stepsize = 10.0
			    )
{
	string MODELNAME( "linear_stiff" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME,
			      fmippFalse, fmippFalse, EPS_TIME , integratorType );
	string integratorName = fmu.getIntegratorProperties().name;
	fmu.instantiate( "linear_stiff1" );
	fmu.initialize();
	fmippReal x, y, error, maxError = 0;
	fmippTime t = 0, tMaxError;

	double time = clock();
	while ( t < tstop ){
		t  = fmu.integrate( fmin( t + stepsize, tstop ), dt );
		fmu.getValue( "x", x );
		x = fabs( x - 2*exp(-t) + exp( -1000*t ) );
		fmu.getValue( "y", y );
		y = fabs( y +   exp(-t) - exp( -1000*t ) );
		error = fmax( x, y );
		if ( error > maxError ){
			maxError = error;
			tMaxError = t;
		}
	}
	time = clock() - time; // time now stores the length of the simulation in
	                       // clock ticks.

	cout << format("%-20s %-20E %-20E %-20E\n")
		% integratorName % maxError % tMaxError % time;
	BOOST_CHECK( maxError < tolerance );
}

BOOST_AUTO_TEST_CASE( test_linear_stiff_system )
{
	std::cout << "\nrunning the problem linear_stiff for different integrators\n\n";
	fmippReal tol = 1.0e-3;

	cout << format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "maxError" % "time of maxError" % "CPU time (clock ticks)";

	for ( int i = 0; i < IntegratorType::NSTEPPERS; i++ ){
		if ( i == IntegratorType::abm )
			simulate_linear_stiff( IntegratorType::abm, tol, 5.0e-4 ); // use special step size
		                                                                   // for abm due to stability
		else
			simulate_linear_stiff( (IntegratorType)i, tol );
	}
}

/// Tests equality on Integrator::Properties
BOOST_AUTO_TEST_CASE(test_equal_integrator_properties)
{
	Integrator::Properties propA;
	Integrator::Properties propB;
	BOOST_CHECK(propA == propB);
	BOOST_CHECK(propB == propA);

	propA.name = "a name";
	BOOST_CHECK(propA != propB);
	BOOST_CHECK(propB != propA);
	propB.name = "a name";
	BOOST_CHECK(propA == propB);
	BOOST_CHECK(propB == propA);

	propA.type = IntegratorType::eu;
	BOOST_CHECK(propA != propB);
	BOOST_CHECK(propB != propA);
	propB.type = IntegratorType::eu;
	BOOST_CHECK(propA == propB);
	BOOST_CHECK(propB == propA);

	propA.order = 2;
	BOOST_CHECK(propA != propB);
	BOOST_CHECK(propB != propA);
	propB.order = 2;
	BOOST_CHECK(propA == propB);
	BOOST_CHECK(propB == propA);

	propA.abstol = 1.0;
	BOOST_CHECK(propA != propB);
	BOOST_CHECK(propB != propA);
	propB.abstol = 1.0;
	BOOST_CHECK(propA == propB);
	BOOST_CHECK(propB == propA);

	propA.reltol = 2.0;
	BOOST_CHECK(propA != propB);
	BOOST_CHECK(propB != propA);
	propB.reltol = 2.0;
	BOOST_CHECK(propA == propB);
	BOOST_CHECK(propB == propA);
}
