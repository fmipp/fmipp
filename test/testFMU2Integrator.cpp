// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#include "import/base/include/FMUModelExchange_v2.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMU2Integrator

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
#include <time.h>
#include <iostream>

#if defined( WIN32 ) // Windows.
#include <algorithm>
#define fmin min
#else // Linux, Unix, etc.
#include <cmath>
#endif

using namespace std;
using namespace boost; // for std::cout << boost::format( ... ) % ... % ... ;
using namespace fmi_2_0;

// simulates the model stiff2 from tstart to tstop and prints the integration-error
// as well as the cpu time to the console. ts is a parameter of the model which
// determines when the event (including a discontinuouty of the RHS) happens.
void runSimulation( IntegratorType integratorType,
	fmippReal ts, fmippReal tolerance,
	fmippReal k        = 10,
	fmippTime tstart   = 0,
	fmippTime tstop    = 1,
	fmippTime stepsize = 0.0025 )
{
	fmippString fmuFolder( "numeric/" );
	fmippString MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuFolder + MODELNAME, MODELNAME,
			       fmippFalse, false, EPS_TIME , integratorType );
	fmu.instantiate( "stiff21" );
	fmu.setValue( "ts", ts );
	fmu.setValue( "k" , k  );
	fmu.initialize();

	fmippString integratorName = fmu.getIntegratorProperties().name;

	fmippReal   x, error, maxError;
	fmippStatus status;
	fmippTime   t, t2, tMaxError;

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
			BOOST_REQUIRE_MESSAGE( error <= tolerance,
					       "Error for Integrator "<< integratorName
					       << " is too big at t = " << t << ". Error = "
					       << error << ". Stopping simulation."
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
	fmippTime ts        = 2.00;       // If ts is bigger than one, there are no events.
	fmippReal tolerance = 0.01;       // if the difference of the numerical and analytical
	                                 // is bigger than this tolerance at one of the
	                                 // communication step points, the test fails.

	cout << "Integrating stiff2 from t = 0 to t = 1 with different integrators."
	     << " The errors are measured at all the communication step points."
	     << endl << endl;

	cout << format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "maxError" % "time of maxError" % "CPU time (clock ticks)";

	for ( int i = 0; i < IntegratorType::NSTEPPERS; i++ )
		runSimulation( (IntegratorType)i, ts, tolerance );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_with_events )
{
	fmippReal s         = 0.6;
	fmippTime ts        = 1.0/2.0 + log( s/(1 - s) )/100;
	                                 // at time ts, the RHS instantainously swiches its sign.
                                         // It can be expected that the biggest errors happen
	                                 // shortly after the event.
	fmippReal tolerance = 0.01;       // if the difference of the numerical and analytical
	                                 // is bigger than this tolerance at one of the
	                                 // communication step points, the test fails.

	cout << endl << "Including events into the simulation ( s = "
	     << s << ", ts = " << ts
	     << " ). Errors are again measured at all communication step points."
	     << endl << endl;

	cout << format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "maxError" % "time of maxError" % "CPU time (clock ticks)";

	for ( int i = 0; i < IntegratorType::NSTEPPERS; i++ )
		runSimulation( (IntegratorType)i, ts, tolerance );
}

void simulate_robertson( IntegratorType integratorType,
	fmippTime tstop = 1.0e2,
	fmippReal abstol = 1.0e-10,
	fmippReal reltol = 1.0e-10 )
{
	fmippString fmuFolder( "numeric/" );
	fmippString MODELNAME( "robertson" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuFolder + MODELNAME, MODELNAME,
		fmippFalse, false, EPS_TIME , integratorType );
	fmippStatus status = fmu.instantiate( "robertson1" );
	BOOST_REQUIRE_EQUAL( status, fmippOK );
	status = fmu.initialize();
	BOOST_REQUIRE_EQUAL( status, fmippOK );

	// without resetting the tolerances, the test will fail for all steppers except
	// bdf which has a smaller tolerance by default
	Integrator::Properties properties = fmu.getIntegratorProperties();
	fmippString integratorName = properties.name;
	properties.abstol = abstol;
	properties.reltol = reltol;
	fmu.setIntegratorProperties( properties );

	double time = clock();
	fmu.integrate( tstop );
	time = clock() - time;

	fmippReal x;
	fmu.getValue( "x", x );
	x -= 6.172349e-1; // actual solution correct to 6 significant digits
	cout << format( "%-20s %-20E %-20s %-20E\n" ) % integratorName
		% fabs( x )  % "" % time;
	BOOST_CHECK_SMALL( x, 1.0e-6  );
}

BOOST_AUTO_TEST_CASE( test_fmu_robertson )
{
	cout << "\nsimulating the test fmu robertson from t = 0 to t = 100\n\n";

	cout << format( "%-20s %-20E %-20E %-20E\n" )
		% "Integrator" % "max Error" % "" % "CPU time";
	simulate_robertson( IntegratorType::ck );
	simulate_robertson( IntegratorType::dp );
	simulate_robertson( IntegratorType::fe );
	simulate_robertson( IntegratorType::bs );
	simulate_robertson( IntegratorType::ro );
#ifdef USE_SUNDIALS
	simulate_robertson( IntegratorType::bdf );
#endif
}
