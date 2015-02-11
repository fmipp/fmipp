/**
 * \file fmi_cs.h
 *
 * Function types for all function of the "FMI for Co-Simulation 1.0"
 * and a struct with the corresponding function pointers.
 *
 * ----------------------------------------------------------------------------
 *
 * Revisions:
 * - May 14, 2014: adapted to fit the needs of FMI++ (by E. Widl, AIT)
 *
 * ----------------------------------------------------------------------------
 *
 * Copyright 2011 QTronic GmbH. All rights reserved.
 * The FmuSdk is licensed by the copyright holder under the BSD License
 * (http://www.opensource.org/licenses/bsd-license.html):
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY QTRONIC GMBH "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL QTRONIC GMBH BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _FMIPP_FMICS_H
#define _FMIPP_FMICS_H


/* Version number */
#define fmiVersion "1.0"

#include <stdlib.h>
#include "fmiModelTypes.h"

/** This namespace separates the defintions for FMI CS from the definitions for FMI ME.
 *  This separation is necessary because of the differing definitions for struct fmiCallbackFunctions.
 */
namespace cs {

typedef void (*fmiCallbackLogger) (fmiComponent c, fmiString instanceName, fmiStatus status,
				   fmiString category, fmiString message, ...);
typedef void* (*fmiCallbackAllocateMemory) (size_t nobj, size_t size);
typedef void (*fmiCallbackFreeMemory) (void* obj);
typedef void (*fmiStepFinished) (fmiComponent c, fmiStatus status);

typedef struct {
	fmiCallbackLogger         logger;
	fmiCallbackAllocateMemory allocateMemory;
	fmiCallbackFreeMemory     freeMemory;
	fmiStepFinished           stepFinished;
} fmiCallbackFunctions;


typedef const char* (*fGetTypesPlatform)();
typedef const char* (*fGetVersion)();
typedef fmiStatus (*fSetDebugLogging) ( fmiComponent c, fmiBoolean loggingOn );
typedef fmiStatus (*fSetReal)   ( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal    value[] );
typedef fmiStatus (*fSetInteger)( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[] );
typedef fmiStatus (*fSetBoolean)( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[] );
typedef fmiStatus (*fSetString) ( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[] );
typedef fmiStatus (*fGetReal)   ( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal    value[] );
typedef fmiStatus (*fGetInteger)( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[] );
typedef fmiStatus (*fGetBoolean)( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[] );
typedef fmiStatus (*fGetString) ( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[] );
typedef fmiComponent (*fInstantiateSlave) ( fmiString  instanceName, fmiString  fmuGUID, fmiString  fmuLocation,
						    fmiString  mimeType, fmiReal timeout, fmiBoolean visible, fmiBoolean interactive,
						    fmiCallbackFunctions functions, fmiBoolean loggingOn );
typedef fmiStatus (*fInitializeSlave)( fmiComponent c, fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop );
typedef fmiStatus (*fTerminateSlave)   ( fmiComponent c );
typedef fmiStatus (*fResetSlave)       ( fmiComponent c );
typedef void      (*fFreeSlaveInstance)( fmiComponent c );
typedef fmiStatus (*fSetRealInputDerivatives)( fmiComponent c, const  fmiValueReference vr[], size_t nvr,
						       const fmiInteger order[], const  fmiReal value[] );
typedef fmiStatus (*fGetRealOutputDerivatives)( fmiComponent c, const fmiValueReference vr[], size_t  nvr,
							const fmiInteger order[], fmiReal value[] );
typedef fmiStatus (*fCancelStep)( fmiComponent c );
typedef fmiStatus (*fDoStep)( fmiComponent c, fmiReal currentCommunicationPoint,
			      fmiReal communicationStepSize, fmiBoolean newStep );
typedef fmiStatus (*fGetStatus)       ( fmiComponent c, const fmiStatusKind s, fmiStatus*  value );
typedef fmiStatus (*fGetRealStatus)   ( fmiComponent c, const fmiStatusKind s, fmiReal*    value );
typedef fmiStatus (*fGetIntegerStatus)( fmiComponent c, const fmiStatusKind s, fmiInteger* value );
typedef fmiStatus (*fGetBooleanStatus)( fmiComponent c, const fmiStatusKind s, fmiBoolean* value );
typedef fmiStatus (*fGetStringStatus) ( fmiComponent c, const fmiStatusKind s, fmiString*  value );

/**
 * \struct FMUCoSimulation_functions fmi_cs.h
 * Holds pointers to the functions dynamically loaded from a FMU CS.
 */
typedef struct {
	HANDLE dllHandle;
	fGetTypesPlatform getTypesPlatform;
	fGetVersion getVersion;
	fSetDebugLogging setDebugLogging;
	fSetReal setReal;
	fSetInteger setInteger;
	fSetBoolean setBoolean;
	fSetString setString;
	fGetReal getReal;
	fGetInteger getInteger;
	fGetBoolean getBoolean;
	fGetString getString;
	fInstantiateSlave instantiateSlave;
	fInitializeSlave initializeSlave;
	fTerminateSlave terminateSlave;
	fResetSlave resetSlave;
	fFreeSlaveInstance freeSlaveInstance;
	fGetRealOutputDerivatives getRealOutputDerivatives;
	fSetRealInputDerivatives setRealInputDerivatives;
	fDoStep doStep;
	fCancelStep cancelStep;
	fGetStatus getStatus;
	fGetRealStatus getRealStatus;
	fGetIntegerStatus getIntegerStatus;
	fGetBooleanStatus getBooleanStatus;
	fGetStringStatus getStringStatus;
} FMUCoSimulation_functions;

} // namespace cs

#endif // _FMIPP_FMICS_H
