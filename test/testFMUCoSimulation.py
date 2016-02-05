# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

import fmippim
import unittest

class testFMUCoSimulation(unittest.TestCase):
  def test_fmi_1_0_load(self):
    model_name = 'sine_standalone';
    fmu = fmippim.FMUCoSimulation( FMU_URI_PRE + model_name, model_name )
  
  def test_fmi_1_0_instantiate(self):
    model_name = 'sine_standalone';
    fmu = fmippim.FMUCoSimulation( FMU_URI_PRE + model_name, model_name )
    status = fmu.instantiate( "sine_standalone1", 0., False, False )
    self.assertEqual( status, fmippim.fmiOK )

  def test_fmi_1_0_initialize(self):
    model_name = 'sine_standalone';
    fmu = fmippim.FMUCoSimulation( FMU_URI_PRE + model_name, model_name )
    status = fmu.instantiate( "sine_standalone1", 0., False, False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize( 0., True, 10. );
    self.assertEqual( status, fmippim.fmiOK )


  def test_fmi_1_0_getvalue(self):
    model_name = 'sine_standalone';
    fmu = fmippim.FMUCoSimulation( FMU_URI_PRE + model_name, model_name )
    status = fmu.instantiate( "sine_standalone1", 0., False, False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize( 0., True, 10. );
    self.assertEqual( status, fmippim.fmiOK )
    x = fmu.getRealValue( 'x' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )
    self.assertEqual( x, 0.0 )
    omega = fmu.getRealValue( 'omega' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )
    self.assertEqual( omega, 1.0 )

  def test_fmi_1_0_setvalue(self):
    model_name = 'sine_standalone';
    fmu = fmippim.FMUCoSimulation( FMU_URI_PRE + model_name, model_name )
    status = fmu.instantiate( "sine_standalone1", 0., False, False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize( 0., True, 10. );
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.setRealValue( 'omega', 0.123 )
    self.assertEqual( status, fmippim.fmiOK )
    omega = fmu.getRealValue( 'omega' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )
    self.assertEqual( omega, 0.123 )

  def test_fmi_1_0_run_simulation(self):
    import math
	
    model_name = 'sine_standalone';
    fmu = fmippim.FMUCoSimulation( FMU_URI_PRE + model_name, model_name )
    status = fmu.instantiate( "sine_standalone1", 0., False, False )
    self.assertEqual( status, fmippim.fmiOK )
    
    omega = 0.628318531
    status = fmu.setRealValue( 'omega', omega )
    self.assertEqual( status, fmippim.fmiOK )
    
    t = 0.
    stepsize = 1.
    tstop = 10.
    x = 0.
    cycles = 0
    positive = False
    twopi = 6.28318530718
    
    status = fmu.initialize( t, True, tstop )
    self.assertEqual( status, fmippim.fmiOK )
    
    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      # Make co-simulation step.
      status = fmu.doStep( t, stepsize, True )
      self.assertEqual( status, fmippim.fmiOK )

      # Advance time.
      t += stepsize;
      self.assertTrue( math.fabs( t - fmu.getTime() ) < EPS_TIME )

      # Retrieve result.
      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

      cycles = fmu.getIntegerValue( 'cycles' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

      positive = fmu.getBooleanValue( 'positive' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

      self.assertTrue( math.fabs( x - math.sin( omega*t ) ) < 1e-9 )
      self.assertEqual( cycles, math.floor( omega*t/twopi ) )
      self.assertEqual( positive, True if ( x > 0.0 ) else False )
	
    self.assertTrue( math.fabs( tstop - fmu.getTime() ) < EPS_TIME )
	
	
  def test_fmi_1_0_run_simulation_start_time_not_zero(self):
    import math
	
    model_name = 'sine_standalone';
    fmu = fmippim.FMUCoSimulation( FMU_URI_PRE + model_name, model_name )
    status = fmu.instantiate( "sine_standalone1", 0., False, False )
    self.assertEqual( status, fmippim.fmiOK )
    
    omega = 0.628318531
    status = fmu.setRealValue( 'omega', omega )
    self.assertEqual( status, fmippim.fmiOK )
    
    tstart = 5.
    t = tstart
    stepsize = 1.
    tstop = 10.
    x = 0.
    cycles = 0
    positive = False
    twopi = 6.28318530718
    
    status = fmu.initialize( tstart, True, tstop )
    self.assertEqual( status, fmippim.fmiOK )
    
    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      # Make co-simulation step.
      status = fmu.doStep( t, stepsize, True )
      self.assertEqual( status, fmippim.fmiOK )

      # Advance time.
      t += stepsize;
      self.assertTrue( math.fabs( t - fmu.getTime() ) < EPS_TIME )

      # Retrieve result.
      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

      cycles = fmu.getIntegerValue( 'cycles' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

      positive = fmu.getBooleanValue( 'positive' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

      self.assertTrue( math.fabs( x - math.sin( omega*t ) ) < 1e-9 )
      self.assertEqual( cycles, math.floor( omega*t/twopi ) )
      self.assertEqual( positive, True if ( x > 0.0 ) else False )
	
    self.assertTrue( math.fabs( tstop - fmu.getTime() ) < EPS_TIME )


if __name__ == '__main__':
  import sys
  global EPS_TIME
  EPS_TIME = float( sys.argv.pop() )
  global FMU_URI_PRE
  FMU_URI_PRE = sys.argv.pop()
  unittest.main()
