// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#include "import/base/include/FMUModelExchange_v2.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMU2SDKImport

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
#include <string>
#include <time.h>
#include <iostream>
#include <fstream>

#if defined( WIN32 ) // Windows.
#include <algorithm>
#define fmin min
double round( double number )
{
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}
#else // Linux, Unix, etc.
#include <cmath>
#endif

using namespace std;
using namespace boost; // for std::cout << boost::format( ... ) % ... % ... ;
using namespace fmi_2_0;


// setings for the simulation
fmippTime t0                 = 0.0;         // beginning of the simulation
fmippTime tEnd               = 2.36;        // end of the simulation
bool stopBeforeEvent        = true;        // should the integrator stop before or after the
                                           // bounes if they are detected?
bool loggingOn              = false;       // print information about the simulation to the terminal?
                                           // warning: the output produced here is very low-level
fmippTime stepSize           = 9.8e-3;
IntegratorType integrator   = IntegratorType::dp;

// settings for the model
fmippReal startingValues [2] = { 5, 3 };    // { h0, v0 }
fmippReal dampingFactor      = 0.5;         // the parameter "e" in the bouncingBall model


fmippReal e, g;                             // modelparameters
fmippReal h, v;                             // height and velocity
fmippTime t;                                // time

fmippStatus status;

int nModels = 5;
string modelNames [5];
string fmuFolder( "fmusdk_examples/" );

std::vector<FMUModelExchange*> Models;


BOOST_AUTO_TEST_CASE( call_constructor_sdk )
{
	modelNames[0] = "bouncingBall";
	modelNames[1] = "dq";
	modelNames[2] = "vanDerPol";
	modelNames[3] = "values";
	modelNames[4] = "inc";

	for ( int i = 0; i < nModels; i++ ){
		Models.push_back( new FMUModelExchange( FMU_URI_PRE + fmuFolder +
							modelNames[i], modelNames[i], loggingOn,
							stopBeforeEvent, EPS_TIME , integrator
							)
				  );
	}
}

BOOST_AUTO_TEST_CASE( instantiate_and_initialize_sdk )
{
	for ( int i = 0; i < nModels; i++ ){
		status = Models[ i ]->instantiate( modelNames[ i ] + "1" );
		BOOST_REQUIRE_EQUAL( status, fmiOK );
		status = Models[ i ]->initialize();
		BOOST_REQUIRE_EQUAL( status, fmiOK );
	}

}

BOOST_AUTO_TEST_CASE( test_sdk_model_description )
{
	cout << endl << "---- BASIC FUNCTIONALITIES OF MODELDESCRIPTION ----" << endl << endl;
	cout << format( "%-20s%-12s%-12s%-12s%-12s \n" )
		% "Model Name" % "nStates" % "nEventInds" % "nValRef" % "provides Jacobian";
	for ( int i = 0; i < nModels; i++ ){
		cout << format("%-20s") % modelNames[ i ];
		cout << format("%-12s") % Models[ i ]->nStates();
		cout << format("%-12s") % Models[ i ]->nEventInds();
		cout << format("%-12s") % Models[ i ]->nValueRefs();
		if ( Models[ i ]->providesJacobian() )
			cout << format("%-12s") % "true";
		else
			cout << format("%-12s") % "false";
		cout << endl;

	}
}


BOOST_AUTO_TEST_CASE( test_bouncingball_get_set )
{
	FMUModelExchange* bouncingBall = Models[ 0 ];

	fmippReal x[ 2 ];

	cout << endl << "---- GETTERS AND SETTERS FOR BOUNCINGBALL ----" << endl << endl;
	cout << "values after initialize():" << endl;

	t = bouncingBall->getTime();
	cout << format( "%-8s %-E\n" ) % "t" % t;

	status = bouncingBall->getValue( "h", h );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	cout << format( "%-8s %-E\n" ) % "h" % h;

	status = bouncingBall->getValue( "v", v );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	cout << format( "%-8s %-E\n" ) % "v" % v;

	status = bouncingBall->getValue( "g", g );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	cout << format( "%-8s %-E\n" ) % "g" % g;

	status = bouncingBall->getValue( "e", e );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	cout << format( "%-8s %-E\n" ) % "e" % e;

	cout << "setting continuousStates to { "
	     << startingValues[ 0 ] << " ," <<  startingValues[ 1 ]
	     << " }" ;

	status = bouncingBall->setContinuousStates( startingValues );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	bouncingBall->getContinuousStates( x );
	if ( x[ 0 ] == startingValues[ 0 ] && x[ 1 ] == startingValues[ 1 ] )
		cout << " worked" << endl;
	else{
		cout << " did not work" << endl;
		BOOST_REQUIRE_EQUAL( 1, 2 );   // abort the test if the setter function does not work
	}

	status = bouncingBall->setValue( "e", dampingFactor );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	cout << "setting e to " << dampingFactor ;
	status = bouncingBall->getValue( "e", e );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	if ( e == dampingFactor ) cout << " worked" << endl;
	else {
		cout << " did not work" << endl;
		BOOST_REQUIRE_EQUAL( 1, 2 );
		                           // abort the test if the setter function does not work
	}
	bouncingBall->setTime( t0 );

	// load the fmu time/states into local variables and check a last time
	status = bouncingBall->getValue( "h", h );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	BOOST_REQUIRE_EQUAL( h, startingValues[ 0 ] );

	status = bouncingBall->getValue( "v", v );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	BOOST_REQUIRE_EQUAL( v, startingValues[ 1 ] );

	t = bouncingBall->getTime();
	BOOST_REQUIRE_EQUAL( t, t0 );
}

