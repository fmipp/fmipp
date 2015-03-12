// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

public class testIncrementalFMU {
	public static void main( String [] args ) {
		System.loadLibrary("fmippim_wrap_java");
		FMU_URI_PRE = args[0];
		EPS_TIME = Double.parseDouble( args[1] );
		test_fmu_load();
		test_fmu_init();
	}

	private static void test_fmu_load() {
		String MODELNAME = "zigzag";
		IncrementalFMU fmu = new IncrementalFMU( FMU_URI_PRE + MODELNAME, MODELNAME );
	}

	private static void test_fmu_init() {
		String MODELNAME = "zigzag";
		IncrementalFMU fmu = new IncrementalFMU( FMU_URI_PRE + MODELNAME, MODELNAME );

		SWIGTYPE_p_std__string foo;
		SWIGTYPE_p_double bar;

		foo = fmippim.new_string_array( 2 );
		bar = fmippim.new_double_array( 2 );

		fmippim.string_array_setitem(foo, 0, "k");
		fmippim.double_array_setitem(bar, 0, 10.0);

		fmippim.string_array_setitem(foo, 1, "x");
		fmippim.double_array_setitem(bar, 1, 1.0);

		double starttime = 0.0;
		double stepsize = 0.0025;

		double horizon = 2 * stepsize;
		double intstepsize = stepsize/2;

		int status = fmu.init( "zigzag1", foo, bar, 2, starttime, horizon, stepsize, intstepsize );
		assert( status == 1 );
	}

	private static String FMU_URI_PRE;
	private static double EPS_TIME;
}
