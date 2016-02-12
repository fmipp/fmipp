# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

import fmippim
import unittest

class testIncrementalFMU(unittest.TestCase):
  def test_fmi_1_0_load(self):
    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name )


  def test_fmi_1_0_init(self):
    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 10.0 )
    fmippim.double_array_setitem( vals, 1, 1.0 )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status


  def test_fmi_1_0_getrealoutputs(self):
    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 10.0 )
    fmippim.double_array_setitem( vals, 1, 1.0 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( outputs, 0, 'x' )
    fmippim.string_array_setitem( outputs, 1, 'der(x)' )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 2 )

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value
    self.assertEqual( fmippim.double_array_getitem( result, 1 ), 10.0 ) # check value


  def test_fmi_1_0_run_simulation_1(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 1.0 )
    fmippim.double_array_setitem( vals, 1, 0.0 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( outputs, 0, 'x' )
    fmippim.string_array_setitem( outputs, 1, 'der(x)' )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 2 )

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value
    self.assertEqual( fmippim.double_array_getitem( result, 1 ), 1.0 ) # check value

    time = start_time
    next = fmu.sync( -42.0, time )
    self.assertEqual( next, horizon )

    while ( time + step_size - 1.0  < EPS_TIME ):
        oldnext = next
        next = fmu.sync( time, min( time + step_size, next ) )
        result = fmu.getRealOutputs()
        time = min( time + step_size, oldnext )
        if ( math.fabs( time - 0.5 ) < 1e-6 ):
          x = fmippim.double_array_getitem( result, 0 )
          self.assertTrue( math.fabs( x - 0.5 ) < 1e-4 )

    self.assertTrue( math.fabs( time - 1.0 ) < step_size/2 )

    result = fmu.getRealOutputs()
    x = fmippim.double_array_getitem( result, 0 )
    self.assertTrue( math.fabs( x - 1.0 ) < 1e-4 )


  def test_fmi_1_0_run_simulation_2(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 10.0 )
    fmippim.double_array_setitem( vals, 1, 0.0 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( outputs, 0, 'x' )
    fmippim.string_array_setitem( outputs, 1, 'der(x)' )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 2 )

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value
    self.assertEqual( fmippim.double_array_getitem( result, 1 ), 10.0 ) # check value

    time = start_time
    next = fmu.sync( -42.0, time )
    self.assertEqual( next, horizon )

    dx = 10.0
    eventctr = 0
	
    while ( time - 1.0  < EPS_TIME ):
      oldnext = next
      next = fmu.sync( time, min( time + step_size, next ) )
      time = min( time + step_size, oldnext )
      result = fmu.getRealOutputs()
      k = fmippim.double_array_getitem( result, 1 )
      if ( k != dx ):
        eventctr += 1
        dx = k

    self.assertEqual( eventctr, 5 )

    self.assertTrue( math.fabs( time - 1.0 ) < step_size/2 )

    result = fmu.getRealOutputs()
    x = fmippim.double_array_getitem( result, 0 )
    self.assertTrue( math.fabs( x ) < 1e-6 )


  def test_fmi_1_0_check_sync_times(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 1.0 )
    fmippim.double_array_setitem( vals, 1, 0.0 )

    start_time = 0.0 
    stop_time = 4.0
    step_size = 0.3
    horizon = 2*step_size
    int_step_size = step_size/2

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    time = start_time
    next = start_time
    sync_times = []

    while ( time - stop_time  < EPS_TIME ):
      oldnext = next
      next = fmu.sync( time, min( time + step_size, next ) )
      time = min( time + step_size, oldnext )
      sync_times.append( time )

    expected_sync_times = [ 0., .3, .6, .9, 1., 1., 1.3, 1.6, 1.9, 2.2, 2.5, 2.8, 3., 3., 3.3, 3.6, 3.9, 4.2 ]
    self.assertEqual( len( expected_sync_times ), len( sync_times ) )

    for i in range( 0, len( sync_times ) ):
      self.assertTrue( math.fabs( sync_times[i] - expected_sync_times[i] ) < 1e-7 )


  def test_fmi_1_0_indicated_event_timing(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 1.0 )
    fmippim.double_array_setitem( vals, 1, 0.0 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( outputs, 0, 'x' )
    fmippim.string_array_setitem( outputs, 1, 'der(x)' )

    start_time = 0.0 
    step_size = 0.11
    horizon = 10*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 2 )

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value
    self.assertEqual( fmippim.double_array_getitem( result, 1 ), 1.0 ) # check value

    # get first event at t=1.0
    event_time = fmu.sync( -42.0, start_time )
    self.assertTrue( math.fabs( event_time - 1.0 ) < 1.0*100*EPS_TIME )

    # step to event
    next_time = fmu.sync( start_time, event_time )
    self.assertTrue( math.fabs( next_time - event_time ) < 2*2.1*100*EPS_TIME )

    # step over event and get end of horizon event at t=2.1
    next_time = fmu.sync( event_time, event_time )
    self.assertTrue( math.fabs( next_time - 2.1 ) < 2*2.1*100*EPS_TIME )


  def test_fmi_1_0_time_event(self):
    import math

    model_name = 'step_t0'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( vars, 0, 't0' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 1 )
    fmippim.double_array_setitem( vals, 0, 0.5 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( outputs, 0, 'x' )

    start_time = 0.0 
    step_size = 0.3
    horizon = 2*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 1 )

    status = fmu.init( 'step_t0', vars, vals, 1, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value

    time = fmu.sync( -4711.0, start_time )
    self.assertTrue( math.fabs( time - 0.5 ) < EPS_TIME )

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value

    # step to event
    time = fmu.sync( start_time, time )

    # step over event
    time = fmu.sync( time, time )
    self.assertTrue( math.fabs( time - ( 0.5 + horizon ) ) < EPS_TIME )

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value


  def test_fmi_1_0_updateStateFromTheRight(self):
    import math

    model_name = 'step_t0'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( vars, 0, 't0' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 1 )
    fmippim.double_array_setitem( vals, 0, 0.5 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( outputs, 0, 'x' )

    start_time = 0.0 
    step_size = 0.3
    horizon = 2*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 1 )

    status = fmu.init( 'step_t0', vars, vals, 1, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value

    time = fmu.predictState( start_time, start_time )
    self.assertTrue( math.fabs( time - 0.5 ) < EPS_TIME )

    time = fmu.updateStateFromTheRight( time )
    self.assertTrue( math.fabs( time - 0.5 ) < 2*EPS_TIME )

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 1.0 ) # check value


  def test_fmi_1_0_init_error_handling_real(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 3 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )
    fmippim.string_array_setitem( vars, 2, 'ERR' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 3 )
    fmippim.double_array_setitem( vals, 0, 10.0 )
    fmippim.double_array_setitem( vals, 1, 0.0 )
    fmippim.double_array_setitem( vals, 2, 0.0 )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    status = fmu.init( 'zigzag1', vars, vals, 3, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 0 ) # check status


  def test_fmi_1_0_init_error_handling_integer(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for real init parameter names
    varsReal = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( varsReal, 0, 'k' )
    fmippim.string_array_setitem( varsReal, 1, 'x' )

    # construct string array for real init parameter values
    valsReal = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( valsReal, 0, 10.0 )
    fmippim.double_array_setitem( valsReal, 1, 0.0 )

    # construct string array for integer init parameter names
    varsInt = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( varsInt, 0, 'ERR' )

    # construct string array for integer parameter values
    valsInt = fmippim.new_int_array( 1 )
    fmippim.int_array_setitem( valsInt, 0, 1 )

    # dummy boolean and string parameter names and values
    varsBool = fmippim.new_string_array( 0 )
    valsBool = fmippim.new_char_array( 0 )
    varsString = fmippim.new_string_array( 0 )
    valsString = fmippim.new_string_array( 0 )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    status = fmu.init( 'zigzag1', varsReal, valsReal, 2, varsInt, valsInt, 1, varsBool, valsBool, 0, varsString, valsString, 0, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 0 ) # check status


  def test_fmi_1_0_init_error_handling_boolean(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for real init parameter names
    varsReal = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( varsReal, 0, 'k' )
    fmippim.string_array_setitem( varsReal, 1, 'x' )

    # construct string array for real init parameter values
    valsReal = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( valsReal, 0, 10.0 )
    fmippim.double_array_setitem( valsReal, 1, 0.0 )

    # construct string array for boolean init parameter names
    varsBool = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( varsBool, 0, 'ERR' )

    # construct string array for boolean parameter values
    valsBool = fmippim.new_char_array( 1 )
    fmippim.char_array_setitem( valsBool, 0, '1' )

    # dummy integer and string parameter names and values
    varsInt = fmippim.new_string_array( 0 )
    valsInt = fmippim.new_int_array( 0 )
    varsString = fmippim.new_string_array( 0 )
    valsString = fmippim.new_string_array( 0 )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    status = fmu.init( 'zigzag1', varsReal, valsReal, 2, varsInt, valsInt, 0, varsBool, valsBool, 1, varsString, valsString, 0, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 0 ) # check status


  def test_fmi_1_0_init_error_handling_string(self):
    import math

    model_name = 'zigzag'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for real init parameter names
    varsReal = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( varsReal, 0, 'k' )
    fmippim.string_array_setitem( varsReal, 1, 'x' )

    # construct string array for real init parameter values
    valsReal = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( valsReal, 0, 10.0 )
    fmippim.double_array_setitem( valsReal, 1, 0.0 )

    # construct string array for string init parameter names
    varsString = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( varsString, 0, 'ERR_STRING' )

    # construct string array for string parameter values
    valsString = fmippim.new_string_array( 1 )
    fmippim.string_array_setitem( valsString, 0, 'Nope' )

    # dummy boolean and integer parameter names and values
    varsInt = fmippim.new_string_array( 0 )
    valsInt = fmippim.new_int_array( 0 )
    varsBool = fmippim.new_string_array( 0 )
    valsBool = fmippim.new_char_array( 0 )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    status = fmu.init( 'zigzag1', varsReal, valsReal, 2, varsInt, valsInt, 0, varsBool, valsBool, 0, varsString, valsString, 1, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 0 ) # check status


  def test_fmi_2_0_load(self):
    model_name = 'zigzag2'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name )


  def test_fmi_2_0_init(self):
    model_name = 'zigzag2'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 10.0 )
    fmippim.double_array_setitem( vals, 1, 1.0 )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status


  def test_fmi_2_0_getrealoutputs(self):
    model_name = 'zigzag2'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 10.0 )
    fmippim.double_array_setitem( vals, 1, 1.0 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( outputs, 0, 'x' )
    fmippim.string_array_setitem( outputs, 1, 'der(x)' )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 2 )

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value
    self.assertEqual( fmippim.double_array_getitem( result, 1 ), 10.0 ) # check value


  def test_fmi_2_0_run_simulation_1(self):
    import math

    model_name = 'zigzag2'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 1.0 )
    fmippim.double_array_setitem( vals, 1, 0.0 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( outputs, 0, 'x' )
    fmippim.string_array_setitem( outputs, 1, 'der(x)' )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 2 )

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value
    self.assertEqual( fmippim.double_array_getitem( result, 1 ), 1.0 ) # check value

    time = start_time
    next = fmu.sync( -42.0, time )
    self.assertEqual( next, horizon )

    while ( time + step_size - 1.0  < EPS_TIME ):
        oldnext = next
        next = fmu.sync( time, min( time + step_size, next ) )
        result = fmu.getRealOutputs()
        time = min( time + step_size, oldnext )
        if ( math.fabs( time - 0.5 ) < 1e-6 ):
          x = fmippim.double_array_getitem( result, 0 )
          self.assertTrue( math.fabs( x - 0.5 ) < 1e-4 )

    self.assertTrue( math.fabs( time - 1.0 ) < step_size/2 )

    result = fmu.getRealOutputs()
    x = fmippim.double_array_getitem( result, 0 )
    self.assertTrue( math.fabs( x - 1.0 ) < 1e-4 )


  def test_fmi_2_0_run_simulation_2(self):
    import math

    model_name = 'zigzag2'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 10.0 )
    fmippim.double_array_setitem( vals, 1, 0.0 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( outputs, 0, 'x' )
    fmippim.string_array_setitem( outputs, 1, 'der(x)' )

    start_time = 0.0 
    step_size = 0.0025
    horizon = 2*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 2 )

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value
    self.assertEqual( fmippim.double_array_getitem( result, 1 ), 10.0 ) # check value

    time = start_time
    next = fmu.sync( -42.0, time )
    self.assertEqual( next, horizon )

    dx = 10.0
    eventctr = 0
	
    while ( time - 1.0  < EPS_TIME ):
      oldnext = next
      next = fmu.sync( time, min( time + step_size, next ) )
      time = min( time + step_size, oldnext )
      result = fmu.getRealOutputs()
      k = fmippim.double_array_getitem( result, 1 )
      if ( k != dx ):
        eventctr += 1
        dx = k

    self.assertEqual( eventctr, 5 )

    self.assertTrue( math.fabs( time - 1.0 ) < step_size/2 )

    result = fmu.getRealOutputs()
    x = fmippim.double_array_getitem( result, 0 )
    self.assertTrue( math.fabs( x ) < 1e-6 )


  def test_fmi_2_0_indicated_event_timing(self):
    import math

    model_name = 'zigzag2'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 1.0 )
    fmippim.double_array_setitem( vals, 1, 0.0 )

    # construct string array with output names
    outputs = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( outputs, 0, 'x' )
    fmippim.string_array_setitem( outputs, 1, 'der(x)' )

    start_time = 0.0 
    step_size = 0.11
    horizon = 10*step_size
    int_step_size = step_size/2

    fmu.defineRealOutputs( outputs, 2 )

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    result = fmu.getRealOutputs()
    self.assertEqual( fmippim.double_array_getitem( result, 0 ), 0.0 ) # check value
    self.assertEqual( fmippim.double_array_getitem( result, 1 ), 1.0 ) # check value

    # get first event at t=1.0
    event_time = fmu.sync( -42.0, start_time )
    self.assertTrue( math.fabs( event_time - 1.0 ) < 1.0*100*EPS_TIME )

    # step to event
    next_time = fmu.sync( start_time, event_time )
    self.assertTrue( math.fabs( next_time - event_time ) < 2*2.1*100*EPS_TIME )

    # step over event and get end of horizon event at t=2.1
    next_time = fmu.sync( event_time, event_time )
    self.assertTrue( math.fabs( next_time - 2.1 ) < 2*2.1*100*EPS_TIME )


  def test_fmi_2_0_check_sync_times(self):
    import math

    model_name = 'zigzag2'
    fmu = fmippim.IncrementalFMU( FMU_URI_PRE + model_name, model_name, False, EPS_TIME )

    # construct string array for init parameter names
    vars = fmippim.new_string_array( 2 )
    fmippim.string_array_setitem( vars, 0, 'k' )
    fmippim.string_array_setitem( vars, 1, 'x' )

    # construct double array for init parameter values
    vals = fmippim.new_double_array( 2 )
    fmippim.double_array_setitem( vals, 0, 1.0 )
    fmippim.double_array_setitem( vals, 1, 0.0 )

    start_time = 0.0 
    stop_time = 4.0
    step_size = 0.3
    horizon = 2*step_size
    int_step_size = step_size/2

    status = fmu.init( 'zigzag1', vars, vals, 2, start_time, horizon, step_size, int_step_size ) # initialize model
    self.assertEqual( status, 1 ) # check status

    time = start_time
    next = start_time
    sync_times = []

    while ( time - stop_time  < EPS_TIME ):
      oldnext = next
      next = fmu.sync( time, min( time + step_size, next ) )
      time = min( time + step_size, oldnext )
      sync_times.append( time )

    expected_sync_times = [ 0., .3, .6, .9, 1., 1., 1.3, 1.6, 1.9, 2.2, 2.5, 2.8, 3., 3., 3.3, 3.6, 3.9, 4.2 ]
    self.assertEqual( len( expected_sync_times ), len( sync_times ) )

    for i in range( 0, len( sync_times ) ):
      self.assertTrue( math.fabs( sync_times[i] - expected_sync_times[i] ) < 1e-7 )



if __name__ == '__main__':
  import sys
  global EPS_TIME
  EPS_TIME = float( sys.argv.pop() )
  global FMU_URI_PRE
  FMU_URI_PRE = sys.argv.pop()
  unittest.main()
