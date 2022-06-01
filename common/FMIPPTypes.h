// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file FMIPPTypes.h

#ifndef _FMIPP_TYPES_H
#define _FMIPP_TYPES_H

#include <limits>
#include <string>
#include <stdint.h>
#include <stdbool.h>

/** Used for array indexing and loop counting- */
typedef std::size_t fmippSize;

/** Pointer to FMU instance. */
typedef void* fmippInstance;

/** Pointer to FMU environment. */
typedef void* fmippInstanceEnvironment;

/** Pointer to FMU state. */
typedef void* fmippFMUState;

/** Handle to the value of a variable. */
typedef uint32_t fmippValueReference;

/** Single precision floating point (32-bit). */
typedef        float fmippFloat32;
/** Double precision floating point (64-bit). */
typedef       double fmippFloat64;
/** Default floating point type for FMI++. */
typedef fmippFloat64 fmippReal;

/** 8-bit signed integer. */
typedef     int8_t fmippInt8;
/** 8-bit unsigned integer. */
typedef    uint8_t fmippUInt8;
/** 16-bit signed integer. */
typedef    int16_t fmippInt16;
/** 16-bit unsigned integer. */
typedef   uint16_t fmippUInt16;
/** 32-bit signed integer. */
typedef    int32_t fmippInt32;
/** 32-bit unsigned integer. */
typedef   uint32_t fmippUInt32;
/** 64-bit signed integer. */
typedef    int64_t fmippInt64;
/*+ 64-bit unsigned integer. */
typedef   uint64_t fmippUInt64;
/** Default integer type for FMI++. */
typedef fmippInt32 fmippInteger;

/** Data type to be used with fmippTrue and fmippFalse. */
typedef bool fmippBoolean;

/** Data type for single character. */
typedef        char fmippChar;
/** Data type for character strings. */
typedef std::string fmippString;

/** Smallest addressable unit of the machine (typically one byte). */
typedef          uint8_t fmippByte;
/** Data type for binary data (out-of-band length terminated). */
typedef const fmippByte* fmippBinary;

/** Data type for FMI3 clocks. */
typedef bool fmippClock;

/** Data type for representing time. */
typedef fmippReal fmippTime;

// Values for fmippBoolean.
#define fmippTrue  true
#define fmippFalse false

// Values for fmippClock.
#define fmippClockActive   true
#define fmippClockInactive false

// Define invalid time.
#define INVALID_FMI_TIME std::numeric_limits<fmippTime>::infinity()

#define fmippUndefinedValueReference (fmippValueReference)(-1)

#endif // _FMIPP_TYPES_H
