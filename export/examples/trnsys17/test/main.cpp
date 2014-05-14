/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#include "fmiFunctions.h"


static  fmiCallbackFunctions functions = { 0, 0, 0 }; // FIXME: Callback functions are not being used yet!!!


int main ( int argc, char* argv[] )
{
	fmiComponent trnsysSlave1 = fmiInstantiateSlave( "myTrnsysModel",
							 "{TRNSYS17-TYPE-6139-TEST-000000000000}",
							 "file:///C:/Development/fmipp/co-sim/examples/trnsys17/test",
							 "application/x-trnsys17", 0, fmiTrue,
							 fmiFalse, functions, fmiFalse );

	fmiInitializeSlave( trnsysSlave1, 0., fmiFalse, 0. );

	fmiReal input1;
	fmiValueReference inputRef1 = 1;

	fmiReal input2;
	fmiValueReference inputRef2 = 2;

	fmiReal output1;
	fmiValueReference outputRef1 = 3;

	fmiReal output2;
	fmiValueReference outputRef2 = 4;

	fmiReal time = 0.;
	fmiReal delta = 450.; // equals 1/8th of an hour.

	fmiStatus doStepStatus = fmiOK;

	while ( fmiOK == doStepStatus ) {

		/// input1 -> output1.
		fmiGetReal( trnsysSlave1, &outputRef1, 1, &output1 );
		if ( output1 > 50. ) input1 = -1.;
		if ( output1 < 10. ) input1 = 1.;
		fmiSetReal( trnsysSlave1, &inputRef1, 1, &input1 );

		/// input2 -> output2.
		fmiGetReal( trnsysSlave1, &outputRef2, 1, &output2 );
		if ( output2 > 45. ) input2 = -0.5;
		if ( output2 < 20. ) input2 = 1.5;
		fmiSetReal( trnsysSlave1, &inputRef2, 1, &input2 );

		doStepStatus = fmiDoStep( trnsysSlave1, time, delta, fmiTrue );

		time += delta;
	}


	fmiTerminateSlave( trnsysSlave1 );
	fmiFreeSlaveInstance( trnsysSlave1 );

	return 0;
}
