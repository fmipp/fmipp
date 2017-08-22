// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#include "import/base/include/FMUModelExchange_v2.h"
#include "import/base/include/ModelDescription.h"
#include "import/base/include/ModelManager.h"
#include "import/base/include/CallbackFunctions.h"
#include "import/base/include/LogBuffer.h"


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

namespace { // This anonymous namespace contains all the things needed for the custom logger used in the tests.

	// This class is used for defining a custom logger (see below).
	class FMULoggingHelper
	{

	public:

		FMULoggingHelper( string name ) : strName_( name ), iCall_( 0 ) {}

		fmi2String logMessage( fmi2String msg ) {
			++iCall_; // Increment counter.
			stringstream s;
			s << strName_ << ", call #" << iCall_ << " -> " << msg; // Concatenate log message.
			strcpy( strMsg_, s.str().c_str() ); // Copy log message to persistent variable.
			return strMsg_;
		}

	private:

		string strName_; // Name given to the instance.
		unsigned int iCall_; // Number of times the logMessage function has been called.
		fmi2Char strMsg_[256]; // Persistent variable for storing the output message.
	};


	// This is a custom logger, which expects an instance of class FMULoggingHelper as
	// input (via opaque pointer componentEnvironment).
	void customLogger( fmi2ComponentEnvironment componentEnviroment, fmi2String instanceName,
		fmi2Status status, fmi2String category, fmi2String message, ... )
	{
		FMULoggingHelper* lh = reinterpret_cast<FMULoggingHelper*>( componentEnviroment );

		if ( 0 == lh ) {
			callback2::verboseLogger( 0, instanceName, status, category, message );
		} else {
			callback2::verboseLogger( 0, instanceName, status, category, lh->logMessage( message ) );
		}
	}
}

string fmuPath( "numeric/" );

// the first few test are almost exact copies of testFMUModelExchange
BOOST_AUTO_TEST_CASE( test_fmu_load_faulty )
{
	string MODELNAME( "XYZ" );
	FMUModelExchange fmu( "ABC", MODELNAME, fmi2False, EPS_TIME );
	fmiStatus status = fmu.instantiate( "xyz" );
	BOOST_REQUIRE( status == fmiError );
}

BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME, fmi2False, EPS_TIME );
	BOOST_CHECK(fmu.getLastStatus() == fmiOK);
}

BOOST_AUTO_TEST_CASE( test_fmu_instantiate )
{
	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME, fmi2False, EPS_TIME );
	BOOST_CHECK(fmu.getLastStatus() == fmiOK);
	fmiStatus status = fmu.instantiate( "stiff21" );
	BOOST_REQUIRE( status == fmiOK );
	BOOST_CHECK(fmu.getLastStatus() == fmiOK);
}

BOOST_AUTO_TEST_CASE( test_fmu_initialize )
{
	string MODELNAME( "stiff2" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuPath + MODELNAME, MODELNAME, fmi2False, EPS_TIME );
	BOOST_CHECK(fmu.getLastStatus() == fmiOK);
	fmu.instantiate( "stiff21" );
	fmiStatus status = fmu.initialize();
	BOOST_REQUIRE( status == fmiOK );
}

