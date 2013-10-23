#!/usr/bin/env python

"""
Module containing the FMI++ interface Python wrappers.
"""

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector

# Define size_t.
ctypedef unsigned int size_t

# Define FMI data types.
ctypedef unsigned int fmiValueReference
ctypedef double fmiReal
ctypedef int fmiInteger
ctypedef char fmiBoolean
ctypedef double fmiTime

# Define struct fmiStatus.
ctypedef enum fmiStatus:
    fmiOK = 0
    fmiWarning = 1
    fmiDiscard = 2
    fmiError = 3
    fmiFatal = 4
    fmiPending = 5

# Define NAN.
cdef fmiReal cppRealNAN = <fmiReal>float("NaN")
cdef fmiInteger cppIntegerNAN = <fmiInteger>float("NaN")

#
# Import C++ definition of class FMU.
#
cdef extern from "FMU.h":

    # Enum reflecting the basic FMI types.
    ctypedef enum FMIType:
        fmiTypeReal = 0
        fmiTypeInteger = 1
        fmiTypeBoolean = 2
        fmiTypeString = 3
        fmiTypeUnknown = 4


    cdef cppclass FMU:

        # Constructor.
        FMU( string, string ) except +

        # Get variable type.
        FMIType getType( string )

        # Instantiate the FMU.
        fmiStatus instantiate( string, fmiBoolean )

        # Initialize the FMU.
        fmiStatus initialize()

        # Set a value (using the value's name).
        fmiStatus setValue( string, fmiReal )

        # Set a value (using the value's name).
        fmiStatus setValue( string, fmiInteger )

        # Get a value (using the value's name).
        fmiStatus getValue( string, fmiReal )

        # Get a value (using the value's name).
        fmiStatus getValue( string, fmiInteger )

        # Integrate.
        fmiStatus integrate( fmiReal, double )


#
# Python wrapper for class FMU.  
#
cdef class PyFMU:

    cdef FMU* thisptr_ # hold pointer to wrapped C++ instance

    def __cinit__( self, fmuPath, modelName ):
        cdef string cppFmuPath = fmuPath.encode( 'UTF-8' )
        cdef string cppModelName = modelName.encode( 'UTF-8' )
        self.thisptr_ = new FMU( cppFmuPath, cppModelName )

    def getType( self, name ):
        cdef string cppName = name.encode( 'UTF-8' )
        return self.thisptr_.getType( cppName )

    def instantiate( self, instanceName, loggingOn = False ):
        cdef string cppInstanceName = instanceName.encode( 'UTF-8' )
        cdef bool cppLoggingOn = loggingOn
        self.thisptr_.instantiate( cppInstanceName, cppLoggingOn )

    def initialize( self ):
        self.thisptr_.initialize()

    def setRealValue( self, name, value ):
        cdef string cppName = name.encode( 'UTF-8' )
        self.thisptr_.setValue( cppName, <fmiReal>value )

    def setIntegerValue( self, name, value ):
        cdef string cppName = name.encode( 'UTF-8' )
        self.thisptr_.setValue( cppName, <fmiInteger>value )

    def setValue( self, name, value ):
        if 0 == self.getType( name ): # fmiTypeReal = 0
            self.setRealValue( name, value )
        elif 1 == self.getType( name ): # fmiTypeInteger = 1
            self.setIntegerValue( name, value )
        else:
            raise TypeError( 'Support currently only for "int" and "float" inputs.' )

    def getRealValue( self, name ):
        cdef string cppName = name.encode( 'UTF-8' )
        cdef fmiReal cppValue = cppRealNAN
        self.thisptr_.getValue( cppName, cppValue )
        return cppValue

    def getIntegerValue( self, name ):
        cdef string cppName = name.encode( 'UTF-8' )
        cdef fmiInteger cppValue = cppIntegerNAN
        self.thisptr_.getValue( cppName, cppValue )
        return cppValue

    def getValue( self, name ):
        if 0 == self.getType( name ): # fmiTypeReal = 0
            return self.getRealValue( name )
        elif 1 == self.getType( name ): # fmiTypeInteger = 1
            return self.getIntegerValue( name )
        else:
            raise TypeError( 'Support currently only for "int" and "float" inputs.' )

    def integrate( self, tend, delta = 1e-5 ):
        # cdef fmiReal cppTend = tend
        # cdef double cppDelta = delta
        # self.thisptr_.integrate( cppTend, cppDelta )
        self.thisptr_.integrate( tend, delta )


