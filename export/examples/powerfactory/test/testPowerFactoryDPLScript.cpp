/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testPowerFactoryDPLScript

/// \file testPowerFactoryTriggers.cpp

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <cmath>

#include "import/base/include/CallbackFunctions.h"
#include "export/functions/fmiFunctions.h"

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



BOOST_AUTO_TEST_CASE( test_power_factory_fmu_dplscript )
{
	fmiStatus status = fmiFatal;

	std::string fmuLocation = std::string( FMU_URI_BASE ) + std::string( "/dplscript" );

	fmiComponent pfSlave = fmiInstantiateSlave( "PFTestDPLScript",
						    "{DIGPF150-TEST-0000-0000-dplscript000}",
						    fmuLocation.c_str(),
						    "application/x-powerfactory", 0, fmiTrue,
						    fmiFalse, functions, fmiTrue );
	BOOST_REQUIRE_MESSAGE( 0 != pfSlave, "fmiInstantiateSlave(...) failed." );

	fmiReal tStart = 0.;

	status = fmiInitializeSlave( pfSlave, tStart, fmiFalse, 0. );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiInitializeSlave(...) failed." );

	//
	// Test doStep(...) function.
	//
	fmiReal time = tStart;
	fmiReal delta = 300.;

	while ( time <= 3600.0 )
	{
		status = fmiDoStep( pfSlave, time, delta, true );
		BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiDoStep(...) failed." );

		time += delta;
	}

	status = fmiTerminateSlave( pfSlave );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiTerminateSlave(...) failed." );

	fmiFreeSlaveInstance( pfSlave );
}
