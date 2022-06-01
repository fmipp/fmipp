// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file FMIPPVariableType.h

#ifndef _FMIPP_FMIPPVARIABLETYPE_H
#define _FMIPP_FMIPPVARIABLETYPE_H

/**
 * \enum FMIPPVariableType FMIPPVariableType.h
 * Enumerator for FMI model variable types.
 */
enum FMIPPVariableType {
	fmippTypeReal = 0,
	fmippTypeInteger = 1,
	fmippTypeBoolean = 2,
	fmippTypeString = 3,
	fmippTypeUnknown = 4
};


#endif // _FMIPP_FMIPPVARIABLETYPE_H
