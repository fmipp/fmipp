## To run this test script:
#   - Generate an FMU from model "Simple.mo" and unzip into
#     a subdirectory called "Simple".
#   - Make sure that the paths to all necessary libraries
#     are included in environment variable PATH

import fmipp

model = fmipp.PyFMU( "Simple" )
model.initialize()

for t in range( 1, 10 ):
    model.integrate( t, 0.01 )
#    print "time = " + str( t ) + "  -  x = " + str( model.get( "x" ) ) # TODO: implement "get()"
