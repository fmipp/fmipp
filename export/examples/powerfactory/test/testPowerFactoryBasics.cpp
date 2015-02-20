/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testPowerFactoryBasics

/// \file testPowerFactoryBasics.cpp

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
	unsigned int iFatal = 0;
	std::string lastFatalCategory;

	// This logger counts the number of messages that are issued with status 'fmiFatal'.
	// In addition, the category of the last such message is stored.
	void testLogger( fmiComponent c, fmiString instanceName, fmiStatus status,
			 fmiString category, fmiString message, ... )
	{
		if ( fmiFatal == status ) {
			iFatal++;
			lastFatalCategory = category;
		}

		callback::logger( c, instanceName, status, category, message );
	}

	// Define callback functions.
	static  fmiCallbackFunctions functions =
	{ testLogger, callback::allocateMemory, callback::freeMemory, callback::stepFinished };

	// Check values with a precision of 5e-3 percent.
	const double testPrecision = 5e-3;
}



BOOST_AUTO_TEST_CASE( test_power_factory_fmu_bad_uri )
{
	iFatal = 0;

	fmiStatus status = fmiFatal;

	std::string fmuLocation( "this is not an URI" ); // <-- this is not a valid URI

	fmiComponent pfSlave = fmiInstantiateSlave( "PFTestBasics",
						    "{DIGPF150-TEST-0000-0000-testbasics00}",
						    fmuLocation.c_str(),
						    "application/x-digpf", 0, fmiTrue,
						    fmiFalse, functions, fmiTrue );

	// The instantiation should fail, because the model description URI is faulty.
	BOOST_REQUIRE_MESSAGE( 0 == pfSlave, "fmiInstantiateSlave should have failed (bad URI)" );

	// Check if the instantiation failed due to the expected reason.
	BOOST_REQUIRE_MESSAGE( 1 == iFatal,
			       "exactly one message with status 'fmiFatal' should have been issued " );
	BOOST_REQUIRE_MESSAGE( 0 == lastFatalCategory.compare( "URL" ),
			       "the category of the last issued message is expected to be 'URL'" );

}


BOOST_AUTO_TEST_CASE( test_power_factory_fmu_bad_file_path )
{
	iFatal = 0;

	fmiStatus status = fmiFatal;

 	std::string fmuLocation = std::string( FMU_URI_BASE ) + std::string( "/basics" );
	fmuLocation += ( "/invalid/path" ); // <-- this is not the correct URI

	fmiComponent pfSlave = fmiInstantiateSlave( "PFTestBasics",
						    "{DIGPF150-TEST-0000-0000-testbasics00}",
						    fmuLocation.c_str(),
						    "application/x-digpf", 0, fmiTrue,
						    fmiFalse, functions, fmiTrue );

	// The instantiation should fail, because the model description URI is faulty.
	BOOST_REQUIRE_MESSAGE( 0 == pfSlave, "fmiInstantiateSlave should have failed (bad file path)" );

	// Check if the instantiation failed due to the expected reason.
	BOOST_REQUIRE_MESSAGE( 1 == iFatal,
			       "exactly one message with status 'fmiFatal' should have been issued " );
	BOOST_REQUIRE_MESSAGE( 0 == lastFatalCategory.compare( "MODEL-DESCRIPTION" ),
			       "the category of the last issued message is expected to be 'MODEL-DESCRIPTION'" );

}


BOOST_AUTO_TEST_CASE( test_power_factory_fmu_wrong_guid )
{
	iFatal = 0;

	fmiStatus status = fmiFatal;

 	std::string fmuLocation = std::string( FMU_URI_BASE ) + std::string( "/basics" );

	fmiComponent pfSlave = fmiInstantiateSlave( "PFTestBasics",
						    "{00000000000-0000-0000}", // <-- this is not the correct GUID
						    fmuLocation.c_str(),
						    "application/x-digpf", 0, fmiTrue,
						    fmiFalse, functions, fmiTrue );

	// The instantiation should fail, because the GUID is wrong.
	BOOST_REQUIRE_MESSAGE( 0 == pfSlave, "fmiInstantiateSlave should have failed (wrong GUID)" );

	// Check if the instantiation failed due to the expected reason.
	BOOST_REQUIRE_MESSAGE( 1 == iFatal,
			       "exactly one message with status 'fmiFatal' should have been issued " );
	BOOST_REQUIRE_MESSAGE( 0 == lastFatalCategory.compare( "GUID" ),
			       "the category of the last issued message is expected to be 'GUID'" );

}


BOOST_AUTO_TEST_CASE( test_power_factory_fmu_wrong_mime_type )
{
	iFatal = 0;

	fmiStatus status = fmiFatal;

 	std::string fmuLocation = std::string( FMU_URI_BASE ) + std::string( "/basics" );

	fmiComponent pfSlave = fmiInstantiateSlave( "PFTestBasics",
						    "{DIGPF150-TEST-0000-0000-testbasics00}",
						    fmuLocation.c_str(),
						    "application/x-something",  // <-- incorrect MIME type
						    0, fmiTrue, fmiFalse, functions, fmiTrue );

	// The instantiation should fail, because the GUID is wrong.
	BOOST_REQUIRE_MESSAGE( 0 == pfSlave, "fmiInstantiateSlave should have failed (wrong MIME type)" );

	// Check if the instantiation failed due to the expected reason.
	BOOST_REQUIRE_MESSAGE( 1 == iFatal,
			       "exactly one message with status 'fmiFatal' should have been issued " );
	BOOST_REQUIRE_MESSAGE( 0 == lastFatalCategory.compare( "MIME-TYPE" ),
			       "the category of the last issued message is expected to be 'MIME-TYPE'" );

}



BOOST_AUTO_TEST_CASE( test_power_factory_fmu_faulty_model_description )
{
	iFatal = 0;

	fmiStatus status = fmiFatal;

	std::string fmuLocation = std::string( FMU_URI_BASE ) + std::string( "/basics" );

	fmiComponent pfSlave = fmiInstantiateSlave( "PFTestBasics",
						    "{DIGPF150-TEST-0000-0000-testbasics00}",
						    fmuLocation.c_str(),
						    "application/x-digpf", 0, fmiTrue,
						    fmiFalse, functions, fmiTrue );

	// The instantiation should fail, because the model description is faults (it contains nodes
	// defining the time advance mechanism via DPL script and triggers).
	BOOST_REQUIRE_MESSAGE( 0 == pfSlave, "fmiInstantiateSlave should have failed (bad model description)" );

	// Check if the instantiation failed due to the expected reason.
	BOOST_REQUIRE_MESSAGE( 1 == iFatal,
			       "exactly one message with status 'fmiFatal' should have been issued " );
	BOOST_REQUIRE_MESSAGE( 0 == lastFatalCategory.compare( "TIME-ADVANCE" ),
			       "the category of the last issued message is expected to be 'TIME-ADVANCE'" );

}
