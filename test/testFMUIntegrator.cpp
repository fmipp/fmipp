// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#include <import/base/include/FMUModelExchange.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMUIntegrator

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <time.h>

#if defined( WIN32 ) // Windows.
#include <algorithm>
#define fmin min
#else // Linux, Unix, etc.
#include <cmath>
#endif

using namespace std;
using namespace boost; // for std::cout << boost::format( ... ) % ... % ... ;

string fmuPath( "numeric/" );

int estimateOrder( IntegratorType integratorType, string integratorName, int nSteps = 1 )
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
	fmiReal error = 0;
	fmiReal x;
	fmiReal tolerance = 1e-15;
	fmiReal p = -1;
	string MODELNAME( "polynomial" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME,
			      fmiFalse, EPS_TIME, integratorType );

	fmu.instantiate( "polynomial1", fmiFalse );
	fmu.initialize();

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
	estimateOrder( IntegratorType::eu, "eu" );
	estimateOrder( IntegratorType::rk, "rk" );
	estimateOrder( IntegratorType::ck, "ck" );
	estimateOrder( IntegratorType::dp, "dp" );
	estimateOrder( IntegratorType::fe, "fe" );
	estimateOrder( IntegratorType::bs, "bs" );
	//estimateOrder( IntegratorType::abm, "abm" );	// skip this integrator since the version of odeint which comes with boost 1.57
							// will fail the test otherwise. The test is passed with the github version of
							// odeint tough. ( https://github.com/headmyshoulder/odeint-v2 ).
#ifdef USE_SUNDIALS
	estimateOrder( IntegratorType::bdf, "bdf" );
	estimateOrder( IntegratorType::abm2, "abm2" );
#endif
}



// simulates the model stiff from tstart to tstop and prints the integration-error
// as well as the cpu time to the console. ts is a parameter of the model which
// determines when the event (including a discontinuouty of the RHS) happens.
void runSimulation( IntegratorType integratorType, string integratorName,
		    fmiReal ts,                   fmiReal tolerance,
		    fmiReal k        = 10,
		    fmiTime tstart   = 0,
		    fmiTime tstop    = 1,
		    fmiTime stepsize = 0.0025,
		    fmiReal eventSearchPrecision = 1e-15 )
{
	string MODELNAME( "stiff" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME,
			       fmiFalse, eventSearchPrecision , integratorType );
	fmu.instantiate( "stiff1", fmiFalse );
	fmu.setValue( "ts", ts );
	fmu.setValue( "k" , k  );
	fmu.initialize();

	fmiReal   x, error, maxError;
	fmiStatus status;
	fmiTime   t, t2, tMaxError;

	t        = tstart;
	maxError = 0;

	// measure CPU time by calling clock directly before and after the simulation
	double time = clock(); // \FIXME probably won't work on windows

	while ( t < tstop ){

		// perform Integrator step
		t2 = t;
		t  = fmu.integrate( fmin( t + stepsize, tstop ) );

		// get values (time and state) after integrator step
		status = fmu.getValue( "x", x );
		BOOST_REQUIRE_MESSAGE( status == fmiOK ,
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
			BOOST_REQUIRE_MESSAGE( error <= tolerance, 
					       "Maximum error for Integrator "<< integratorName 
					       << " is too big at t = " << t << ". Stopping simulation."
					       );
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
	fmiTime ts        = 2.00;       // If ts is bigger than one, there are no events.
	fmiReal tolerance = 1e-4;       // if the difference of the numerical and analytical
	                                // is bigger than this tolerance at one of the
	                                // communication step points, the test fails.

	cout << "Integrating stiff from t = 0 to t = 1 with different integrators."
	     << " The errors are measured at all the communication step points."
	     << endl << endl;

	cout << format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "maxError" % "time of maxError" % "CPU time (clock ticks)";

	runSimulation(IntegratorType::eu,   "eu",   ts , tolerance);
	runSimulation(IntegratorType::rk,   "rk",   ts , tolerance);
	runSimulation(IntegratorType::ck,   "ck",   ts , tolerance);
	runSimulation(IntegratorType::dp,   "dp",   ts , tolerance);
	runSimulation(IntegratorType::fe,   "fe",   ts , tolerance);
	runSimulation(IntegratorType::bs,   "bs",   ts , tolerance);
	runSimulation(IntegratorType::abm,  "abm",  ts , tolerance);
#ifdef USE_SUNDIALS
	runSimulation(IntegratorType::bdf,  "bdf",  ts , tolerance);
	runSimulation(IntegratorType::abm2, "abm2", ts , tolerance);
#endif
}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_with_events )
{
	fmiReal s         = 0.6;
	fmiTime ts        = 1.0/2.0 + log( s/(1 - s) )/10;
	                                // at time ts, the RHS instantainously swiches its sign.
                                        // It can be expected that the biggest errors happen
	                                // shortly after the event.
	fmiReal tolerance = 1e-4;       // if the difference of the numerical and analytical
	                                // is bigger than this tolerance at one of the
	                                // communication step points, the test fails.

	cout << endl << "Including events into the simulation ( s = "
	     << s << ", ts = " << ts
	     << " ). Errors are again measured at all communication step points."
	     << endl << endl;

	cout << format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "maxError" % "time of maxError" % "CPU time (clock ticks)";

	runSimulation(IntegratorType::eu,   "eu",   ts , tolerance);
	runSimulation(IntegratorType::rk,   "rk",   ts , tolerance);
	runSimulation(IntegratorType::ck,   "ck",   ts , tolerance);
	runSimulation(IntegratorType::dp,   "dp",   ts , tolerance);
	runSimulation(IntegratorType::fe,   "fe",   ts , tolerance);
	runSimulation(IntegratorType::bs,   "bs",   ts , tolerance);
	runSimulation(IntegratorType::abm,  "abm",  ts , tolerance);
#ifdef USE_SUNDIALS
	runSimulation(IntegratorType::bdf,  "bdf",  ts , tolerance);
	runSimulation(IntegratorType::abm2, "abm2", ts , tolerance);
#endif
}
