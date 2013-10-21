#!/usr/bin/env python

"""
Module containing the FMI++ interface Python wrappers.
"""

from libcpp.string cimport string
from libcpp.vector cimport vector


cdef extern from "IncrementalFMU.h":

    ctypedef unsigned int size_t

    ctypedef unsigned int fmiValueReference
    ctypedef double fmiReal
    ctypedef int fmiInteger
    ctypedef char fmiBoolean
    ctypedef double fmiTime

    # ctypedef enum fmiStatus:
    #     fmiOK = 0
    #     fmiWarning = 1
    #     fmiDiscard = 2
    #     fmiError = 3
    #     fmiFatal = 4
    #     fmiPending = 5


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



cdef class PyIncrementalFMU:

    cdef IncrementalFMU* thisptr_   # hold a C++ instance which we're wrapping

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
            self.realInputs_.push_back( 0. );
        cdef vector[string] cppRealInputNames
        for name in realInputNames:
            cppRealInputNames.push_back( name.encode( 'UTF-8' ) )

        self.thisptr_.defineRealInputs( &cppRealInputNames[0], self.nRealInputs_ );


    def defineRealOutputs( self, realOutputNames ):
        self.nRealOutputs_ = len( realOutputNames )
        for i in range( 0, self.nRealOutputs_ ):
            self.realOutputs_.push_back( 0. );
        cdef vector[string] cppRealOutputNames
        for name in realOutputNames:
            cppRealOutputNames.push_back( name.encode( 'UTF-8' ) )

        self.thisptr_.defineRealOutputs( &cppRealOutputNames[0], self.nRealOutputs_ );


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