BOOST_AUTO_TEST_CASE( test_bouncingball_run_simulation )
{

	FMUModelExchange* bouncingBall = Models[ 0 ];

	fmippReal v2;                               // velocity at the end of the current bounce
	fmippTime t2, tOld;                         // original time and artificial times for the calculaton
	                                           // of the exact solution

	fmippReal nextEventTime, exactHeight;       // exact values calculated frm the analytical solution

	fmippReal error, maxError = 0, tMaxError = 0;

	cout << endl << "--- SIMULATION OF BOUNCINGBALL ---" << endl << endl;

	//cout << format( "%-20s %-20s %-20s %-20s\n" ) % "time" % "height" % "velocity" % "error (height)";

	exactHeight = startingValues[0] + startingValues[1]*t - g/2.0*t*t;
	//cout << format( "%-20E %-20E %-20E %-20E\n" ) % t % h  % v % ( h - exactHeight );
	BOOST_REQUIRE_SMALL( h - exactHeight, 1e-16 );

	nextEventTime = ( v + sqrt( v*v + 2.0*h*g ) )/g;     // time of first bounce
	t2 = ( sqrt( v*v + 2.0*h*g ) - v )/g;                // time since the last event (for h0 > 0
                                                             // the last event "happened" for a t < 0 )
	v2 = sqrt( 2.0*g*h + v*v );           	             // conservation of energy mv2^2/2 = mgh + mv^2/2

	maxError = 0;
	tMaxError = 0;

	while ( t < tEnd ){
		tOld = t;
		t = bouncingBall->integrate( fmin( t + stepSize, tEnd ) );
		t2+= t - tOld;
		bouncingBall->getValue( "h", h );
		bouncingBall->getValue( "v", v );

		// calculate the analytical solution and the error
		while ( t > nextEventTime ){
			// t2 is the time relative to the new event
			t2 = t - nextEventTime;
			v2 = v2 * e;
			nextEventTime = nextEventTime + 2*v2/g;
		}
		exactHeight = v2*t2 - g/2.0*t2*t2;
		error = fabs( exactHeight - h );
		if ( error > maxError ){
			maxError = error;
			tMaxError = t;
		}
		// print the results
		//cout << format( "%-20E %-20E %-20E %-20E\n" ) % t % h  % exactHeight % fabs( h - exactHeight );
	}
	//cout << endl;
	cout << format("%-20s %-20E\n") % "maxError"  %  maxError;
	cout << format("%-20s %-20E\n") % "tMaxError" % tMaxError;
	BOOST_REQUIRE_SMALL( maxError, 1e-2 );
}

BOOST_AUTO_TEST_CASE( test_dq_run_simulation )
{
	cout << endl << "--- SIMULATION OF DAHLQUIST TEST EQUATION ---" << endl << endl;

	fmippReal error, maxError = 0, tMaxError = 0;
	fmippReal exactState;
	fmippReal x;
	fmippReal x0 = 1;

	FMUModelExchange* dq = Models[ 1 ];
	dq->setTime( 0.0 );
	status = dq->setContinuousStates( &x0 );
	BOOST_REQUIRE_EQUAL( status, fmiOK );

	t = 0.0;

	status = dq->setValue( "k", 1.0 );
	BOOST_REQUIRE_EQUAL( status, fmiOK );

	while ( t < tEnd ){
		t = dq->integrate( fmin( t + stepSize, tEnd ) );
		exactState = exp( -t );
		dq->getValue( "x", x  );
		error = fabs( x - exactState );
		if ( error > maxError ){
			maxError = error;
			tMaxError = t;
		}
	}
	cout << format("%-20s %-20E\n") % "maxError"  %  maxError;
	cout << format("%-20s %-20E\n") % "tMaxError" % tMaxError;
	BOOST_REQUIRE_SMALL( maxError, 1e-2 );
}

