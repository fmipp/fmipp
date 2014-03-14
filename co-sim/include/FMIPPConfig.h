/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMIPPCONFIG_H
#define _FMIPP_FMIPPCONFIG_H

// FIXME: There is a functionally identical copy of this file in "FMIPP_ROOT/include"!!!

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

#pragma warning( disable : 4251 ) // FIXME: Is this really a good idea?
#pragma message( "ATTENTION: Warning 4251 has been disabled." )

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
