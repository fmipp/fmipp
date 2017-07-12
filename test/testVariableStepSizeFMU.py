# -------------------------------------------------------------------
# Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

import fmippim
import unittest
import math

class testVariableStepSizeFMU(unittest.TestCase):
  def test_fmi_1_0_load(self):
    model_name = 'sine_standalone'
    fmu = fmippim.VariableStepSizeFMU( FMU_URI_PRE + model_name, model_name )


  def test_fmi_1_0_init(self):
    model_name = 'sine_standalone';
    fmu = fmippim.VariableStepSizeFMU( FMU_URI_PRE + model_name, model_name )

    # construct string array for init parameter names
    init_vars = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( init_vars, 0, 'omega' )

    # construct double array for init parameter values
    init_vals = fmippim.new_double_array( 1 )
    fmippim.double_array_setitem( init_vals, 0, 0.1 * math.pi )

    start_time = 0.
    step_size = 1. # NB: fixed step size enforced by FMU!

    status = fmu.init( "test_sine", init_vars, init_vals, 1, start_time, step_size )
    self.assertEqual( status, 1 )


  def test_fmi_1_0_getrealoutputs(self):
    model_name = 'sine_standalone';
    fmu = fmippim.VariableStepSizeFMU( FMU_URI_PRE + model_name, model_name )

    # construct string array for init parameter names
    init_vars = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( init_vars, 0, 'omega' )

    # construct double array for init parameter values
    init_vals = fmippim.new_double_array( 1 )
    fmippim.double_array_setitem( init_vals, 0, 0.1 * math.pi )

    # construct string array with output names
    outputs = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( outputs, 0, 'x' )

    # define real output names
    fmu.defineRealOutputs( outputs, 1 );

    start_time = 0.
    step_size = 1. # NB: fixed step size enforced by FMU!

    status = fmu.init( "test_sine", init_vars, init_vals, 1, start_time, step_size )
    self.assertEqual( status, 1 )

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value


  def test_fmi_1_0_runsimulation(self):
    model_name = 'sine_standalone';
    fmu = fmippim.VariableStepSizeFMU( FMU_URI_PRE + model_name, model_name )

    # construct string array for init parameter names
    init_vars = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( init_vars, 0, 'omega' )

    # construct double array for init parameter values
    init_vals = fmippim.new_double_array( 1 )
    fmippim.double_array_setitem( init_vals, 0, 0.1 * math.pi )

    # construct string array with output names
    outputs = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( outputs, 0, 'x' )

    # define real output names
    fmu.defineRealOutputs( outputs, 1 );

    start_time = 0.
    stop_time = 5.
    fmu_step_size = 1. # NB: fixed step size enforced by FMU!
    time = start_time

    status = fmu.init( "test_sine", init_vars, init_vals, 1, start_time, 2 * fmu_step_size )
    self.assertEqual( status, 1 )

    while ( time <= stop_time ):
      fmu.sync( time, time + fmu_step_size )
      time += fmu_step_size

      result = fmu.getRealOutputs()
      reference = math.sin( 0.1 * math.pi * fmu_step_size * math.floor( time / fmu_step_size ) )
      self.assertTrue( math.fabs( fmippim.double_array_getitem( result, 0 ) - reference ) < 1e-8 ) # check value



if __name__ == '__main__':
  import sys
  global EPS_TIME
  EPS_TIME = float( sys.argv.pop() )
  global FMU_URI_PRE
  FMU_URI_PRE = sys.argv.pop()
  unittest.main()
