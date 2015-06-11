#include <import/base/include/FMUModelExchange_v2.h>
#include <import/base/include/ModelManager.h>


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMU2ModelExchange

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
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

// the first few test are almost exact copies of testFMUModelExchange
BOOST_AUTO_TEST_CASE( test_fmu_load_faulty )
{
	string MODELNAME( "XYZ" );
	FMUModelExchange fmu( "ABC", MODELNAME, fmi2False, EPS_TIME );
	fmiStatus status = fmu.instantiate( "xyz" );
	BOOST_REQUIRE( status == fmiError );
}

string fmuPath( "numeric/" );

BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME, fmi2False, EPS_TIME );
}

BOOST_AUTO_TEST_CASE( test_fmu_instantiate )
{
	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME, fmi2False, EPS_TIME );
	fmiStatus status = fmu.instantiate( "stiff21" );
	BOOST_REQUIRE( status == fmiOK );
}

BOOST_AUTO_TEST_CASE( test_fmu_initialize )
{
	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME, fmi2False, EPS_TIME );
	fmu.instantiate( "stiff21" );
	fmiStatus status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );
}

BOOST_AUTO_TEST_CASE( test_setters_and_getters )
{
	cout << endl << "---- GETTERS AND SETTERS ----" << endl << endl;

	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME, fmi2False, EPS_TIME );
	fmu.instantiate( "stiff21" );
	fmu.initialize();

	fmi2Real y;
	fmiStatus status;

	// test getValue
	cout << "values after initialize():" << endl;
	status = fmu.getValue( "x", y );
	BOOST_REQUIRE( status == fmiOK );
	cout << format( "%-8s %-E\n" ) % "x" % y;
	fmu.getValue( "x0", y );
	cout << format( "%-8s %-E\n" ) % "x0" % y;
	fmu.getValue( "ts", y );
	cout << format( "%-8s %-E\n" ) % "ts" % y;
	fmu.getValue( "k", y );
	cout << format( "%-8s %-E\n" ) % "k" % y;

	// test setValue
	cout << format( "\n%-40s %-20s \n" ) % "setting x to 2.01" % "fmu.setValue( \"x\", 2.01 )";
	status = fmu.setValue( "x", 2.01 );
	BOOST_REQUIRE( status == fmiOK );
	cout << format( "%-40s %-20s \n" ) % "calling getter function" % "fmu.getValue( \"x\", y )";
	fmu.getValue( "x", y );
	if ( y == 2.01 ){
		cout << format( "%-40s %-20s \n" ) % "y == 2.01?" %  "fmi2True";
	}else{
		cout << format( "%-40s %-20s \n" ) % "y == 2.01?" %  "fmi2True";
	}
	BOOST_REQUIRE( y == 2.01 );
}

BOOST_AUTO_TEST_CASE( test_fmu_model_description )
{
	cout << endl << "---- BASIC FUNCTIONALITIES OF MODELDESCRIPTION ----" << endl << endl;

	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME,
			       fmi2False, EPS_TIME , IntegratorType::rk );

	cout << format( "%-40s %-10E \n" ) % "number of continuous states" % fmu.nStates();
	cout << format( "%-40s %-10E \n" ) % "number of event indicators"  % fmu.nEventInds();
	cout << format( "%-40s %-10E \n" ) % "number of value references"  % fmu.nValueRefs();
	if ( fmu.providesJacobian() )
		cout << format( "%-40s %-10s \n" ) % "provides Jacobian"   % "true";
	else
		cout << format( "%-40s %-10s \n" ) % "provides Jacobian"   % "false";
}

