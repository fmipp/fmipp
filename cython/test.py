import fmipp

def testWrapper_FMU( fmuFileName, modelName ):
    fmu = fmipp.PyFMU( fmuFileName, modelName )

    fmu.instantiate( modelName + "_1", False )
    fmu.initialize()

    realOutputNames = [ 'x', 'z' ]
    realOutputs = [ 0., 0. ]

    fmu.setRealValue( 'p', 0.2 )

    for t in range( 0, 5 ):
        fmu.integrate( t/10 )
        realOutputs[0] = fmu.getRealValue( realOutputNames[0] )
        realOutputs[1] = fmu.getRealValue( realOutputNames[1] )
        print( realOutputNames[0], "=", realOutputs[0], " - ",
               realOutputNames[1], "=", realOutputs[1] )


def testWrapper_IncrementalFMU( fmuFileName, modelName ):
    fmu = fmipp.PyIncrementalFMU( fmuFileName, modelName )

    realOutputNames = [ 'x', 'z' ]
    realInitialInputNames = [ 'p', 'x' ]
    realInitialValues = [ 0.2, 0.5 ]

    startTime = 0.
    lookAheadHorizon = 1.
    lookAheadStepSize = 0.05
    integratorStepSize = 0.001

    fmu.defineRealOutputs( realOutputNames )

    fmu.init( modelName + "_1", realInitialInputNames, realInitialValues,
              startTime, lookAheadHorizon, lookAheadStepSize, integratorStepSize )


    fmu.sync( 0, 0 )

    for t in range( 0, 5 ):
        fmu.sync( t/10, (t+1)/10 )
        realOutputs = fmu.getRealOutputs()
        print( realOutputNames[0], "=", realOutputs[0], " - ",
               realOutputNames[1], "=", realOutputs[1] )


if __name__ == '__main__':

    print( '### Test wrapper for class FMU ###' )
    testWrapper_FMU( 'file:///home/user/fmipp/examples/fmu/simple/Simple', 'Simple' )

    print( '### Test wrapper for class IncrementalFMU ###' )
    testWrapper_IncrementalFMU( 'file:///home/user/fmipp/examples/fmu/simple/Simple', 'Simple' )
