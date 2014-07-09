/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMIExportUtilities

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <cmath>

#include "export/functions/fmiFunctions.h"

#ifdef _MSC_VER
#pragma comment( linker, "/SUBSYSTEM:CONSOLE" )
#pragma comment( linker, "/ENTRY:mainCRTStartup" )
#endif


static  fmiCallbackFunctions functions = { 0, 0, 0 }; // FIXME: Callback functions are not being used yet!!!


BOOST_AUTO_TEST_CASE( test_trnsys_fmu )
{
	fmiStatus status = fmiFatal;

	fmiComponent trnsysSlave = fmiInstantiateSlave( "Type6139_Test",
							"{TRNSYS17-TYPE-6139-TEST-000000000000}",
							FMU_URI,
							"application/x-trnexe", 0, fmiTrue,
							fmiFalse, functions, fmiFalse );
	BOOST_REQUIRE_MESSAGE( 0 != trnsysSlave, "fmiInstantiateSlave(...) failed." );

	status = fmiInitializeSlave( trnsysSlave, 0., fmiFalse, 0. );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiInitializeSlave(...) failed." );

	fmiReal FMI_in;
	fmiValueReference FMI_in_ref = 1;
	status = fmiGetReal( trnsysSlave, &FMI_in_ref, 1, &FMI_in );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiGetReal(...) failed." );

	fmiReal FMI_out;
	fmiValueReference FMI_out_ref = 2;

	fmiReal time = 0.;
	fmiReal delta = 450.; // equals 1/8th of an hour.
	fmiReal eps = 1e-10;

	while ( time <= 100.*3600. ) {

		status = fmiGetReal( trnsysSlave, &FMI_out_ref, 1, &FMI_out );
		BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiGetReal(...) failed." );

		if ( ( static_cast<int>( time ) - 36000 )%72000 == 0 ) BOOST_CHECK_CLOSE( FMI_out, 1.0, eps );

		if ( FMI_out < 0. ) FMI_in *= -1.;

		status = fmiSetReal( trnsysSlave, &FMI_in_ref, 1, &FMI_in );
		BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiSetReal(...) failed." );

		status = fmiDoStep( trnsysSlave, time, delta, fmiTrue );
		BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiDoStep(...) failed." );

		time += delta;
	}

	status = fmiTerminateSlave( trnsysSlave );
	BOOST_REQUIRE_MESSAGE( fmiOK == status, "fmiTerminateSlave(...) failed." );

	fmiFreeSlaveInstance( trnsysSlave );
}
