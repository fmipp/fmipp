#include <import/base/include/FMUModelExchange.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMUIntegrator

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <algorithm>

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_1 )
{
	// tests the accuracy of the integrator
	// accuracy measurement is the maximal difference to the real solution on all
	// available times (0, stepsize, 2*stepsize, ..., 1-stepsize, 1)
	
	fmiReal t = 0.0;
	fmiReal stepsize = 0.0025;
	fmiReal tstop = 1.0;
	fmiReal x;
	fmiReal k;
	fmiReal i;
	fmiReal j;
	fmiReal t2;
	fmiReal s = 1.9;
	fmiReal ts;
	fmiReal error;
	fmiReal maxError = 0;
	fmiReal tMaxError = t;  // time of maximum Error

	// each iteration in i and j integrates the model from t=0 to t=1
	// i and j determine the parameters s and k of the model
	for ( j = 0; j < 2; j++ ){
		s=1.6-j;
		for ( i = 15; i < 17; i++ ) {
			k = 10*i;

			std::string MODELNAME( "stiff" );
			FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, EPS_TIME );
			fmiStatus status = fmu.instantiate( "stiff1", fmiFalse );
			BOOST_REQUIRE( status == fmiOK );				

			if ( ( 0 < s ) && ( s < 1 ) ){
				ts = 1.0/2.0 + log( s/(1 - s) )/k;
			}else{
				ts = std::numeric_limits<double>::infinity();
			}

			fmu.setValue( "k", k );
			fmu.setValue( "ts", ts );
			status = fmu.initialize();
			BOOST_REQUIRE( status == fmiOK );

			t = 0.0;
			maxError = 0;

			while ( ( t + stepsize ) - tstop < EPS_TIME ) {
				fmu.setTime(t);
				t = fmu.integrate( t + stepsize  );
				status = fmu.getValue( "x" , x );
				BOOST_REQUIRE( status == fmiOK );
				
				// compare with the exact solution
				t2 = std::min(t,2*ts-t);
				error=fabs( exp( k*t2 )/( exp( k*0.5 ) + exp( k*t2 ) ) - x );
		    
				// check if error is NaN
				BOOST_REQUIRE( error == error );  

				// update maxError and tMaxError
				if( error > maxError ){
					maxError = error;
					tMaxError = t;
				}
			}
			
			// print test results
			std::cout << "s = "<< s << ", k = "<<  k << ", ts = " << ts << std::endl << 
				"    maximum error " << maxError << " at t = " << tMaxError << std::endl << std::endl;
			t = fmu.getTime();

			BOOST_REQUIRE( std::abs( t - tstop ) < stepsize );
			BOOST_REQUIRE_MESSAGE( maxError < 1e-6 , "maximum error " << maxError 
					       <<  " is bigger than the tolerance 1e-6" 
					       );
              
		}
	} 
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation_2 )
{
	// Estimates the order of convergence i.e. the p in the following formula 
	// err <= C*h^p
	// the stiff model is run with different step sizes

	fmiReal t = 0.0;
	fmiReal stepsize = 0.1;
	fmiReal tstop = 1.0;
	fmiReal x;
	fmiReal k = 100.0;
	fmiReal i;
	fmiReal dt = stepsize;
	fmiReal t2;
	fmiReal s = 1.9;
	fmiReal ts;
	fmiReal error;
	fmiReal maxError = 0;
	fmiReal maxErrorOld;

	std::cout << "### Estimators for the order of convergence" << std::endl;
        
    
	for ( i = 0; i < 13; i++ ) {
 
		dt = stepsize = stepsize/2;
		
		std::string MODELNAME( "stiff" );
		FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, dt );
		fmiStatus status = fmu.instantiate( "stiff1", fmiFalse );
		BOOST_REQUIRE( status == fmiOK );

		if ( (0<s) && (s<1) ){
			ts = 1.0/2.0 + log( s/( 1-s ) )/k;
		}else{
			ts = std::numeric_limits<double>::infinity();
		}
		
		fmu.setValue( "k", k );
		fmu.setValue( "ts", ts );
		status = fmu.initialize();
		BOOST_REQUIRE( status == fmiOK );
		t = 0.0;
		maxError = 0;

		while ( ( t + stepsize ) - tstop < dt ) {
			fmu.setTime( t );
			t = fmu.integrate( t + stepsize , dt );
			status = fmu.getValue( "x" , x );
			BOOST_REQUIRE( status == fmiOK );

			// compare with exact solution to get error
			t2 = t;
			error=fabs( exp( k*t2 )/( exp( k*0.5 ) + exp( k*t2 ) ) - x );
		    
			// check if error is NaN
			BOOST_REQUIRE( error == error );  

			// update maxError and tMaxError
			if( error > maxError ){
				maxError = error;
			}
		}
		
		// print the estimator for the order of convergence i.e. the p in err <= C*h^p
		if ( i != 0 ){
			std::cout << log(maxErrorOld/maxError)/std::log(2.) << "\t" <<  maxError << std::endl;
		}
		
		t = fmu.getTime();
		BOOST_REQUIRE( std::abs( t - tstop ) < stepsize );

		// only test if step size is small enough
		if ( i > 7 ){
			BOOST_REQUIRE( maxError < 1e-2 * stepsize );
		}
		maxErrorOld = maxError;	
	}	
}
