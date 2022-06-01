// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

 #ifndef _FMIPP_FMUTYPE_H
#define _FMIPP_FMUTYPE_H

/// \file FMUType.h

/**
 * \enum FMUType FMUType.h
 * Enumerator for FMU type, i.e., FMI version numer and ME/CS support.
 */
enum FMUType {
	invalid = 0,
	fmi_1_0_me = 10,
	fmi_1_0_cs = 11,
	fmi_2_0_me = 20,
	fmi_2_0_cs = 21,
	fmi_2_0_me_and_cs = 22
};


#endif // _FMIPP_FMUTYPE_H
