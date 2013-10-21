import fmipp

fmu = fmipp.PyIncrementalFMU( 'file:///home/user/fmipp/examples/fmu/simple/Simple', 'Simple' )

realOutputNames = [ 'x', 'z' ]
realInitialInputNames = [ 'p', 'x' ]
realInitialValues = [ 0.2, 0.5 ]

startTime = 0.
lookAheadHorizon = 1.
lookAheadStepSize = 0.2
integratorStepSize = 0.05

fmu.defineRealOutputs( realOutputNames )

fmu.init( "Simple1", realInitialInputNames, realInitialValues,
          startTime, lookAheadHorizon, lookAheadStepSize, integratorStepSize )


fmu.sync( 0, 0 )

for t in range( 0, 5 ):
    fmu.sync( t/10, (t+1)/10 )
    out = fmu.getRealOutputs()
    print( realOutputNames[0], out[0], realOutputNames[1], out[1] )
