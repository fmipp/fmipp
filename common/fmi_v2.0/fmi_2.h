/**
 * \file fmi_2.h
 *
 * Function types for all function of the "FMI for Model Exchange and
 * Co-Simulation 2.0" and a struct with the corresponding function pointers.
 *
 * ----------------------------------------------------------------------------
 *
 * Revisions:
 * - Nov 13, 2014: adapted to fit the needs of FMI++ (by G. de Cillia, AIT)
 *
 * ----------------------------------------------------------------------------
 *
 * Copyright 2010 QTronic GmbH. All rights reserved.
 *
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

#ifndef _FMIPP_FMI2_H
#define _FMIPP_FMI2_H

/* Version number */
#define fmi2Version "2.0"

#include <stdlib.h>
#include "fmi2ModelTypes.h"

namespace fmi2 {

/** Make sure all compiler use the same alignment policies for structures. **/
#ifdef WIN32
#pragma pack(push,8)
#endif

	typedef void      (*fmi2CallbackLogger)        (fmi2ComponentEnvironment componentEnviroment,
							fmi2String instanceName,
							fmi2Status status,
							fmi2String category,
							fmi2String message, ...);
	typedef void*     (*fmi2CallbackAllocateMemory)(size_t nobj, size_t size);
	typedef void      (*fmi2CallbackFreeMemory)    (void* obj);
	typedef void      (*fmi2StepFinished)          (fmi2ComponentEnvironment componentEnvironment,
							fmi2Status status);

	typedef struct {
		// the original uses const functions. \FIXME ?
		fmi2CallbackLogger         logger;
		fmi2CallbackAllocateMemory allocateMemory;
		fmi2CallbackFreeMemory     freeMemory;
		fmi2StepFinished           stepFinished;
		fmi2ComponentEnvironment   componentEnvironment;
	} fmi2CallbackFunctions;

	/***************************************************
        Types for Common Functions
	****************************************************/

	/* Inquire version numbers of header files and setting logging status */
	typedef const char* (*fmi2GetTypesPlatformTYPE)(void);
	typedef const char* (*fmi2GetVersionTYPE)(void);
	typedef fmi2Status  (*fmi2SetDebugLoggingTYPE)(fmi2Component c,
						       fmi2Boolean loggingOn,
						       size_t nCategories,
						       const fmi2String categories[]);

	/* Creation and destruction of FMU instances and setting debug status */
	typedef fmi2Component (*fmi2InstantiateTYPE) (fmi2String instanceName,
						      fmi2Type   fmuType,
						      fmi2String fmuGUID,
						      fmi2String fmuResourceLocation,
						      const fmi2CallbackFunctions* functions,
						      fmi2Boolean visible,
						      fmi2Boolean loggingOn);

	typedef void          (*fmi2FreeInstanceTYPE)(fmi2Component c);

	/* Enter and exit initialization mode, terminate and reset */
	typedef fmi2Status (*fmi2SetupExperimentTYPE) (fmi2Component c,
						       fmi2Boolean toleranceDefined,
						       fmi2Real tolerance,
						       fmi2Real startTime,
						       fmi2Boolean stopTimeDefined,
						       fmi2Real stopTime);
	typedef fmi2Status (*fmi2EnterInitializationModeTYPE)(fmi2Component c);
	typedef fmi2Status (*fmi2ExitInitializationModeTYPE) (fmi2Component c);
	typedef fmi2Status (*fmi2TerminateTYPE)              (fmi2Component c);
	typedef fmi2Status (*fmi2ResetTYPE)                  (fmi2Component c);

	/* Getting and setting variable values */
	typedef fmi2Status (*fmi2GetRealTYPE)   (fmi2Component c,   const fmi2ValueReference vr[],
						 size_t        nvr, fmi2Real                 value[]);
	typedef fmi2Status (*fmi2GetIntegerTYPE)(fmi2Component c,   const fmi2ValueReference vr[],
						 size_t        nvr, fmi2Integer              value[]);
	typedef fmi2Status (*fmi2GetBooleanTYPE)(fmi2Component c,   const fmi2ValueReference vr[],
						 size_t        nvr, fmi2Boolean              value[]);
	typedef fmi2Status (*fmi2GetStringTYPE) (fmi2Component c  , const fmi2ValueReference vr[],
						 size_t        nvr, fmi2String               value[]);