BOOST_AUTO_TEST_CASE( test_inc_run_simulation )
{
	cout << endl << "--- SIMULATION OF THE INCREMENTAL EXAMPLE ---" << endl << endl;

	fmippBoolean printSimulation = fmippFalse;
	fmippInteger i, error, maxError = 0;
	fmippTime tMaxError = 0;

	t        =  0.00;   // start of the simulation
	tEnd     = 12.00;   // end of the simulation

	FMUModelExchange* inc = Models[ 4 ];

	if ( printSimulation ){
		cout << format( "%-20s %-20s %-20s\n" ) % "time" % "counter" % "error";
		inc->getValue( "counter", i );
		error =  abs( i - 1 );
		cout << format( "%-20E %-20i %-20i\n" ) % t % i % error;
	}
	while ( t < tEnd ){
		// perform integration and get the new "state"
		t = inc->integrate( fmin( t + stepSize, tEnd ) );
		status = inc->getValue( "counter", i );
		BOOST_REQUIRE_EQUAL( status, fmiOK );

		// calculate the error by comparing with the exact solution
		// if t is close to an integer value, the exact solution depends on the
		// flag stopBeforeEvent.
		if ( ( abs( t - round( t ) ) < 1e-16 ) ){
			if (stopBeforeEvent){
				error =  abs( round( t ) - i );
			}else{
				error =  abs( round( t ) + 1 - i );
	                }
		} else{
			error = abs( ceil( t ) - i );
		}

		if ( printSimulation )
			cout << format( "%-20E %-20i %-20i\n" ) % t % i % error;
		if ( error > maxError ){
			maxError  = error;
			tMaxError = t;
		}
	}
	cout << format("%-20s %-20i\n") % "maxError"  %  maxError;
	cout << format("%-20s %-20E\n") % "tMaxError" % tMaxError;
	BOOST_REQUIRE_EQUAL( maxError, 0 );
}

BOOST_AUTO_TEST_CASE( test_van_der_pol_simulation )
{
	fmippReal x[ 2 ];
	fmippBoolean printSimulation = fmippFalse;
	fmippReal error, maxError = 0, tMaxError = 0;

	cout << endl << "--- SIMULATION OF VAN DER POL ---" << endl << endl;
	FMUModelExchange* vanDerPol = Models[ 2 ];

	t        =  0.00;   // start of the simulation
	tEnd     =  7.00;   // end of the simulation

	// values of the exact solution imported from matlab (ode45)
#ifdef _MSC_VER
	vector<fmippReal> tVec;
	tVec.push_back( 1 );
	tVec.push_back( 2 );
	tVec.push_back( 3 );
	tVec.push_back( 4 );
	tVec.push_back( 5 );
	tVec.push_back( 6 );
	tVec.push_back( 7 );
	vector<fmippReal> x0;
	x0.push_back( 1.508144236975608 );
	x0.push_back( 0.323316667046163 );
	x0.push_back( -1.866073911061080 );
	x0.push_back( -1.741768324360919 );
	x0.push_back( -0.837077450294759 );
	x0.push_back( 1.279042029109059 );
	x0.push_back( 1.920152417369846 );
	vector<fmippReal> x1;
	x1.push_back( -0.780218074629693 );
	x1.push_back( -1.832974567985825 );
	x1.push_back( -1.021060340195857 );
	x1.push_back( 0.624666163677373 );
	x1.push_back( 1.307088937799676 );
	x1.push_back( 2.437814449637331 );
	x1.push_back( -0.435838533125378 );
#else
	vector<fmippReal> tVec {1 , 2 , 3 , 4, 5, 6, 7};
	vector<fmippReal> x0 { 1.508144236975608,
			0.323316667046163,
			-1.866073911061080,
			-1.741768324360919,
			-0.837077450294759,
			1.279042029109059,
			1.920152417369846,
			};
	vector<fmippReal> x1 { -0.780218074629693,
			-1.832974567985825,
			-1.021060340195857,
			0.624666163677373,
			1.307088937799676,
			2.437814449637331,
			-0.435838533125378,
			};
#endif

	unsigned int N = tVec.size();
	BOOST_REQUIRE_EQUAL( x0.size(), N );
	BOOST_REQUIRE_EQUAL( x1.size(), N );

	if ( printSimulation ){
		cout << format( "%-20s %-20s %-20s %-20s\n" ) % "t" % "x0" % "x1" % "Error";
	}

	for ( unsigned int i = 0; i < N; i++ ){
		// integrate the model from tVec[i]
		tEnd = tVec[i];
		while ( t < tEnd ){
			// perform integration and get the new "state"
			t      = vanDerPol->integrate( fmin( t + stepSize, tEnd ) );
			status = vanDerPol->getContinuousStates( x );
			BOOST_REQUIRE_EQUAL( status, fmiOK );
		}

		// calculate the errors from the hardcoded values
		error = max( abs(x[0] - x0[i]), abs( x[1] -x1[i]) );
		if ( printSimulation )
			cout << format( "%-20E %-20E %-20E %-20E\n" ) % t % x[0] % x[1] % error;
		if ( error > maxError ){
			maxError = error;
			tMaxError = t;
		}
        }
	if ( printSimulation )
		cout << endl;
	cout << format("%-20s %-20i\n") % "maxError"  %  maxError;
	cout << format("%-20s %-20E\n") % "tMaxError" % tMaxError;
	BOOST_REQUIRE_SMALL( maxError, 0.01 );
}

