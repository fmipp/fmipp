#ifndef fmi2Functions_h
#define fmi2Functions_h

/**
 * \file fmi2Functions.h
 * Complete set of declarations according to the FMI 2.0 specification.
 */ 


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

/* Version number */
#define fmi2Version "2.0"

/* Export FMI functions on Windows */
#if defined( WIN32 )
#define DllExport __declspec( dllexport )
#else
#define DllExport
#endif

// #define fmi2Paste(a,b)     a ## b
// #define fmi2PasteB(a,b)    fmi2Paste(a,b)
// #define fmi2FullName(name) fmi2PasteB(MODEL_IDENTIFIER, name)

// /***************************************************
// Common Functions
// ****************************************************/
// #define fmi2GetTypesPlatform         fmi2FullName(_fmi2GetTypesPlatform)
// #define fmi2GetVersion               fmi2FullName(_fmi2GetVersion)
// #define fmi2SetDebugLogging          fmi2FullName(_fmi2SetDebugLogging)
// #define fmi2Instantiate              fmi2FullName(_fmi2Instantiate)
// #define fmi2FreeInstance             fmi2FullName(_fmi2FreeInstance)
// #define fmi2SetupExperiment          fmi2FullName(_fmi2SetupExperiment)
// #define fmi2EnterInitializationMode  fmi2FullName(_fmi2EnterInitializationMode)
// #define fmi2ExitInitializationMode   fmi2FullName(_fmi2ExitInitializationMode)
// #define fmi2Terminate                fmi2FullName(_fmi2Terminate)
// #define fmi2Reset                    fmi2FullName(_fmi2Reset)
// #define fmi2GetReal                  fmi2FullName(_fmi2GetReal)
// #define fmi2GetInteger               fmi2FullName(_fmi2GetInteger)
// #define fmi2GetBoolean               fmi2FullName(_fmi2GetBoolean)
// #define fmi2GetString                fmi2FullName(_fmi2GetString)
// #define fmi2SetReal                  fmi2FullName(_fmi2SetReal)
// #define fmi2SetInteger               fmi2FullName(_fmi2SetInteger)
// #define fmi2SetBoolean               fmi2FullName(_fmi2SetBoolean)
// #define fmi2SetString                fmi2FullName(_fmi2SetString)
// #define fmi2GetFMUstate              fmi2FullName(_fmi2GetFMUstate)
// #define fmi2SetFMUstate              fmi2FullName(_fmi2SetFMUstate)
// #define fmi2FreeFMUstate             fmi2FullName(_fmi2FreeFMUstate)
// #define fmi2SerializedFMUstateSize   fmi2FullName(_fmi2SerializedFMUstateSize)
// #define fmi2SerializeFMUstate        fmi2FullName(_fmi2SerializeFMUstate)
// #define fmi2DeSerializeFMUstate      fmi2FullName(_fmi2DeSerializeFMUstate)
// #define fmi2GetDirectionalDerivative fmi2FullName(_fmi2GetDirectionalDerivative)


// /***************************************************
// Functions for FMI2 for Model Exchange
// ****************************************************/
// #define fmi2EnterEventMode                fmi2FullName(_fmi2EnterEventMode)
// #define fmi2NewDiscreteStates             fmi2FullName(_fmi2NewDiscreteStates)
// #define fmi2EnterContinuousTimeMode       fmi2FullName(_fmi2EnterContinuousTimeMode)
// #define fmi2CompletedIntegratorStep       fmi2FullName(_fmi2CompletedIntegratorStep)
// #define fmi2SetTime                       fmi2FullName(_fmi2SetTime)
// #define fmi2SetContinuousStates           fmi2FullName(_fmi2SetContinuousStates)
// #define fmi2GetDerivatives                fmi2FullName(_fmi2GetDerivatives)
// #define fmi2GetEventIndicators            fmi2FullName(_fmi2GetEventIndicators)
// #define fmi2GetContinuousStates           fmi2FullName(_fmi2GetContinuousStates)
// #define fmi2GetNominalsOfContinuousStates fmi2FullName(_fmi2GetNominalsOfContinuousStates)


