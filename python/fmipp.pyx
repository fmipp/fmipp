#!/usr/bin/env python 

"""
Module containing the FMI++ interface Python wrappers.
"""

from libcpp.string cimport string

cdef extern from "FMU.h":

    ctypedef double fmiReal
    ctypedef unsigned int fmiValueReference
    ctypedef char fmiBoolean

    ctypedef enum fmiStatus:
        fmiOK = 0
        fmiWarning = 1
        fmiDiscard = 2
        fmiError = 3
        fmiFatal = 4
        fmiPending = 5

    cdef cppclass FMU:

        FMU( string ) except +
        FMU( string, string ) except +
        FMU( string, string, string ) except +

        fmiStatus initialize()

        fmiStatus integrate( fmiReal, double )



cdef class PyFMU:

    cdef FMU* thisptr      # hold a C++ instance which we're wrapping
	
    def __cinit__( self, string modelName ):
        self.thisptr = new FMU( modelName )
	
    def initialize( self ):
        return self.thisptr.initialize()

    def integrate( self, fmiReal tend, double deltaT ):
        return self.thisptr.integrate( tend, deltaT )
