/**
 * \file digusermodel.cpp 
 * External C-Interface for dynamic models for PowerFactory (DLL main function).
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


BOOL WINAPI DllMain (HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
  switch (dwReason)
  {
    case DLL_PROCESS_ATTACH:
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}
