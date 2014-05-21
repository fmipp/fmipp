public class testFMUModelExchange {
	public static void main( String [] args ) {
		System.loadLibrary("fmippim_wrap_java");
		FMU_URI_PRE = "file:///home/wolfi/fmipp/build/test/";
		EPS_TIME = 1e-9;
		test_fmu_load();
		test_fmu_instantiate();
		test_fmu_initialize();
		test_fmu_getvalue();
		test_fmu_setvalue();
		test_fmu_run_simulation_1();
		test_fmu_run_simulation_1_stop_before_event();
		test_fmu_run_simulation_2();
		test_fmu_run_simulation_2_stop_before_event();
		test_fmu_find_event();
		test_fmu_find_time_event();
	}

	private static void test_fmu_load() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
	}

	private static void test_fmu_instantiate() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "zigzag1", (char)0 );
		assert( status == fmiStatus.fmiOK );
	}

	private static void test_fmu_initialize() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "zigzag1", (char)0 );
		assert( status == fmiStatus.fmiOK );
		status = fmu.initialize();
		assert( status == fmiStatus.fmiOK );
	}

	private static void test_fmu_getvalue() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "zigzag1", (char)0 );
		assert( status == fmiStatus.fmiOK );
		status = fmu.initialize();
		assert( status == fmiStatus.fmiOK );
		double x;
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmiStatus.fmiOK );
		assert( x == 0.0 );
	}

	private static void test_fmu_setvalue() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "zigzag1", (char)0 );
		assert( status == fmiStatus.fmiOK );
		status = fmu.initialize();
		assert( status == fmiStatus.fmiOK );
		status = fmu.setRealValue( "x0", 0.5 );
		assert( status == fmiStatus.fmiOK );
		assert( status == fmiStatus.fmiOK );
		double x0;
		x0 = fmu.getRealValue( "x0" );
		assert( fmu.getLastStatus() == fmiStatus.fmiOK );
		assert( x0 == 0.5 );
	}

	private static void test_fmu_run_simulation_1() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "zigzag1", (char)0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.setRealValue( "k", 1.0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.initialize();
		assert( status == fmiStatus.fmiOK );

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
		assert( fmu.getLastStatus() == fmiStatus.fmiOK );
		assert( Math.abs( x - 1.0 ) < 1e-6 );
	}

	private static void test_fmu_run_simulation_1_stop_before_event() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "zigzag1", (char)0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.setRealValue( "k", 1.0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.initialize();
		assert( status == fmiStatus.fmiOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmiStatus.fmiOK );
		}

		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmiStatus.fmiOK );
		assert( Math.abs( x - 1.0 ) < 1e-6 );
	}

	private static void test_fmu_run_simulation_2() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "zigzag1", (char)0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.setRealValue( "k", 10.0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.initialize();
		assert( status == fmiStatus.fmiOK );

		double t = 0.0;
		double stepsize = 0.025;
		double tstop = 1.0;
		double x;
		double dx;
		int eventctr = 0;

		while ( t < tstop ) {
			t = fmu.integrate( Math.min( t + stepsize, tstop ) );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmiStatus.fmiOK );
			dx = fmu.getRealValue( "der(x)" );
			assert( fmu.getLastStatus() == fmiStatus.fmiOK );
			if ( fmu.getEventFlag() != (char)0 ) {
				eventctr++;
				fmu.setEventFlag( (char)0 );
			}
		}

		assert( eventctr == 5 );
		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmiStatus.fmiOK );
		assert( Math.abs( x - 0.0 ) < 1e-6 );
	}

	private static void test_fmu_run_simulation_2_stop_before_event() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "zigzag1", (char)0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.setRealValue( "k", 10.0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.initialize();
		assert( status == fmiStatus.fmiOK );

		double t = 0.0;
		double stepsize = 0.025;
		double tstop = 1.0;
		double x;
		double dx;
		int eventctr = 0;

		while ( t < tstop ) {
			t = fmu.integrate( Math.min( t + stepsize, tstop ) );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmiStatus.fmiOK );
			dx = fmu.getRealValue( "der(x)" );
			assert( fmu.getLastStatus() == fmiStatus.fmiOK );
			if ( fmu.getEventFlag() != (char)0 ) {
				eventctr++;
				fmu.setEventFlag( (char)0 );
			}
		}

		assert( eventctr == 5 );
		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmiStatus.fmiOK );
		assert( Math.abs( x - 0.0 ) < 1e-6 );
	}

	private static void test_fmu_find_event() {
		String MODELNAME = "zigzag";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "zigzag1", (char)0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.setRealValue( "k", 2.0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.initialize();
		assert( status == fmiStatus.fmiOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;
		int eventctr = 0;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			if ( fmu.getEventFlag() != (char)0 ) {
				assert( Math.abs( t - 0.5 ) < 0.0025 );
				eventctr++;
				fmu.setEventFlag( (char)0 );
			}
		}

		assert( eventctr == 1 );
		t = fmu.getTime();
		assert( Math.abs( t - tstop ) < stepsize/2 );
		x = fmu.getRealValue( "x" );
		assert( fmu.getLastStatus() == fmiStatus.fmiOK );
		assert( Math.abs( x - 0.0 ) < 1e-6 );
	}

	private static void test_fmu_find_time_event() {
		String MODELNAME = "step_t0";
		FMUModelExchange fmu = new FMUModelExchange( FMU_URI_PRE + MODELNAME, MODELNAME, (char)0, EPS_TIME );
		fmiStatus status = fmu.instantiate( "step_t01", (char)0 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.setRealValue( "t0", 0.5 );
		assert( status == fmiStatus.fmiOK );

		status = fmu.initialize();
		assert( status == fmiStatus.fmiOK );

		double t = 0.0;
		double stepsize = 0.0025;
		double tstop = 1.0;
		double x;

		while ( ( t + stepsize ) - tstop < EPS_TIME ) {
			t = fmu.integrate( t + stepsize );
			x = fmu.getRealValue( "x" );
			assert( fmu.getLastStatus() == fmiStatus.fmiOK );
			if ( t < 0.5 ) {
				assert( x == 0 );
			} else {
				assert( x == 1 );
			}
		}
	}

	private static String FMU_URI_PRE;
	private static double EPS_TIME;
}
