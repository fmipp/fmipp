# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

import fmippim
import unittest

class testFMUModelExchange(unittest.TestCase):
  def test_fmu_load(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
  
  def test_fmu_instantiate(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )

  def test_fmu_initialize(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmiOK )


  def test_fmu_getvalue(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmiOK )
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )
    self.assertEqual( x, 0.0 )

  def test_fmu_setvalue(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.setRealValue( "x0", 0.5 )
    self.assertEqual( status, fmippim.fmiOK )
    x0 = fmu.getRealValue( "x0" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )
    self.assertEqual( x0, 0.5 )

  def test_fmu_run_simulation_1(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.setRealValue( "k", 1.0 )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )
    
    t = 0.0
    stepsize = 0.0025
    tstop = 1.0

    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + stepsize )
      x = fmu.getRealValue( "x" )

    t = fmu.getTime()
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );
    self.assertTrue( abs( x - 1.0 ) < 1e-6 );

  def test_fmu_run_simulation_1_stop_before_event(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", True, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.setRealValue( "k", 1.0 )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0
    stepsize = 0.0025
    tstop = 1.0

    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + stepsize )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    t = fmu.getTime()
    self.assertTrue( abs( t - tstop ) < stepsize/2 )
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )
    self.assertTrue( abs( x - 1.0 ) < 1e-6 );

  def test_fmu_run_simulation_2(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.setRealValue( "k", 10.0 )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0
    stepsize = 0.025
    tstop = 1.0
    eventctr = 0
    
    while ( t < tstop ):
      t = fmu.integrate( min( t + stepsize, tstop ) )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );
      dx = fmu.getRealValue( "der(x)" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );

      if ( fmu.getEventFlag() ):
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 5 );
    t = fmu.getTime();
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmu_run_simulation_2_encore(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.setRealValue( "k", 10.0 )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0
    stepsize = 0.025
    tstop = 1.0
    eventctr = 0

    while ( t < tstop ):
      t = fmu.integrate( min( t + stepsize, tstop ) )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );
      dx = fmu.getRealValue( "der(x)" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );
      
      if ( fmu.getEventFlag() ):
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 5 );
    t = fmu.getTime();
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmu_run_simulation_2_stop_before_event(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.setRealValue( "k", 10.0 )
    self.assertEqual( status, fmippim.fmiOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0
    stepsize = 0.025
    tstop = 1.0
    eventctr = 0

    while ( t < tstop ):
      t = fmu.integrate( min( t + stepsize, tstop ) )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );
      dx = fmu.getRealValue( "der(x)" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );
      
      if ( fmu.getEventFlag() ):
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 5 );
    t = fmu.getTime();
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK );
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmu_find_event(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "zigzag", "zigzag", False, EPS_TIME )
    status = fmu.instantiate( "zigzag1", False )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.setRealValue( "k", 2.0 )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0
    stepsize = 0.0025
    tstop = 1.0
    eventctr = 0

    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + stepsize )
      if ( fmu.getEventFlag() ):
        self.assertTrue( abs( t - 0.5 ) < 0.0025 )
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 1 )
    t = fmu.getTime()
    self.assertTrue( abs( t - tstop ) < stepsize/2 )
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmu_find_time_event(self):
    fmu = fmippim.FMUModelExchange( FMU_URI_PRE + "step_t0", "step_t0", False, EPS_TIME )
    status = fmu.instantiate( "step_t01", False )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.setRealValue( "t0", 0.5 )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0
    stepsize = 0.0025
    tstop = 1.0

    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + stepsize )
      x = fmu.getRealValue( "x" )
      self.assertEqual( status, fmippim.fmiOK )
      if ( t < 0.5 ):
        self.assertEqual( x, 0.0 )
      else:
        self.assertEqual( x, 1.0 )

if __name__ == '__main__':
  import sys
  global EPS_TIME
  EPS_TIME = float( sys.argv.pop() )
  global FMU_URI_PRE
  FMU_URI_PRE = sys.argv.pop()
  unittest.main()