BOOST_AUTO_TEST_CASE( test_values_fmu_run_simulation )
{
	cout << "\n--- SIMULATION OF THE VALUES FMU ---\n\n";

	// reload the values model with stopBeforeEvent == false
	FMUModelExchange valuesModel( FMU_URI_PRE + fmuFolder +
				       "values", "values", loggingOn,
				       fmippFalse, EPS_TIME , integrator
				       );

	// check again whether initialize/instantiate results in errors
	fmippStatus status = valuesModel.instantiate( "valuesModel1" );
	BOOST_REQUIRE_EQUAL( status, fmiOK );
	status = valuesModel.initialize();
	BOOST_REQUIRE_EQUAL( status, fmiOK );

	// define expected outputs for the modelVariable string_out
	string month[] = {
		"jan","feb","march","april","may","june","july",
		"august","sept","october","november","december", "december"
	};

	fmippTime t = valuesModel.getTime();

	for ( unsigned int i = 0; i < 13; i++ ){
		// get the model variables at the current time
		fmippReal     x          = valuesModel.getRealValue   ( "x"          );
		fmippInteger  int_out    = valuesModel.getIntegerValue( "int_out"    );
		fmippString   string_out = valuesModel.getStringValue ( "string_out" );
		fmippBoolean  bool_out   = valuesModel.getBooleanValue( "bool_out"   );

		{
			// test alternative getter functions for booleans

			fmippBoolean bool_out2 = !bool_out;
			valuesModel.getValue( "bool_out", bool_out2 );
			BOOST_CHECK_EQUAL( bool_out, bool_out2 );

			// get the value reference of bool_out to test even more getter functions
			fmiValueReference bool_ref = valuesModel.getValueRef( "bool_out" );
			BOOST_CHECK_EQUAL( bool_ref, 1 );

			// get the boolean value by value reference
			bool_out2 = !bool_out;
			valuesModel.getValue( bool_ref, bool_out2 );
			BOOST_CHECK_EQUAL( bool_out, bool_out2 );

			// get the boolean using the vector-vise getter function
			bool_out2 = !bool_out;
			valuesModel.getValue( &bool_ref, &bool_out2, 1 );
			BOOST_CHECK_EQUAL( bool_out, bool_out2 );
		}

		// print the outputs to screen
		cout << format("%-20E%-20s%-20E%-20i%-20i\n") % t % string_out
			% x % (int)bool_out % int_out;

		// check wether the outputs are as expected
		if ( i != 0 )
			BOOST_CHECK_CLOSE( t, i, 1.0e-7 );
		BOOST_CHECK_EQUAL( i, int_out );
		BOOST_CHECK_EQUAL( string_out, month[i] );
		BOOST_CHECK_EQUAL( (int)bool_out, i % 2 );

		/** \todo check why the values of x are so much different from 0
		          for t >= 1. Include x and der( x ) into the test        */

		// integrate the fmu to the next time.
		t = valuesModel.integrate( 42.0 );
	}
}

BOOST_AUTO_TEST_CASE( test_values_fmu_setter_functions )
{
	///  \todo: test more setter functions.

	FMUModelExchange* valuesModel = Models[ 3 ];
	fmippBoolean bool_in, bool_out;
	bool_in = fmiTrue;
	bool_out = !bool_in;
	valuesModel->setValue( "bool_in", bool_in  );
	valuesModel->getValue( "bool_in", bool_out );
	BOOST_CHECK_EQUAL( bool_in, bool_out );
}
