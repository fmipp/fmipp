// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file FMIPPStatusKind.h

#ifndef _FMIPP_STATUS_KIND_H
#define _FMIPP_STATUS_KIND_H

/**
 * \enum fmippStatus FMIPPStatus.h
 * FMU status flag for FMI++ Library.
 */
 typedef enum {
    fmippDoStepStatus,
    fmippPendingStatus,
    fmippLastSuccessfulTime,
    fmippTerminated
} fmippStatusKind;

#endif // _FMIPP_STATUS_KIND_H
