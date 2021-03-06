// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

import at.ac.ait.fmipp.imp.*;


public class testFMUModelExchange {
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
		test_fmi_1_0_run_simulation_1();
		test_fmi_1_0_run_simulation_1_stop_before_event();
		test_fmi_1_0_run_simulation_2();
		test_fmi_1_0_run_simulation_2_stop_before_event();
		test_fmi_1_0_find_event();
		test_fmi_1_0_find_time_event();

		// FMI 2.0
		test_fmi_2_0_load();
		test_fmi_2_0_instantiate();
		test_fmi_2_0_initialize();
		test_fmi_2_0_getvalue();
		test_fmi_2_0_setvalue();
		test_fmi_2_0_run_simulation_1();
		test_fmi_2_0_run_simulation_1_stop_before_event();
		test_fmi_2_0_run_simulation_2();
		test_fmi_2_0_run_simulation_2_stop_before_event();
		test_fmi_2_0_find_event();
	}

	private static void test_fmi_1_0_load() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
	}

	private static void test_fmi_1_0_instantiate() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );
	}

	private static void test_fmi_1_0_initialize() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );
	}

	private static void test_fmi_1_0_getvalue() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );
		double x;
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( x == 0.0 );
	}

	private static void test_fmi_1_0_setvalue() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );
		status = fmu.setRealValue( "x0", 0.5 );
		assert( status == fmippStatus.fmippOK );
		double x0;
		x0 = fmu.getRealValue( "x0" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( x0 == 0.5 );
	}

	private static void test_fmi_1_0_run_simulation_1() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 1.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			x = fmu.getRealValue( "x" );
		}

		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 1.0 ) < 1e-6 );
	}

	private static void test_fmi_1_0_run_simulation_1_stop_before_event() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, true, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 1.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		}

		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 1.0 ) < 1e-6 );
	}

	private static void test_fmi_1_0_run_simulation_2() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 10.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.025;
		double tstop = 1.0;
		double x;
		double dx;
		int eventctr = 0;

		while ( t < tstop ) {
			t = fmu.integrate( Math.min( t + stepsize, tstop ) );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
			dx = fmu.getRealValue( "der(x)" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
			if ( fmu.getEventFlag() != false ) {
				eventctr++;
				fmu.setEventFlag( false );
			}
		}

		assert( eventctr == 5 );
		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 0.0 ) < 1e-6 );
	}

	private static void test_fmi_1_0_run_simulation_2_stop_before_event() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, true, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 10.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.025;
		double tstop = 1.0;
		double x;
		double dx;
		int eventctr = 0;

		while ( t < tstop ) {
			t = fmu.integrate( Math.min( t + stepsize, tstop ) );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
			dx = fmu.getRealValue( "der(x)" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
			if ( fmu.getEventFlag() != false ) {
				eventctr++;
				fmu.setEventFlag( false );
			}
		}

		assert( eventctr == 5 );
		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 0.0 ) < 1e-6 );
	}

	private static void test_fmi_1_0_find_event() {
		String MODELNAME = "zigzag";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 2.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;
		int eventctr = 0;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			if ( fmu.getEventFlag() != false ) {
				assert( Math.abs( t - 0.5 ) < 0.0025 );
				eventctr++;
				fmu.setEventFlag( false );
			}
		}

		assert( eventctr == 1 );
		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 0.0 ) < 1e-6 );
	}

	private static void test_fmi_1_0_find_time_event() {
		String MODELNAME = "step_t0";
		FMUModelExchangeV1 fmu = new FMUModelExchangeV1( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "step_t01" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "t0", 0.5 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
			if ( t < 0.5 ) {
				assert( x == 0 );
			} else {
				assert( x == 1 );
			}
		}
	}

	private static void test_fmi_2_0_load() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
	}

	private static void test_fmi_2_0_instantiate() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );
	}

	private static void test_fmi_2_0_initialize() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );
	}

	private static void test_fmi_2_0_getvalue() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );
		double x;
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( x == 0.0 );
	}

	private static void test_fmi_2_0_setvalue() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );
		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );
		status = fmu.setRealValue( "x0", 0.5 );
		assert( status == fmippStatus.fmippOK );
		double x0;
		x0 = fmu.getRealValue( "x0" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( x0 == 0.5 );
	}

	private static void test_fmi_2_0_run_simulation_1() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 1.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			x = fmu.getRealValue( "x" );
		}

		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 1.0 ) < 1e-6 );
	}

	private static void test_fmi_2_0_run_simulation_1_stop_before_event() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, true, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 1.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		}

		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 1.0 ) < 1e-6 );
	}

	private static void test_fmi_2_0_run_simulation_2() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 10.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.025;
		double tstop = 1.0;
		double x;
		double dx;
		int eventctr = 0;

		while ( t < tstop ) {
			t = fmu.integrate( Math.min( t + stepsize, tstop ) );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
			dx = fmu.getRealValue( "der(x)" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
			if ( fmu.getEventFlag() != false ) {
				eventctr++;
				fmu.setEventFlag( false );
			}
		}

		assert( eventctr == 5 );
		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 0.0 ) < 1e-6 );
	}

	private static void test_fmi_2_0_run_simulation_2_stop_before_event() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 10.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.025;
		double tstop = 1.0;
		double x;
		double dx;
		int eventctr = 0;

		while ( t < tstop ) {
			t = fmu.integrate( Math.min( t + stepsize, tstop ) );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
			dx = fmu.getRealValue( "der(x)" );
			assert( fmu.getLastStatus() == fmippStatus.fmippOK );
			if ( fmu.getEventFlag() != false ) {
				eventctr++;
				fmu.setEventFlag( false );
			}
		}

		assert( eventctr == 5 );
		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 0.0 ) < 1e-6 );
	}

	private static void test_fmi_2_0_find_event() {
		String MODELNAME = "zigzag2";
		FMUModelExchangeV2 fmu = new FMUModelExchangeV2( FMU_URI_PRE + MODELNAME, MODELNAME, false, false, EPS_TIME );
		fmippStatus status = fmu.instantiate( "zigzag1" );
		assert( status == fmippStatus.fmippOK );

		status = fmu.setRealValue( "k", 2.0 );
		assert( status == fmippStatus.fmippOK );

		status = fmu.initialize();
		assert( status == fmippStatus.fmippOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;
		int eventctr = 0;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			if ( fmu.getEventFlag() != false ) {
				assert( Math.abs( t - 0.5 ) < 0.0025 );
				eventctr++;
				fmu.setEventFlag( false );
			}
		}

		assert( eventctr == 1 );
		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmippStatus.fmippOK );
		assert( Math.abs( x - 0.0 ) < 1e-6 );
	}

	private static String FMU_URI_PRE;
	private static double EPS_TIME;
}