// /***************************************************
// Functions for FMI2 for Co-Simulation
// ****************************************************/
// #define fmi2SetRealInputDerivatives      fmi2FullName(_fmi2SetRealInputDerivatives)
// #define fmi2GetRealOutputDerivatives     fmi2FullName(_fmi2GetRealOutputDerivatives)
// #define fmi2DoStep                       fmi2FullName(_fmi2DoStep)
// #define fmi2CancelStep                   fmi2FullName(_fmi2CancelStep)
// #define fmi2GetStatus                    fmi2FullName(_fmi2GetStatus)
// #define fmi2GetRealStatus                fmi2FullName(_fmi2GetRealStatus)
// #define fmi2GetIntegerStatus             fmi2FullName(_fmi2GetIntegerStatus)
// #define fmi2GetBooleanStatus             fmi2FullName(_fmi2GetBooleanStatus)
// #define fmi2GetStringStatus              fmi2FullName(_fmi2GetStringStatus)


/***************************************************
Definitions
****************************************************/

#include <stdlib.h>
#include "common/fmi_v2.0/fmi2ModelTypes.h"

typedef void (*fmi2CallbackLogger) (fmi2ComponentEnvironment componentEnviroment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...);
typedef void* (*fmi2CallbackAllocateMemory) (size_t nobj, size_t size);
typedef void (*fmi2CallbackFreeMemory) (void* obj);
typedef void (*fmi2StepFinished) (fmi2ComponentEnvironment componentEnvironment, fmi2Status status);

typedef struct {
	// the original uses const functions. \FIXME ?
	fmi2CallbackLogger         logger;
	fmi2CallbackAllocateMemory allocateMemory;
	fmi2CallbackFreeMemory     freeMemory;
	fmi2StepFinished           stepFinished;
	fmi2ComponentEnvironment   componentEnvironment;
} fmi2CallbackFunctions;


