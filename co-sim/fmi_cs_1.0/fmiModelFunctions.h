#ifndef fmiModelFunctions_h
#define fmiModelFunctions_h

/*
   ----------------------------------------------------------------------------
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   - Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
   - Neither the name of the copyright holders nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   ----------------------------------------------------------------------------
*/

#include "fmiPlatformTypes.h"
#include <stdlib.h>

/* Version number */
#define fmiVersion "1.0"

/* make sure all compiler use the same alignment policies for structures */
#ifdef WIN32
#pragma pack(push,8)
#endif


/* Type definitions */
typedef enum {fmiOK,
	      fmiWarning,
	      fmiDiscard,
	      fmiError,
	      fmiFatal,
	      fmiPending} fmiStatus;

typedef void  (*fmiCallbackLogger) (fmiComponent c, fmiString instanceName, fmiStatus status,
				    fmiString category, fmiString message, ...);
typedef void* (*fmiCallbackAllocateMemory)(size_t nobj, size_t size);
typedef void  (*fmiCallbackFreeMemory)    (void* obj);
typedef void  (*fmiStepFinished)          (fmiComponent c, fmiStatus status);

typedef struct {
	fmiCallbackLogger         logger;
	fmiCallbackAllocateMemory allocateMemory;
	fmiCallbackFreeMemory     freeMemory;
	fmiStepFinished           stepFinished;
} fmiCallbackFunctions;

typedef struct {
	fmiBoolean iterationConverged;
	fmiBoolean stateValueReferencesChanged;
	fmiBoolean stateValuesChanged;
	fmiBoolean terminateSimulation;
	fmiBoolean upcomingTimeEvent;
	fmiReal    nextEventTime;
} fmiEventInfo;

/* reset alignment policy to the one set before reading this file */
#ifdef WIN32
#pragma pack(pop)
#endif

#endif // fmiModelFunctions_h
