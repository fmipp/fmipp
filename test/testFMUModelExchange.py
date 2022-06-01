# -------------------------------------------------------------------
# Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

# Windows: add path to directory containing Sundials DLLs
import os
if hasattr( os, 'add_dll_directory' ):
  os.add_dll_directory( os.getenv( 'SUNDIALS_LIBRARYDIR' ) )

import fmippim
import unittest

class testFMUModelExchange(unittest.TestCase):
  def test_fmi_1_0_load(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
  
  def test_fmi_1_0_instantiate(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )

  def test_fmi_1_0_initialize(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )

  def test_fmi_1_0_initialize_with_resolution(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize( True, 1e-7 );
    self.assertEqual( status, fmippim.fmippOK )

  def test_fmi_1_0_getvalue(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )
    self.assertEqual( x, 0.0 )

  def test_fmi_1_0_setvalue(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "x0", 0.5 )
    self.assertEqual( status, fmippim.fmippOK )
    x0 = fmu.getRealValue( "x0" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )
    self.assertEqual( x0, 0.5 )

  def test_fmi_1_0_run_simulation_1(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 1.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )
    
    t = 0.0
    stepsize = 0.0025
    tstop = 1.0

    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + stepsize )
      x = fmu.getRealValue( "x" )

    t = fmu.getTime()
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
    self.assertTrue( abs( x - 1.0 ) < 1e-6 );

  def test_fmi_1_0_run_simulation_1_stop_before_event(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, True, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 1.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

    t = 0.0
    stepsize = 0.0025
    tstop = 1.0

    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + stepsize )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )

    t = fmu.getTime()
    self.assertTrue( abs( t - tstop ) < stepsize/2 )
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )
    self.assertTrue( abs( x - 1.0 ) < 1e-6 );

  def test_fmi_1_0_run_simulation_2(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 10.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

    t = 0.0
    stepsize = 0.025
    tstop = 1.0
    eventctr = 0
    
    while ( t < tstop ):
      t = fmu.integrate( min( t + stepsize, tstop ) )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      dx = fmu.getRealValue( "der(x)" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );

      if ( fmu.getEventFlag() ):
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 5 );
    t = fmu.getTime();
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmi_1_0_run_simulation_2_encore(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 10.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

    t = 0.0
    stepsize = 0.025
    tstop = 1.0
    eventctr = 0

    while ( t < tstop ):
      t = fmu.integrate( min( t + stepsize, tstop ) )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      dx = fmu.getRealValue( "der(x)" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      
      if ( fmu.getEventFlag() ):
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 5 );
    t = fmu.getTime();
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmi_1_0_run_simulation_2_stop_before_event(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 10.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

    t = 0.0
    stepsize = 0.025
    tstop = 1.0
    eventctr = 0

    while ( t < tstop ):
      t = fmu.integrate( min( t + stepsize, tstop ) )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      dx = fmu.getRealValue( "der(x)" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      
      if ( fmu.getEventFlag() ):
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 5 );
    t = fmu.getTime();
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmi_1_0_find_event(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "zigzag", "zigzag", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )

    status = fmu.setRealValue( "k", 2.0 )
    self.assertEqual( status, fmippim.fmippOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

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
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmi_1_0_find_time_event(self):
    fmu = fmippim.FMUModelExchangeV1( FMU_URI_PRE + "step_t0", "step_t0", False, False, EPS_TIME )
    status = fmu.instantiate( "step_t01" )
    self.assertEqual( status, fmippim.fmippOK )

    status = fmu.setRealValue( "t0", 0.5 )
    self.assertEqual( status, fmippim.fmippOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

    t = 0.0
    stepsize = 0.0025
    tstop = 1.0

    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + stepsize )
      x = fmu.getRealValue( "x" )
      self.assertEqual( status, fmippim.fmippOK )
      if ( t < 0.5 ):
        self.assertEqual( x, 0.0 )
      else:
        self.assertEqual( x, 1.0 )

  def test_fmi_2_0_load(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
  
  def test_fmi_2_0_instantiate(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )

  def test_fmi_2_0_initialize(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )

  def test_fmi_2_0_initialize_with_resolution(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize( True, 1e-7 );
    self.assertEqual( status, fmippim.fmippOK )

  def test_fmi_2_0_getvalue(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )
    self.assertEqual( x, 0.0 )

  def test_fmi_2_0_setvalue(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "x0", 0.5 )
    self.assertEqual( status, fmippim.fmippOK )
    x0 = fmu.getRealValue( "x0" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )
    self.assertEqual( x0, 0.5 )

  def test_fmi_2_0_getstatesrefs(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )
    states_refs = fmu.getStatesRefs()
    n_states = fmu.nStates()
    self.assertEqual( len(states_refs), n_states )
    self.assertEqual( states_refs[0], 0 )

  def test_fmi_2_0_getstatesnames(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )
    states_names = fmu.getStatesNames()
    n_states = fmu.nStates()
    self.assertEqual( len(states_names), n_states )
    self.assertEqual( states_names[0], 'x' )

  def test_fmi_2_0_getderivativesrefs(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )
    der_refs = fmu.getDerivativesRefs()
    n_states = fmu.nStates()
    self.assertEqual( len(der_refs), n_states )
    self.assertEqual( der_refs[0], 1 )

  def test_fmi_2_0_getderivativesnames(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize();
    self.assertEqual( status, fmippim.fmippOK )
    der_names = fmu.getDerivativesNames()
    n_states = fmu.nStates()
    self.assertEqual( len(der_names), n_states )
    self.assertEqual( der_names[0], 'der(x)' )

  def test_fmi_2_0_run_simulation_1(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 1.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )
    
    t = 0.0
    stepsize = 0.0025
    tstop = 1.0

    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + stepsize )
      x = fmu.getRealValue( "x" )

    t = fmu.getTime()
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
    self.assertTrue( abs( x - 1.0 ) < 1e-6 );

  def test_fmi_2_0_run_simulation_1_stop_before_event(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, True, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 1.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

    t = 0.0
    stepsize = 0.0025
    tstop = 1.0

    while ( ( t + stepsize ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + stepsize )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )

    t = fmu.getTime()
    self.assertTrue( abs( t - tstop ) < stepsize/2 )
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )
    self.assertTrue( abs( x - 1.0 ) < 1e-6 );

  def test_fmi_2_0_run_simulation_2(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 10.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

    t = 0.0
    stepsize = 0.025
    tstop = 1.0
    eventctr = 0
    
    while ( t < tstop ):
      t = fmu.integrate( min( t + stepsize, tstop ) )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      dx = fmu.getRealValue( "der(x)" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );

      if ( fmu.getEventFlag() ):
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 5 );
    t = fmu.getTime();
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmi_2_0_run_simulation_2_encore(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 10.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

    t = 0.0
    stepsize = 0.025
    tstop = 1.0
    eventctr = 0

    while ( t < tstop ):
      t = fmu.integrate( min( t + stepsize, tstop ) )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      dx = fmu.getRealValue( "der(x)" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      
      if ( fmu.getEventFlag() ):
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 5 );
    t = fmu.getTime();
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmi_2_0_run_simulation_2_stop_before_event(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.setRealValue( "k", 10.0 )
    self.assertEqual( status, fmippim.fmippOK )
    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

    t = 0.0
    stepsize = 0.025
    tstop = 1.0
    eventctr = 0

    while ( t < tstop ):
      t = fmu.integrate( min( t + stepsize, tstop ) )
      x = fmu.getRealValue( "x" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      dx = fmu.getRealValue( "der(x)" )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
      
      if ( fmu.getEventFlag() ):
        eventctr = eventctr + 1
        fmu.setEventFlag( False )

    self.assertEqual( eventctr, 5 );
    t = fmu.getTime();
    self.assertTrue( abs( t - tstop ) < stepsize/2 );
    x = fmu.getRealValue( "x" )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK );
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );

  def test_fmi_2_0_find_event(self):
    fmu = fmippim.FMUModelExchangeV2( FMU_URI_PRE + "zigzag2", "zigzag2", False, False, EPS_TIME )
    status = fmu.instantiate( "zigzag1" )
    self.assertEqual( status, fmippim.fmippOK )

    status = fmu.setRealValue( "k", 2.0 )
    self.assertEqual( status, fmippim.fmippOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmippOK )

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
    self.assertEqual( fmu.getLastStatus(), fmippim.fmippOK )
    self.assertTrue( abs( x - 0.0 ) < 1e-6 );



if __name__ == '__main__':
  import sys
  global EPS_TIME
  EPS_TIME = float( sys.argv.pop() )
  global FMU_URI_PRE
  FMU_URI_PRE = sys.argv.pop()
  unittest.main()