	typedef fmi2Status (*fmi2SetRealTYPE)   (fmi2Component c  , const fmi2ValueReference vr[],
						 size_t        nvr, const fmi2Real           value[]);
	typedef fmi2Status (*fmi2SetIntegerTYPE)(fmi2Component c  , const fmi2ValueReference vr[],
						 size_t        nvr, const fmi2Integer        value[]);
	typedef fmi2Status (*fmi2SetBooleanTYPE)(fmi2Component c  , const fmi2ValueReference vr[],
						 size_t        nvr, const fmi2Boolean        value[]);
	typedef fmi2Status (*fmi2SetStringTYPE) (fmi2Component c  , const fmi2ValueReference vr[],
						 size_t        nvr, const fmi2String         value[]);

	/* Getting and setting the internal FMU state */
	typedef fmi2Status (*fmi2GetFMUstateTYPE)           (fmi2Component c, fmi2FMUstate* FMUState);
	typedef fmi2Status (*fmi2SetFMUstateTYPE)           (fmi2Component c, fmi2FMUstate  FMUState);
	typedef fmi2Status (*fmi2FreeFMUstateTYPE)          (fmi2Component c, fmi2FMUstate* FMUState);
	typedef fmi2Status (*fmi2SerializedFMUstateSizeTYPE)(fmi2Component c, fmi2FMUstate  FMUState,
							     size_t* size);
	typedef fmi2Status (*fmi2SerializeFMUstateTYPE)     (fmi2Component c, fmi2FMUstate  FMUState,
							     fmi2Byte serializedState[], size_t size);
	typedef fmi2Status (*fmi2DeSerializeFMUstateTYPE)   (fmi2Component c, const fmi2Byte serializedState[],
							     size_t size, fmi2FMUstate* FMUState);

	/* Getting partial derivatives */
	typedef fmi2Status (*fmi2GetDirectionalDerivativeTYPE)(fmi2Component c,
							       const fmi2ValueReference vUnknown_ref[],
							       size_t nUnknown,
							       const fmi2ValueReference vKnown_ref[],
							       size_t nKnown,
							       const fmi2Real dvKnown[], fmi2Real dvUnknown[]);

	/***************************************************
Types for Functions for FMI2 for Model Exchange
	****************************************************/

	/* Enter and exit the different modes */
	typedef fmi2Status (*fmi2EnterEventModeTYPE)         (fmi2Component c);
	typedef fmi2Status (*fmi2NewDiscreteStatesTYPE)      (fmi2Component c, fmi2EventInfo* fmi2EventInfo);
	typedef fmi2Status (*fmi2EnterContinuousTimeModeTYPE)(fmi2Component c);
	typedef fmi2Status (*fmi2CompletedIntegratorStepTYPE)(fmi2Component c,
							      fmi2Boolean noSetFMUStatePriorToCurrentPoint,
							      fmi2Boolean* enterEventMode,
							      fmi2Boolean* terminateSimulation);

	/* Providing independent variables and re-initialization of caching */
	typedef fmi2Status (*fmi2SetTimeTYPE)            (fmi2Component c, fmi2Real time);
	typedef fmi2Status (*fmi2SetContinuousStatesTYPE)(fmi2Component c, const fmi2Real x[], size_t nx);

	/* Evaluation of the model equations */
	typedef fmi2Status (*fmi2GetDerivativesTYPE)               (fmi2Component c,
								    fmi2Real derivatives[],
								    size_t nx);
	typedef fmi2Status (*fmi2GetEventIndicatorsTYPE)           (fmi2Component c,
								    fmi2Real eventIndicators[],
								    size_t ni);
	typedef fmi2Status (*fmi2GetContinuousStatesTYPE)          (fmi2Component c, fmi2Real x[], size_t nx);
	typedef fmi2Status (*fmi2GetNominalsOfContinuousStatesTYPE)(fmi2Component c, fmi2Real x_nominal[], size_t nx);


	/***************************************************
Types for Functions for FMI2 for Co-Simulation
	****************************************************/

	/* Simulating the slave */
	typedef fmi2Status (*fmi2SetRealInputDerivativesTYPE) (fmi2Component c,
							       const fmi2ValueReference vr[],
							       size_t nvr,
							       const fmi2Integer order[],
							       const fmi2Real value[]);
	typedef fmi2Status (*fmi2GetRealOutputDerivativesTYPE)(fmi2Component c,
							       const fmi2ValueReference vr[],
							       size_t nvr,
							       const fmi2Integer order[],
							       fmi2Real value[]);

