// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

import at.ac.ait.fmipp.imp.*;


public class testFMUCoSimulation {
	public static void main( String [] args ) {
		System.loadLibrary("fmippim_wrap_java");
		FMU_URI_PRE = args[0];
		EPS_TIME = Double.parseDouble( args[1] );

		// FMI 1.0
		test_fmi_1_0_load();
		test_fmi_1_0_instantiate();
		test_fmi_1_0_initialize();
		test_fmi_1_0_getvalue();
		test_fmi_1_0_setvalue();
		test_fmi_1_0_run_simulation();

		// FMI 2.0
		test_fmi_2_0_load();
		test_fmi_2_0_instantiate();
		test_fmi_2_0_initialize();
		test_fmi_2_0_getvalue();
		test_fmi_2_0_setvalue();
		test_fmi_2_0_run_simulation();
	}

	private static void test_fmi_1_0_load() {
		String MODELNAME = "sine_standalone";
		FMUCoSimulationV1 fmu = new FMUCoSimulationV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmu.terminate();
	}

	private static void test_fmi_1_0_instantiate() {
		String MODELNAME = "sine_standalone";
		FMUCoSimulationV1 fmu = new FMUCoSimulationV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );
		fmu.terminate();
	}

	private static void test_fmi_1_0_initialize() {
		String MODELNAME = "sine_standalone";
		FMUCoSimulationV1 fmu = new FMUCoSimulationV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize( 0., true, 10. );
		assert( status == fmippStatus.fmippOK );
		fmu.terminate();
	}

	private static void test_fmi_1_0_getvalue() {
		String MODELNAME = "sine_standalone";
		FMUCoSimulationV1 fmu = new FMUCoSimulationV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize( 0., true, 10. );
		assert( status == fmippStatus.fmippOK );
		double x;
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( x == 0.0 );
		double omega;
		omega = fmu.getRealValue( "omega" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( omega == 1.0 );
		fmu.terminate();
	}

	private static void test_fmi_1_0_setvalue() {
		String MODELNAME = "sine_standalone";
		FMUCoSimulationV1 fmu = new FMUCoSimulationV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize( 0., true, 10. );
		assert( status == fmippStatus.fmippOK );
		status = fmu.setRealValue( "omega", 0.123 );
		assert( status == fmippStatus.fmippOK );
		double omega;
		omega = fmu.getRealValue( "omega" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( omega == 0.123 );
		fmu.terminate();
	}

	private static void test_fmi_1_0_run_simulation() {
		String MODELNAME = "sine_standalone";
		FMUCoSimulationV1 fmu = new FMUCoSimulationV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );

		double omega = 0.628318531; // Corresponds to a period of 10s.
		status = fmu.setRealValue( "omega", omega );
		assert( status == fmippStatus.fmippOK );

		double t = 0.;
		double stepsize = 1.;
		double tstop = 10.;
		double x = 0.;
		int cycles = 0;
		boolean positive = true;
		double twopi = 6.28318530718;

		status = fmu.initialize(  t, true, tstop );
		assert( status == fmippStatus.fmippOK );

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			// Make co-simulation step.
			status = fmu.doStep( t, stepsize, true );
			assert( status == fmippStatus.fmippOK );

			// Advance time.
			t += stepsize;
			assert( Math.abs( t - fmu.getTime() ) < EPS_TIME );

			// Retrieve result.
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );

			cycles = fmu.getIntegerValue( "cycles" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );

			positive = fmu.getBooleanValue( "positive" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );

			assert( Math.abs( x - Math.sin( omega*t ) ) < 1e-9 );

			assert( cycles == (int)( omega*t/twopi ) );

			assert( positive == ( ( x > 0. ) ? true : false ) );
		}

		assert( Math.abs( tstop - fmu.getTime() ) < EPS_TIME );

		fmu.terminate();
	}


		private static void test_fmi_2_0_load() {
		String MODELNAME = "sine_standalone2";
		FMUCoSimulationV2 fmu = new FMUCoSimulationV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmu.terminate();
	}

	private static void test_fmi_2_0_instantiate() {
		String MODELNAME = "sine_standalone2";
		FMUCoSimulationV2 fmu = new FMUCoSimulationV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone2_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );
		fmu.terminate();
	}

	private static void test_fmi_2_0_initialize() {
		String MODELNAME = "sine_standalone2";
		FMUCoSimulationV2 fmu = new FMUCoSimulationV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone2_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize( 0., true, 10. );
		assert( status == fmippStatus.fmippOK );
		fmu.terminate();
	}

	private static void test_fmi_2_0_getvalue() {
		String MODELNAME = "sine_standalone2";
		FMUCoSimulationV2 fmu = new FMUCoSimulationV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone2_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize( 0., true, 10. );
		assert( status == fmippStatus.fmippOK );
		double x;
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( x == 0.0 );
		double omega;
		omega = fmu.getRealValue( "omega" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( omega == 1.0 );
		fmu.terminate();
	}

	private static void test_fmi_2_0_setvalue() {
		String MODELNAME = "sine_standalone2";
		FMUCoSimulationV2 fmu = new FMUCoSimulationV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone2_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize( 0., true, 10. );
		assert( status == fmippStatus.fmippOK );
		status = fmu.setRealValue( "omega", 0.123 );
		assert( status == fmippStatus.fmippOK );
		double omega;
		omega = fmu.getRealValue( "omega" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( omega == 0.123 );
		fmu.terminate();
	}

	private static void test_fmi_2_0_run_simulation() {
		String MODELNAME = "sine_standalone2";
		FMUCoSimulationV2 fmu = new FMUCoSimulationV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "sine_standalone2_instance1", 0., false, false );
		assert( status == fmippStatus.fmippOK );

		double omega = 0.628318531; // Corresponds to a period of 10s.
		status = fmu.setRealValue( "omega", omega );
		assert( status == fmippStatus.fmippOK );

		double t = 0.;
		double stepsize = 1.;
		double tstop = 10.;
		double x = 0.;
		int cycles = 0;
		boolean positive = true;
		double twopi = 6.28318530718;

		status = fmu.initialize(  t, true, tstop );
		assert( status == fmippStatus.fmippOK );

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			// Make co-simulation step.
			status = fmu.doStep( t, stepsize, true );
			assert( status == fmippStatus.fmippOK );

			// Advance time.
			t += stepsize;
			assert( Math.abs( t - fmu.getTime() ) < EPS_TIME );

			// Retrieve result.
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );

			cycles = fmu.getIntegerValue( "cycles" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );

			positive = fmu.getBooleanValue( "positive" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );

			assert( Math.abs( x - Math.sin( omega*t ) ) < 1e-9 );

			assert( cycles == (int)( omega*t/twopi ) );

			assert( positive == ( ( x > 0. ) ? true : false ) );
		}

		assert( Math.abs( tstop - fmu.getTime() ) < EPS_TIME );

		fmu.terminate();
	}


	private static String FMU_URI_PRE;
	private static double EPS_TIME;
}
