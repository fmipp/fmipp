#ifndef _FMIPP_CONFIG_H
#define _FMIPP_CONFIG_H

extern "C"
{

#if defined(WIN32) // Windows.
  #ifdef BUILD_FMI_DLL
    #define __FMI_DLL __declspec(dllexport)
  #else
    #define __FMI_DLL __declspec(dllimport)
  #endif
#else // Unices.
  #define __FMI_DLL
#endif


#if defined(MINGW) or defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <errno.h>
#else
#include <errno.h>
#define WINDOWS 0
#define TRUE 1
#define FALSE 0
#define HANDLE void *
/* See http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html */
#include <dlfcn.h>
#endif

} // extern "C"

#endif