	typedef fmi2Status (*fmi2DoStepTYPE)     (fmi2Component c,
						  fmi2Real currentCommunicationPoint,
						  fmi2Real communicationPointStepSize,
						  fmi2Boolean noSetFMUStatePriorToCurrentPoint);
	typedef fmi2Status (*fmi2CancelStepTYPE) (fmi2Component c);

	/* Inquire slave status */
	typedef fmi2Status (*fmi2GetStatusTYPE)       ( fmi2Component c, const fmi2StatusKind s,
							fmi2Status*  value );
	typedef fmi2Status (*fmi2GetRealStatusTYPE)   ( fmi2Component c, const fmi2StatusKind s,
							fmi2Real* vlaue );
	typedef fmi2Status (*fmi2GetIntegerStatusTYPE)( fmi2Component c, const fmi2StatusKind s,
							fmi2Integer* value );
	typedef fmi2Status (*fmi2GetBooleanStatusTYPE)( fmi2Component c, const fmi2StatusKind s,
							fmi2Boolean* value );
	typedef fmi2Status (*fmi2GetStringStatusTYPE) ( fmi2Component c, const fmi2StatusKind s,
							fmi2String* value );

	typedef struct {
		HANDLE dllHandle;

		// common
		fmi2GetTypesPlatformTYPE getTypesPlatform;
		fmi2GetVersionTYPE getVersion;
		fmi2SetDebugLoggingTYPE setDebugLogging;
		fmi2InstantiateTYPE instantiate;
		fmi2FreeInstanceTYPE freeInstance;

		fmi2SetupExperimentTYPE setupExperiment;
		fmi2EnterInitializationModeTYPE enterInitializationMode;
		fmi2ExitInitializationModeTYPE exitInitializationMode;

		fmi2TerminateTYPE terminate;
		fmi2ResetTYPE reset;

		fmi2GetRealTYPE getReal;
		fmi2GetIntegerTYPE getInteger;
		fmi2GetBooleanTYPE getBoolean;
		fmi2GetStringTYPE getString;

		fmi2SetRealTYPE setReal;
		fmi2SetIntegerTYPE setInteger;
		fmi2SetBooleanTYPE setBoolean;
		fmi2SetStringTYPE setString;

		fmi2GetFMUstateTYPE getFMUstate;
		fmi2SetFMUstateTYPE setFMUstate;
		fmi2FreeFMUstateTYPE freeFMUstate;
		fmi2SerializedFMUstateSizeTYPE serializedFMUstateSize;
		fmi2SerializeFMUstateTYPE serializeFMUstate;
		fmi2DeSerializeFMUstateTYPE deSerializeFMUstate;

		fmi2GetDirectionalDerivativeTYPE getDirectionalDerivative;

		// me
		fmi2EnterEventModeTYPE enterEventMode;
		fmi2NewDiscreteStatesTYPE newDiscreteStates;
		fmi2EnterContinuousTimeModeTYPE enterContinuousTimeMode;
		fmi2CompletedIntegratorStepTYPE completedIntegratorStep;

		fmi2SetTimeTYPE setTime;
		fmi2SetContinuousStatesTYPE setContinuousStates;
		fmi2GetDerivativesTYPE getDerivatives;
		fmi2GetEventIndicatorsTYPE getEventIndicators;
		fmi2GetContinuousStatesTYPE getContinuousStates;
		fmi2GetNominalsOfContinuousStatesTYPE getNominalsOfContinuousStates;

		// cs
		fmi2SetRealInputDerivativesTYPE setRealInputDerivatives;
		fmi2GetRealOutputDerivativesTYPE getRealOutputDerivatives;
		fmi2DoStepTYPE doStep;
		fmi2CancelStepTYPE cancelStep;
		fmi2GetStatusTYPE getStatus;
		fmi2GetRealStatusTYPE getRealStatus;
		fmi2GetIntegerStatusTYPE getIntegerStatus;
		fmi2GetBooleanStatusTYPE getBooleanStatus;
		fmi2GetStringStatusTYPE getStringStatus;
	} FMU2_functions;
} // namespace fmi2

#endif // _FMIPP_FMI2_H