BOOST_AUTO_TEST_CASE( test_fmu_getModelDescription_success )
{
	std::string MODELNAME( "zigzag2" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	BOOST_REQUIRE_EQUAL(fmu.getLastStatus(), fmiOK);
	const ModelDescription* ptr = fmu.getModelDescription();
	BOOST_REQUIRE(ptr != NULL);
	BOOST_CHECK_EQUAL(
		ptr->getModelAttributes().get<std::string>("modelName"), 
		"zigzag2");
}

BOOST_AUTO_TEST_CASE( test_fmu_getModelDescription_failure )
{
	std::string MODELNAME( "no-fmu-today-my-love-is-gone-away" );
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	BOOST_REQUIRE_NE(fmu.getLastStatus(), fmiOK);
	const ModelDescription* ptr = fmu.getModelDescription();
	BOOST_CHECK(ptr == NULL);
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
	fmi2Real* J = new fmi2Real[1];

	// test getter for jacobian
	fmu.getJac( J );
	cout << format( "%-40s %-E\n" ) % "jacobian for x = 0.5" % J[0];

	// change x and reset Jacobian
	status = fmu.setValue( "x", 0.1 );
	BOOST_REQUIRE( status == fmiOK );
	fmu.getJac( J );
	cout << format( "%-40s %-E\n" ) % "jacobian for x = 0.1" % J[0];

	delete[] J;
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

BOOST_AUTO_TEST_CASE( test_fmu_jacobian_robertson )
{
	// load the FMU
	string fmuFolder( "numeric/" );
	string MODELNAME( "robertson" );
	FMUModelExchange fmu( FMU_URI_PRE + fmuFolder + MODELNAME, MODELNAME );
	fmiStatus status = fmu.instantiate( "robertson1" );

	// check whether the load was sucessfull
	BOOST_REQUIRE_EQUAL( status, fmiOK );

	fmu.initialize();

	// allocate memory for Jacobians, states
	double* x = new double[ 3 ];
	double* Jac = new double[9];

	// set the FMU to the state ( 2, 3, 4 )
	x[0] = 2.0; x[1] = 3.0; x[2] = 4.0;
	status = fmu.setContinuousStates( x );
	BOOST_REQUIRE_EQUAL( status, fmiOK );

	// retrieve the jacobian
	status = fmu.getJac( Jac );
	BOOST_REQUIRE_EQUAL( status, fmiOK );

	 //
	 // check whether the entries of the jacobians are as expected. They should be
	 //
	 //       (  -0.04  , 40000      ,  3e4  )
	 //   J = (   0.04  , -1.8004e8  , -3e4  )
	 //       (   0     , 1.8e8      ,  0    )
	 //
	 // note that the jacobian is stored column-wise i.e. J[0],J[1],J[2] are the
	 // first column of the jacobian
	 //
	 //
	BOOST_CHECK_CLOSE( Jac[0],     -0.04, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[3],     40000, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[6],       3e4, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[1],      0.04, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[4], -1.8004e8, 1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[7],      -3e4, 1.0e-9 );
	BOOST_CHECK_SMALL( Jac[2],            1.0e-9 );
	BOOST_CHECK_CLOSE( Jac[5],     1.8e8, 1.0e-9 );
	BOOST_CHECK_SMALL( Jac[8],            1.0e-9 );

	// free memory
	delete x;
	delete Jac;
}

/// Executes the test for the given zigzag model
void testFMUSimulateZigzag2(const string MODELNAME)
{
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiFalse, fmiFalse, EPS_TIME );
	fmiStatus status = fmu.instantiate( "zigzag21" );
	BOOST_REQUIRE_EQUAL( status, fmiOK );

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

BOOST_AUTO_TEST_CASE( test_fmu_simulate_zigzag2_0)
{
	testFMUSimulateZigzag2("zigzag2");
}

BOOST_AUTO_TEST_CASE( test_fmu_simulate_zigzag2_1)
{
	testFMUSimulateZigzag2("zigzag2_me_only");
}

/// Executes the test for the given zigzag model
void testFMU2LogBuffer(const string MODELNAME)
{
	// Retrieve the global instance of the log buffer.
	LogBuffer& logBuffer = LogBuffer::getLogBuffer();

	// Activate the global log buffer.
	logBuffer.activate();
	BOOST_REQUIRE_EQUAL( logBuffer.isActivated(), true );

	// load the zigzag FMU
	FMUModelExchange fmu( FMU_URI_PRE + MODELNAME, MODELNAME, fmiTrue, fmiFalse, EPS_TIME );

	fmiStatus status = fmu.instantiate( "zigzag21" );
	BOOST_REQUIRE_EQUAL( status, fmiOK );

	status = fmu.initialize();
	BOOST_REQUIRE_EQUAL( status, fmiOK );

	std::string logMessage = logBuffer.readFromBuffer();
	std::string expectedMessage( "zigzag21 [INSTANTIATE_MODEL]: instantiation successful\nzigzag21 [EXIT_INITIALIZATION_MODE]: initialization successful\n");

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

BOOST_AUTO_TEST_CASE( test_fmu2_log_buffer_0 )
{
	testFMU2LogBuffer("zigzag2");
}

BOOST_AUTO_TEST_CASE( test_fmu2_log_buffer_1 )
{
	testFMU2LogBuffer("zigzag2_me_only");
}

void testFMU2LogBufferAndCustomLogger(const string MODELNAME)
{
	// Load the FMU explicitly with the help of the model manager.
	ModelManager::LoadFMUStatus loadStatus = ModelManager::failed;
	FMUType type = invalid;
	loadStatus = ModelManager::loadFMU( MODELNAME, FMU_URI_PRE + MODELNAME, fmiFalse, type );

	// Check that loading the FMU was successful.
	BOOST_REQUIRE_MESSAGE( ( loadStatus == ModelManager::success ) || ( loadStatus == ModelManager::duplicate ), "FMU loading failed" );
	BOOST_REQUIRE_MESSAGE( type == fmi_2_0_me, "wrong FMU type" );

	// Instantiate first model and logging helper.
	FMUModelExchange fmu1( MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	FMULoggingHelper helper1( "helper1" );

	// Set custom logger and component environment.
	fmu1.setCallbacks( customLogger, callback2::allocateMemory, callback2::freeMemory );
	fmu1.setComponentEnvironment( reinterpret_cast<fmi2ComponentEnvironment>( &helper1 ) );

	// Instantiate second model and logging helper.
	FMUModelExchange fmu2( MODELNAME, fmiTrue, fmiFalse, EPS_TIME );
	FMULoggingHelper helper2( "helper2" );

	// Set custom logger and component environment.
	fmu2.setCallbacks( customLogger, callback2::allocateMemory, callback2::freeMemory );
	fmu2.setComponentEnvironment( reinterpret_cast<fmi2ComponentEnvironment>( &helper2 ) );

	// Instantiate first model.
	fmiStatus status = fmu1.instantiate( "fmu1" );
	BOOST_REQUIRE( status == fmiOK );

	// Instantiate second model.
	status = fmu2.instantiate( "fmu2" );
	BOOST_REQUIRE( status == fmiOK );

	// Retrieve the global instance of the log buffer.
	LogBuffer& logBuffer = LogBuffer::getLogBuffer();

	// Activate the global log buffer.
	logBuffer.activate();
	BOOST_REQUIRE_EQUAL( logBuffer.isActivated(), true );

	logBuffer.clear();

	string logMessage;

	for ( unsigned int iCall = 1; iCall < 10; ++iCall ) {
		fmu1.setTime( 0. );
		logMessage = logBuffer.readFromBuffer();
		stringstream fmu1ExpectedMessage;
		fmu1ExpectedMessage << "fmu1 [DEBUG]: helper1, call #" << iCall << " -> this is a test\n";
		BOOST_REQUIRE_MESSAGE( logMessage == fmu1ExpectedMessage.str(), 
			"not the expected log message (fmu1). Expected: \"" << 
			fmu1ExpectedMessage.str() << "\", received: \"" << logMessage << "\"" );

		// Clear the global log buffer.
		logBuffer.clear();
		BOOST_REQUIRE_MESSAGE( logBuffer.readFromBuffer() == string(), "global log buffer has not been cleared properly" );

		fmu2.setTime( 0. );
		string logMessage = logBuffer.readFromBuffer();
		stringstream fmu2ExpectedMessage;
		fmu2ExpectedMessage << "fmu2 [DEBUG]: helper2, call #" << iCall << " -> this is a test\n";
		BOOST_REQUIRE_MESSAGE( logMessage == fmu2ExpectedMessage.str(), "not the expected log message (fmu2)" );

		// Clear the global log buffer.
		logBuffer.clear();
		BOOST_REQUIRE_MESSAGE( logBuffer.readFromBuffer() == string(), "global log buffer has not been cleared properly" );
	}

	// Deactivate the global log buffer.
	logBuffer.deactivate();
	BOOST_REQUIRE_EQUAL( logBuffer.isActivated(), false );
}

BOOST_AUTO_TEST_CASE( test_fmu2_log_buffer_and_custom_logger_0 )
{
	testFMU2LogBufferAndCustomLogger("zigzag2");
}

BOOST_AUTO_TEST_CASE( test_fmu2_log_buffer_and_custom_logger_1 )
{
	testFMU2LogBufferAndCustomLogger("zigzag2_me_only");
}
