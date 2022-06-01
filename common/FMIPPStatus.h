// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file FMIPPStatus.h

#ifndef _FMIPP_STATUS_H
#define _FMIPP_STATUS_H

/**
 * \enum fmippStatus FMIPPStatus.h
 * FMU status flag for FMI++ Library.
 */
typedef enum {
    fmippOK,
    fmippWarning,
    fmippDiscard,
    fmippError,
    fmippFatal,
} fmippStatus;

#endif // _FMIPP_STATUS_H