#
# Import C++ definition of class IncrementalFMU.
#
cdef extern from "IncrementalFMU.h":

    cdef cppclass IncrementalFMU:

        # Constructor.
        IncrementalFMU( string, string ) except +

        # Define real inputs of the FMU (call before initialization).
        void defineRealInputs( string*, size_t )

        # Define real outputs of the FMU (call before initialization).
        void defineRealOutputs( string*, size_t )

        # Initialize the FMU (short version for real initial inputs only).
        int init( string, string*, fmiReal*, size_t,
                  fmiTime, fmiTime, fmiTime, fmiTime )

        # Simulate FMU from time t0 until t1.
        fmiTime sync( fmiTime, fmiTime )

        # Simulate FMU from time t0 until t1, take new inputs into account.
        fmiTime sync( fmiTime, fmiTime, fmiReal*,
                      fmiInteger*, fmiBoolean*, string* )

        # Get current real outputs.
        fmiReal* getRealOutputs()


#
# Python wrapper for class IncrementalFMU.
#
cdef class PyIncrementalFMU:

    cdef IncrementalFMU* thisptr_ # hold pointer to wrapped C++ instance

    cdef size_t nRealInputs_
    cdef vector[double] realInputs_

    cdef size_t nRealOutputs_
    cdef vector[double] realOutputs_

    def __cinit__( self, fmuPath, modelName ):
        cdef string cppFmuPath = fmuPath.encode( 'UTF-8' )
        cdef string cppModelName = modelName.encode( 'UTF-8' )
        self.thisptr_ = new IncrementalFMU( cppFmuPath, cppModelName )

    def defineRealInputs( self, realInputNames ):
        self.nRealInputs_ = len( realInputNames )
        for i in range( 0, self.nRealInputs_ ):
            self.realInputs_.push_back( 0. )
        cdef vector[string] cppRealInputNames
        for name in realInputNames:
            cppRealInputNames.push_back( name.encode( 'UTF-8' ) )
        self.thisptr_.defineRealInputs( &cppRealInputNames[0], self.nRealInputs_ )

    def defineRealOutputs( self, realOutputNames ):
        self.nRealOutputs_ = len( realOutputNames )
        for i in range( 0, self.nRealOutputs_ ):
            self.realOutputs_.push_back( 0. )
        cdef vector[string] cppRealOutputNames
        for name in realOutputNames:
            cppRealOutputNames.push_back( name.encode( 'UTF-8' ) )
        self.thisptr_.defineRealOutputs( &cppRealOutputNames[0], self.nRealOutputs_ )

    def init( self, instanceName, realVariableNames, realInitialValues,
              startTime, lookAheadHorizon, lookAheadStepSize, integratorStepSize ):
        cdef string cppInstanceName = instanceName.encode( 'UTF-8' )
        cdef int nRealInitialValues = len( realVariableNames )
        cdef vector[double] cppRealInitialValues = realInitialValues
        cdef vector[string] cppRealVariableNames
        for name in realVariableNames:
            cppRealVariableNames.push_back( name.encode( 'UTF-8' ) ) # Fill explicitely using encode(...).
        self.thisptr_.init( cppInstanceName,
                            &cppRealVariableNames[0], &cppRealInitialValues[0], nRealInitialValues,
                            startTime, lookAheadHorizon, lookAheadStepSize, integratorStepSize )

    def sync( self, t0, t1 ):
        return self.thisptr_.sync( t0, t1 )

    def sync( self, t0, t1, realInputs ):
        self.realInputs_ = realInputs
        return self.thisptr_.sync( t0, t1, &self.realInputs_[0], NULL, NULL, NULL )

    def getRealOutputs( self ):
        cdef const fmiReal* realOutputs = self.thisptr_.getRealOutputs()
        for i in range( 0, self.nRealOutputs_ ):
            self.realOutputs_[i] = realOutputs[i]
        return self.realOutputs_
