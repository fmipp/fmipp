#include <import/base/include/FMUModelExchange_v2.h>

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
void runSimulation( IntegratorType integratorType, string integratorName,
		    fmi2Real ts,                   fmi2Real tolerance,
		    fmi2Real k        = 10,
		    fmi2Time tstart   = 0,
		    fmi2Time tstop    = 1,
		    fmi2Time stepsize = 0.0025 )
{
	string fmuFolder( "numeric/" );
	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuFolder + MODELNAME, MODELNAME,
			       fmi2False, false, EPS_TIME , integratorType );
	fmu.instantiate( "stiff21" );
	fmu.setValue( "ts", ts );
	fmu.setValue( "k" , k  );
	fmu.initialize();

	fmi2Real   x, error, maxError;
	fmiStatus status;
	fmi2Time   t, t2, tMaxError;

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
	fmi2Time ts        = 2.00;       // If ts is bigger than one, there are no events.
	fmi2Real tolerance = 0.01;       // if the difference of the numerical and analytical
	                                 // is bigger than this tolerance at one of the
	                                 // communication step points, the test fails.

	cout << "Integrating stiff2 from t = 0 to t = 1 with different integrators."
	     << " The errors are measured at all the communication step points."
	     << endl << endl;

	cout << format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "maxError" % "time of maxError" % "CPU time (clock ticks)";

	runSimulation(IntegratorType::eu  , "eu"  , ts, tolerance);
	runSimulation(IntegratorType::rk  , "rk"  , ts, tolerance);
	runSimulation(IntegratorType::ck  , "ck"  , ts, tolerance);
	runSimulation(IntegratorType::dp  , "dp"  , ts, tolerance);
	runSimulation(IntegratorType::fe  , "fe"  , ts, tolerance);
	runSimulation(IntegratorType::bs  , "bs"  , ts, tolerance);
	runSimulation(IntegratorType::abm , "abm" , ts, tolerance);
	runSimulation(IntegratorType::ro  , "ro"  , ts, tolerance);
#ifdef USE_SUNDIALS
	runSimulation(IntegratorType::bdf , "bdf" , ts, tolerance);
	runSimulation(IntegratorType::abm2, "abm2", ts, tolerance);
#endif
}


BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_with_events )
{
	fmi2Real s         = 0.6;
	fmi2Time ts        = 1.0/2.0 + log( s/(1 - s) )/100;
	                                 // at time ts, the RHS instantainously swiches its sign.
                                         // It can be expected that the biggest errors happen
	                                 // shortly after the event.
	fmi2Real tolerance = 0.01;       // if the difference of the numerical and analytical
	                                 // is bigger than this tolerance at one of the
	                                 // communication step points, the test fails.

	cout << endl << "Including events into the simulation ( s = "
	     << s << ", ts = " << ts
	     << " ). Errors are again measured at all communication step points."
	     << endl << endl;

	cout << format("%-20s %-20s %-20s %-20s\n")
		% "Integrator" % "maxError" % "time of maxError" % "CPU time (clock ticks)";

	runSimulation(IntegratorType::eu  , "eu"  , ts, tolerance);
	runSimulation(IntegratorType::rk  , "rk"  , ts, tolerance);
	runSimulation(IntegratorType::ck  , "ck"  , ts, tolerance);
	runSimulation(IntegratorType::dp  , "dp"  , ts, tolerance);
	runSimulation(IntegratorType::fe  , "fe"  , ts, tolerance);
	runSimulation(IntegratorType::bs  , "bs"  , ts, tolerance);
	runSimulation(IntegratorType::abm , "abm" , ts, tolerance);
	runSimulation(IntegratorType::ro  , "ro"  , ts, tolerance);
#ifdef USE_SUNDIALS
	runSimulation(IntegratorType::bdf , "bdf" , ts, tolerance);
	runSimulation(IntegratorType::abm2, "abm2", ts, tolerance);
#endif
}

