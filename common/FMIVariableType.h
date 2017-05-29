#ifndef _FMIPP_FMIVARIABLETYPE_H
#define _FMIPP_FMIVARIABLETYPE_H

/// \file FMIVariableType.h

/**
 * \enum FMIVariableType FMIVariableType.h
 * Enumerator for FMI model variable types.
 */
enum FMIVariableType {
	fmiTypeReal = 0,
	fmiTypeInteger = 1,
	fmiTypeBoolean = 2,
	fmiTypeString = 3,
	fmiTypeUnknown = 4
};


#endif // _FMIPP_FMIVARIABLETYPE_H
