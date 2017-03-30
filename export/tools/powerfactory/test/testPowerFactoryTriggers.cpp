/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file testPowerFactoryTriggers.cpp

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testPowerFactoryTriggers

// #ifndef _CRT_SECURE_NO_WARNINGS
// #define _CRT_SECURE_NO_WARNINGS
// #endif

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <cmath>
#include <cstdio>

#include "import/base/include/CallbackFunctions.h"
#include "export/functions/fmi_v1.0/fmiFunctions.h"

#ifdef _MSC_VER
#pragma comment( linker, "/SUBSYSTEM:CONSOLE" )
#pragma comment( linker, "/ENTRY:mainCRTStartup" )
#endif


namespace
{
	// Define callback functions.
	static  fmiCallbackFunctions functions =
	{ callback::verboseLogger, callback::allocateMemory, callback::freeMemory, callback::stepFinished };

	// Check values with a precision of 5e-3 percent.
	const double testPrecision = 5e-3;
}



BOOST_AUTO_TEST_CASE( test_power_factory_fmu_triggers )
{
	fmiStatus status = fmiFatal;

	std::string fmuLocation = std::string( FMU_URI_BASE ) + std::string( "/triggers" );

	fmiComponent pfSlave = fmiInstantiateSlave( "PFTestTriggers",
						    "{DIGPF152-TEST-0000-0000-triggers0000}",
						    fmuLocation.c_str(),
						    "application/x-powerfactory", 0, fmiTrue,
						    fmiFalse, functions, fmiTrue );
	BOOST_REQUIRE_MESSAGE( 0 != pfSlave, "fmiInstantiateSlave(...) failed." );

	fmiReal tStart = 0.;

	status = fmiInitializeSlave( pfSlave, tStart, fmiFalse, 0. );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiInitializeSlave(...) failed." );

	fmiReal plini_load;
	fmiValueReference plini_load_ref = 1;

	fmiReal mu;
	fmiValueReference mu_ref = 1001;

	fmiReal psum_gen;
	fmiValueReference psum_gen_ref = 1002;

	// There is no model variable defined with this value reference.
	fmiReal fake;
	fmiValueReference fake_ref = 1003;


	//
	// Test getter function.
	//

	status = fmiGetReal( pfSlave, &fake_ref, 1, &fake );
	BOOST_REQUIRE_MESSAGE( fmiWarning == status,
			       "fmiGetReal(...) should not be able to get non-existent variables." );

	status = fmiGetReal( pfSlave, &plini_load_ref, 1, &plini_load );
	BOOST_REQUIRE_MESSAGE( fmiOK == status,
			       "fmiGetReal(...) failed." );
	// Compare against start value from XML model description (0.1), not 
	// the original value specified in the PowerFactory model (0.4).
	BOOST_CHECK_CLOSE( plini_load, 0.1, testPrecision );

	status = fmiGetReal( pfSlave, &psum_gen_ref, 1, &psum_gen );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiGetReal(...) failed." );
	// Compare against (linearly interpolated) value from input characteristics.
	BOOST_CHECK_CLOSE( psum_gen, -0.6, testPrecision );

	status = fmiGetReal( pfSlave, &mu_ref, 1, &mu );
	BOOST_REQUIRE_MESSAGE( fmiOK == status,
			       "fmiGetReal(...) failed." );
	// This value results from an initial load flow calculation done
	// during the initialization of the FMU.
	BOOST_CHECK_CLOSE( mu, 1.035966, testPrecision );


	//
	// Test setter function.
	//

	status = fmiSetReal( pfSlave, &fake_ref, 1, &fake );
	BOOST_REQUIRE_MESSAGE( fmiWarning == status,
			       "fmiGetReal(...) should not be able to set non-existent variables." );

	status = fmiSetReal( pfSlave, &mu_ref, 1, &mu );
	BOOST_REQUIRE_MESSAGE( fmiWarning == status,
			       "fmiSetReal(...) should not be able to set outputs." );

	double setplini_load = 0.6;
	status = fmiSetReal( pfSlave, &plini_load_ref, 1, &setplini_load );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiSetReal(...) failed." );
	status = fmiGetReal( pfSlave, &plini_load_ref, 1, &plini_load );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiGetReal(...) failed." );
	BOOST_CHECK_CLOSE( plini_load, setplini_load, testPrecision );	


	//
	// Test doStep(...) function.
	//
	fmiReal time = tStart;
	fmiReal delta = 60.;

	fmiReal muCompare[5] = { 1.026309, 1.020366, 1.014217, 1.00785, 1.001254 };
	unsigned int iCompare = 0;

	while ( time < tStart + 5*delta )
	{
		status = fmiDoStep( pfSlave, time, delta, true );
		BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiDoStep(...) failed." );

		status = fmiGetReal( pfSlave, &psum_gen_ref, 1, &psum_gen );
		BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiGetReal(...) failed." );

		status = fmiGetReal( pfSlave, &mu_ref, 1, &mu );
		BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiGetReal(...) failed." );

		BOOST_CHECK_CLOSE( mu, muCompare[iCompare], testPrecision );

		time += delta;
		++iCompare;
	}


	//
	// Terminate simulation.
	//

	status = fmiTerminateSlave( pfSlave );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiTerminateSlave(...) failed." );

	fmiFreeSlaveInstance( pfSlave );

	//
	// Clean-up
	//

	remove( "extra_outputs.info" );

}