BOOST_AUTO_TEST_CASE( test_rhs_jacobean_etc )
{
	cout << endl << "---- RHS AND JACOBIAN ----" << endl << endl;

	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME, fmi2False, EPS_TIME );
	fmu.instantiate( "stiff21" );
	fmu.initialize();

	fmiStatus status;
	fmi2Real y;

	// test the RHS evaluation
	status = fmu.getDerivatives( &y );
	BOOST_REQUIRE( status == fmiOK );
	cout << format( "%-40s %-E\n" ) % "derivative from statrting position" % y;

	fmu.setValue( "x", 0.5 );
	status = fmu.getDerivatives( &y );
	BOOST_REQUIRE( status == fmiOK );
	cout << format( "%-40s %-E\n" ) % "derivative for x = 0.5" % y;
	BOOST_REQUIRE( y == 25 );

	// initialize the Jacobian
	fmi2Real** J = new fmi2Real*[1];
	J[0] = new fmi2Real[1];

	// test getter for jacobian
	fmu.getJacobian( J );
	cout << format( "%-40s %-E\n" ) % "jacobian for x = 0.5" % J[0][0];

	// change x and reset Jacobian
	status = fmu.setValue( "x", 0.1 );
	BOOST_REQUIRE( status == fmiOK );
	fmu.getJacobian( J );
	cout << format( "%-40s %-E\n" ) % "jacobian for x = 0.1" % J[0][0];
}

BOOST_AUTO_TEST_CASE( test_model_manager_me )
{
	// almost the same as the ME test in ($build)/test/testModelManager
	string modelName( "stiff2" );
	string fmuUrl = string( FMU_URI_PRE ) + fmuPath + modelName;

	ModelManager& manager = ModelManager::getModelManager();

	BareFMU2* bareFMU1 = manager.getInstance( fmuUrl, modelName, fmiTrue );
	BareFMU2* bareFMU2 = manager.getInstance( fmuUrl, modelName, fmiTrue );

	BOOST_REQUIRE_MESSAGE( bareFMU1 == bareFMU2,
			       "Bare FMUs are not equal." );
}

BOOST_AUTO_TEST_CASE( test_fmu_jacobian_van_der_pol )
{
	std::string MODELNAME( "vanDerPol" );
	std::string fmuPath2( "fmusdk_examples/" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath2 + MODELNAME, MODELNAME,
			      fmi2False, EPS_TIME );

	fmu.instantiate( "van_der_pol1" );
	fmu.initialize();

	// expect providejacobian to be false since the modeldescription does not
	// contain a node called providesDirectionalDerivative
	BOOST_CHECK( fmu.providesJacobian() == false );

	// since the rhs is a polynomial of small order with respect to the state,
	// the numeric jacobian should be exact ( neglecting roundoff errors )
	double* Jac  = new double[ 4 ];
	double* x    = new double[ 2 ];
	double* dfdt = new double[ 2 ];

	// use the starting values of the fmu for testing
	double t = fmu.getTime();
	fmu.getContinuousStates( x );
	fmu.getNumericalJacobian( Jac, x, dfdt, t );

	// test with a tolerance of 1.0e-9 percent ( roundoff errors )
	BOOST_CHECK_SMALL( Jac[0],     1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[1],  1, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[2], -1, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[3], -3, 1.0e-9 );

	// since the rhs is not time dependend, expect dfdt to be zero
	BOOST_CHECK_SMALL( dfdt[0], 1.0e-9 );
	BOOST_CHECK_SMALL( dfdt[1], 1.0e-9 );

	// test again with different state values
	x[0] = 13.23;
	x[1] = 23.14;
	fmu.getNumericalJacobian( Jac, x, dfdt, t );
	BOOST_CHECK_SMALL( Jac[0],                 1.0e-7 );
	BOOST_CHECK_CLOSE( Jac[1],              1, 1.0e-7 );
	BOOST_CHECK_CLOSE( Jac[2], -2*x[0]*x[1]-1, 1.0e-7 );
	BOOST_CHECK_CLOSE( Jac[3],    1-x[0]*x[0], 1.0e-7 );

	delete Jac;
	delete x;
	delete dfdt;
}
