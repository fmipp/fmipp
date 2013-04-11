## To run this test script:
#   - Generate an FMU from model "Simple.mo".
#   - Make sure that the paths to all necessary libraries
#     are included in environment variable PATH

import pyfmi

model = pyfmi.load_fmu( "Simple.fmu" )
model.initialize()

for t in range( 1, 10 ):
    model.integrate( t, 0.01 )
    print "time = " + str( t ) + "  -  x = " + str( model.get( "x" ) )
