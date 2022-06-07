// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file FMIPPConfig.h

#ifndef _FMIPP_FMIPPCONFIG_H
#define _FMIPP_FMIPPCONFIG_H

// Include common definition of types for FMI++ Library.
#include "common/FMIPPStatus.h"
#include "common/FMIPPStatusKind.h"
#include "common/FMIPPTypes.h"
#include "common/FMIPPVariableType.h"
#include "common/FMUType.h"

#if defined( WIN32 ) // Windows.
  #if defined( MINGW ) // MinGW/GCC.
    #define _FMIPP_BUILD_TYPE "MINGW/GCC"
  #elif defined ( _MSC_VER ) // MSVC
    #define _FMIPP_BUILD_TYPE "MSVC"
  #endif
#else // Linux, Unix, etc.
  #define _FMIPP_BUILD_TYPE "LINUX/UNIX"
#endif


extern "C"
{

#if defined( WIN32 ) // Windows (MinGW/GCC & Visual Studio).

  #ifdef BUILD_FMI_DLL
    #define __FMI_DLL __declspec(dllexport)
  #else
    #define __FMI_DLL __declspec(dllimport)
  #endif

#else // Linux.

  #define __FMI_DLL

#endif


#if defined(MINGW) // Definitions specific for MinGW/GCC.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <errno.h>

#elif defined(_MSC_VER) // Definitions specific for MS Visual Studio (Visual C/C++).

#pragma warning( disable : 4251 ) /// \FIXME Is this really a good idea? What about the solutions suggested in http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html ???
#pragma warning( disable : 4482 ) // This is a warning about scoping of enums. It is valid C++11 syntax, though.
#pragma message( "ATTENTION: Warnings 4251 and 4482 have been disabled." )

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <errno.h>

#else  // Definitions specific for Linux.

#include <errno.h>
#define WINDOWS 0
#define TRUE 1
#define FALSE 0
#define HANDLE void *
/* See http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html */
#include <dlfcn.h>

#endif

} // extern "C"

#endif // _FMIPP_FMIPPCONFIG_H