extern "C" {

	/***************************************************
	    Common Functions
	****************************************************/

	/* Inquire version numbers of header files and setting logging status */
	DllExport const char* fmi2GetTypesPlatform();
	DllExport const char* fmi2GetVersion();
	DllExport fmi2Status  fmi2SetDebugLogging(fmi2Component c,
						       fmi2Boolean loggingOn,
						       size_t nCategories,
						       const fmi2String categories[]);

	/* Creation and destruction of FMU instances and setting debug status */
	DllExport fmi2Component fmi2Instantiate(fmi2String instanceName,
						      fmi2Type   fmuType,
						      fmi2String fmuGUID,
						      fmi2String fmuResourceLocation,
						      const fmi2CallbackFunctions* functions,
						      fmi2Boolean visible,
						      fmi2Boolean loggingOn);

	DllExport void fmi2FreeInstance(fmi2Component c);

	/* Enter and exit initialization mode, terminate and reset */
	DllExport fmi2Status fmi2SetupExperiment(fmi2Component c,
						       fmi2Boolean toleranceDefined,
						       fmi2Real tolerance,
						       fmi2Real startTime,
						       fmi2Boolean stopTimeDefined,
						       fmi2Real stopTime);
	DllExport fmi2Status fmi2EnterInitializationMode(fmi2Component c);
	DllExport fmi2Status fmi2ExitInitializationMode(fmi2Component c);
	DllExport fmi2Status fmi2Terminate(fmi2Component c);
	DllExport fmi2Status fmi2Reset(fmi2Component c);

	/* Getting and setting variable values */
	DllExport fmi2Status fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]);
	DllExport fmi2Status fmi2GetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]);
	DllExport fmi2Status fmi2GetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]);
	DllExport fmi2Status fmi2GetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String value[]);

	DllExport fmi2Status fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]);
	DllExport fmi2Status fmi2SetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]);
	DllExport fmi2Status fmi2SetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]);
	DllExport fmi2Status fmi2SetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]);

	/* Getting and setting the internal FMU state */
	DllExport fmi2Status fmi2GetFMUstate(fmi2Component c, fmi2FMUstate* fmuState);
	DllExport fmi2Status fmi2SetFMUstate(fmi2Component c, fmi2FMUstate fmuState);
	DllExport fmi2Status fmi2FreeFMUstate(fmi2Component c, fmi2FMUstate* fmuState);
	DllExport fmi2Status fmi2SerializedFMUstateSize(fmi2Component c, fmi2FMUstate fmuState, size_t* size);
	DllExport fmi2Status fmi2SerializeFMUstate(fmi2Component c, fmi2FMUstate fmuState, fmi2Byte serializedState[], size_t size);
	DllExport fmi2Status fmi2DeSerializeFMUstate(fmi2Component c, const fmi2Byte serializedState[], size_t size, fmi2FMUstate* fmuState);

	/* Getting partial derivatives */
	DllExport fmi2Status fmi2GetDirectionalDerivative(fmi2Component c,
							       const fmi2ValueReference vUnknown_ref[],
							       size_t nUnknown,
							       const fmi2ValueReference vKnown_ref[],
							       size_t nKnown,
							       const fmi2Real dvKnown[], fmi2Real dvUnknown[]);

	/***************************************************
	    Functions for FMI2 for Model Exchange
	****************************************************/

	/* Enter and exit the different modes */
	DllExport fmi2Status fmi2EnterEventMode(fmi2Component c);
	DllExport fmi2Status fmi2NewDiscreteStates(fmi2Component c, fmi2EventInfo* fmi2EventInfo);
	DllExport fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c);
	DllExport fmi2Status fmi2CompletedIntegratorStep(fmi2Component c,
							      fmi2Boolean noSetFMUStatePriorToCurrentPoint,
							      fmi2Boolean* enterEventMode,
							      fmi2Boolean* terminateSimulation);

	/* Providing independent variables and re-initialization of caching */
	DllExport fmi2Status fmi2SetTime(fmi2Component c, fmi2Real time);
	DllExport fmi2Status fmi2SetContinuousStates(fmi2Component c, const fmi2Real x[], size_t nx);

	/* Evaluation of the model equations */
	DllExport fmi2Status fmi2GetDerivatives(fmi2Component c, fmi2Real derivatives[], size_t nx);
	DllExport fmi2Status fmi2GetEventIndicators(fmi2Component c, fmi2Real eventIndicators[], size_t ni);
	DllExport fmi2Status fmi2GetContinuousStates(fmi2Component c, fmi2Real x[], size_t nx);
	DllExport fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Component c, fmi2Real x_nominal[], size_t nx);

	/***************************************************
	    Functions for FMI2 for Co-Simulation
	****************************************************/

	/* Simulating the slave */
	DllExport fmi2Status fmi2SetRealInputDerivatives(fmi2Component c,
							       const fmi2ValueReference vr[],
							       size_t nvr,
							       const fmi2Integer order[],
							       const fmi2Real value[]);
	DllExport fmi2Status fmi2GetRealOutputDerivatives(fmi2Component c,
							       const fmi2ValueReference vr[],
							       size_t nvr,
							       const fmi2Integer order[],
							       fmi2Real value[]);

	DllExport fmi2Status fmi2DoStep(fmi2Component c,
						  fmi2Real currentCommunicationPoint,
						  fmi2Real communicationPointStepSize,
						  fmi2Boolean noSetFMUStatePriorToCurrentPoint);
	DllExport fmi2Status fmi2CancelStep (fmi2Component c);

	/* Inquire slave status */
	DllExport fmi2Status fmi2GetStatus(fmi2Component c, const fmi2StatusKind s, fmi2Status*  value);
	DllExport fmi2Status fmi2GetRealStatus(fmi2Component c, const fmi2StatusKind s, fmi2Real* vlaue);
	DllExport fmi2Status fmi2GetIntegerStatus(fmi2Component c, const fmi2StatusKind s, fmi2Integer* value);
	DllExport fmi2Status fmi2GetBooleanStatus(fmi2Component c, const fmi2StatusKind s, fmi2Boolean* value);
	DllExport fmi2Status fmi2GetStringStatus(fmi2Component c, const fmi2StatusKind s, fmi2String* value);
}

#endif // fmi2Functions_h
