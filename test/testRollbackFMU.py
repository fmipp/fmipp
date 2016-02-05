# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

import fmippim
import unittest

class testRollbackFMU(unittest.TestCase):
  def test_fmi_1_0_load(self):
    model_name = 'zigzag'
    fmu = fmippim.RollbackFMU( FMU_URI_PRE + model_name, model_name )


  def test_fmi_1_0_run_simulation_without_rollback(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.RollbackFMU( FMU_URI_PRE + model_name, model_name)

    status = fmu.instantiate( 'zigzag1' )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.setRealValue( 'k', 1.0 )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0 
    step_size = 0.0025
    tstop = 1.0
    x = -1.0

    while ( ( t + step_size ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + step_size )
      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    t = fmu.getTime();
    self.assertTrue( math.fabs( t - tstop ) < step_size/2 )

    x = fmu.getRealValue( 'x' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    # with an eventsearchprecision of 1.0e-4, require the same accuracy for x.
    self.assertTrue( math.fabs( x - 1.0 ) < 1e-4 )


  def test_fmi_1_0_run_simulation_with_rollback_1(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.RollbackFMU( FMU_URI_PRE + model_name, model_name)

    status = fmu.instantiate( 'zigzag1' )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.setRealValue( 'k', 1.0 )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0 
    step_size = 0.025
    tstop = 0.5
    x = -1.0

    while ( ( t + step_size ) - tstop < EPS_TIME ):
      # Make integration step.
      fmu.integrate( t + step_size )

      # Enforce rollback.
      fmu.integrate( t + 0.5*step_size )
      t = fmu.integrate( t + step_size )

      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    t = fmu.getTime();
    self.assertTrue( math.fabs( t - tstop ) < step_size/2 )

    x = fmu.getRealValue( 'x' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    # with an eventsearchprecision of 1.0e-6, require the same accuracy for x.
    self.assertTrue( math.fabs( x - 0.5 ) < 1e-6 )


  def test_fmi_1_0_run_simulation_with_rollback_2(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.RollbackFMU( FMU_URI_PRE + model_name, model_name)

    status = fmu.instantiate( 'zigzag1' )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.setRealValue( 'k', 1.0 )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0 
    step_size = 0.0025
    tstop = 0.5
    x = -1.0

    # Save initial state as rollback state.
    fmu.saveCurrentStateForRollback()
	
    while ( ( t + step_size ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + step_size )
      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    t = fmu.getTime();
    self.assertTrue( math.fabs( t - tstop ) < step_size/2 )

    x = fmu.getRealValue( 'x' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    # with an eventsearchprecision of 1.0e-6, require the same accuracy for x.
    self.assertTrue( math.fabs( x - 0.5 ) < 1e-6 )
	
    # Enforce rollback.
    t = 0.0

    while ( ( t + step_size ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + step_size )
      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    t = fmu.getTime();
    self.assertTrue( math.fabs( t - tstop ) < step_size/2 )

    x = fmu.getRealValue( 'x' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    # with an eventsearchprecision of 1.0e-6, require the same accuracy for x.
    self.assertTrue( math.fabs( x - 0.5 ) < 1e-6 )


  def test_fmi_2_0_load(self):
    model_name = 'zigzag2'
    fmu = fmippim.RollbackFMU( FMU_URI_PRE + model_name, model_name )


  def test_fmi_2_0_run_simulation_without_rollback(self):
    import math

    model_name = 'zigzag2'
    fmu = fmippim.RollbackFMU( FMU_URI_PRE + model_name, model_name)

    status = fmu.instantiate( 'zigzag1' )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.setRealValue( 'k', 1.0 )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0 
    step_size = 0.0025
    tstop = 1.0
    x = -1.0

    while ( ( t + step_size ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + step_size )
      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    t = fmu.getTime();
    self.assertTrue( math.fabs( t - tstop ) < step_size/2 )

    x = fmu.getRealValue( 'x' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    # with an eventsearchprecision of 1.0e-4, require the same accuracy for x.
    self.assertTrue( math.fabs( x - 1.0 ) < 1e-4 )


  def test_fmi_2_0_run_simulation_with_rollback_1(self):
    import math

    model_name = 'zigzag2'
    fmu = fmippim.RollbackFMU( FMU_URI_PRE + model_name, model_name)

    status = fmu.instantiate( 'zigzag1' )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.setRealValue( 'k', 1.0 )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0 
    step_size = 0.025
    tstop = 0.5
    x = -1.0

    while ( ( t + step_size ) - tstop < EPS_TIME ):
      # Make integration step.
      fmu.integrate( t + step_size )

      # Enforce rollback.
      fmu.integrate( t + 0.5*step_size )
      t = fmu.integrate( t + step_size )

      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    t = fmu.getTime();
    self.assertTrue( math.fabs( t - tstop ) < step_size/2 )

    x = fmu.getRealValue( 'x' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    # with an eventsearchprecision of 1.0e-6, require the same accuracy for x.
    self.assertTrue( math.fabs( x - 0.5 ) < 1e-6 )


  def test_fmi_2_0_run_simulation_with_rollback_2(self):
    import math

    model_name = 'zigzag2'
    fmu = fmippim.RollbackFMU( FMU_URI_PRE + model_name, model_name)

    status = fmu.instantiate( 'zigzag1' )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.setRealValue( 'k', 1.0 )
    self.assertEqual( status, fmippim.fmiOK )

    status = fmu.initialize()
    self.assertEqual( status, fmippim.fmiOK )

    t = 0.0 
    step_size = 0.0025
    tstop = 0.5
    x = -1.0

    # Save initial state as rollback state.
    fmu.saveCurrentStateForRollback()
	
    while ( ( t + step_size ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + step_size )
      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    t = fmu.getTime();
    self.assertTrue( math.fabs( t - tstop ) < step_size/2 )

    x = fmu.getRealValue( 'x' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    # with an eventsearchprecision of 1.0e-6, require the same accuracy for x.
    self.assertTrue( math.fabs( x - 0.5 ) < 1e-6 )
	
    # Enforce rollback.
    t = 0.0

    while ( ( t + step_size ) - tstop < EPS_TIME ):
      t = fmu.integrate( t + step_size )
      x = fmu.getRealValue( 'x' )
      self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    t = fmu.getTime();
    self.assertTrue( math.fabs( t - tstop ) < step_size/2 )

    x = fmu.getRealValue( 'x' )
    self.assertEqual( fmu.getLastStatus(), fmippim.fmiOK )

    # with an eventsearchprecision of 1.0e-6, require the same accuracy for x.
    self.assertTrue( math.fabs( x - 0.5 ) < 1e-6 )



if __name__ == '__main__':
  import sys
  global EPS_TIME
  EPS_TIME = float( sys.argv.pop() )
  global FMU_URI_PRE
  FMU_URI_PRE = sys.argv.pop()
  unittest.main()
