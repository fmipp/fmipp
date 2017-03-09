/**
 * \file digexdyn.cpp 
 * Link to an external event driven C/C++ function for PowerFactory (DLL main function).
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


BOOL WINAPI DllMain( HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason ) {
		case DLL_PROCESS_ATTACH: // DLL is attaching to the address space of the current process.
			break;
		case DLL_THREAD_ATTACH: // A new thread is being created in the current process.
			break;
		case DLL_THREAD_DETACH: // A thread is exiting cleanly.
			break;
		case DLL_PROCESS_DETACH: // The calling process is detaching the DLL from its address space.
			break;
	}

	return TRUE;
}

