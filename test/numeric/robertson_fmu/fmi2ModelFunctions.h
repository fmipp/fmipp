#ifndef fmi2ModelFunctions_h
#define fmi2ModelFunctions_h

/* This header file must be utilized when compiling a FMU.
   It defines all functions of the
         FMI 2.0 Model Exchange and Co-Simulation Interface.

   In order to have unique function names even if several FMUs
   are compiled together (e.g. for embedded systems), every "real" function name
   is constructed by prepending the function name by "FMI2_FUNCTION_PREFIX".
   Therefore, the typical usage is:

      #define FMI2_FUNCTION_PREFIX MyModel_
      #include "fmi2Functions.h"

   As a result, a function that is defined as "fmi2GetDerivatives" in this header file,
   is actually getting the name "MyModel_fmi2GetDerivatives".

   This only holds if the FMU is shipped in C source code, or is compiled in a
   static link library. For FMUs compiled in a DLL/sharedObject, the "actual" function
   names are used and "FMI2_FUNCTION_PREFIX" must not be defined.

   Revisions:
   - Apr.  9, 2014: all prefixes "fmi" renamed to "fmi2" (decision from April 8)
   - Mar. 26, 2014: FMI_Export set to empty value if FMI_Export and FMI_FUNCTION_PREFIX
                    are not defined (#173)
   - Oct. 11, 2013: Functions of ModelExchange and CoSimulation merged:
                      fmiInstantiateModel , fmiInstantiateSlave  -> fmiInstantiate
                      fmiFreeModelInstance, fmiFreeSlaveInstance -> fmiFreeInstance
                      fmiEnterModelInitializationMode, fmiEnterSlaveInitializationMode -> fmiEnterInitializationMode
                      fmiExitModelInitializationMode , fmiExitSlaveInitializationMode  -> fmiExitInitializationMode
                      fmiTerminateModel, fmiTerminateSlave  -> fmiTerminate
                      fmiResetSlave -> fmiReset (now also for ModelExchange and not only for CoSimulation)
                    Functions renamed:
                      fmiUpdateDiscreteStates -> fmiNewDiscreteStates
   - June 13, 2013: Functions removed:
                       fmiInitializeModel
                       fmiEventUpdate
                       fmiCompletedEventIteration
                       fmiInitializeSlave
                    Functions added:
                       fmiEnterModelInitializationMode
                       fmiExitModelInitializationMode
                       fmiEnterEventMode
                       fmiUpdateDiscreteStates
                       fmiEnterContinuousTimeMode
                       fmiEnterSlaveInitializationMode;
                       fmiExitSlaveInitializationMode;
   - Feb. 17, 2013: Portability improvements:
                       o DllExport changed to FMI_Export
                       o FUNCTION_PREFIX changed to FMI_FUNCTION_PREFIX
                       o Allow undefined FMI_FUNCTION_PREFIX (meaning no prefix is used)
                    Changed function name "fmiTerminate" to "fmiTerminateModel" (due to #113)
                    Changed function name "fmiGetNominalContinuousState" to
                                          "fmiGetNominalsOfContinuousStates"
                    Removed fmiGetStateValueReferences.
   - Nov. 14, 2011: Adapted to FMI 2.0:
                       o Split into two files (fmiFunctions.h, fmiTypes.h) in order
                         that code that dynamically loads an FMU can directly
                         utilize the header files).
                       o Added C++ encapsulation of C-part, in order that the header
                         file can be directly utilized in C++ code.
                       o fmiCallbackFunctions is passed as pointer to fmiInstantiateXXX
                       o stepFinished within fmiCallbackFunctions has as first
                         argument "fmiComponentEnvironment" and not "fmiComponent".
                       o New functions to get and set the complete FMU state
                         and to compute partial derivatives.
   - Nov.  4, 2010: Adapted to specification text:
                       o fmiGetModelTypesPlatform renamed to fmiGetTypesPlatform
                       o fmiInstantiateSlave: Argument GUID     replaced by fmuGUID
                                              Argument mimetype replaced by mimeType
                       o tabs replaced by spaces
   - Oct. 16, 2010: Functions for FMI for Co-simulation added
   - Jan. 20, 2010: stateValueReferencesChanged added to struct fmiEventInfo (ticket #27)
                    (by M. Otter, DLR)
                    Added WIN32 pragma to define the struct layout (ticket #34)
                    (by J. Mauss, QTronic)
   - Jan.  4, 2010: Removed argument intermediateResults from fmiInitialize
                    Renamed macro fmiGetModelFunctionsVersion to fmiGetVersion
                    Renamed macro fmiModelFunctionsVersion to fmiVersion
                    Replaced fmiModel by fmiComponent in decl of fmiInstantiateModel
                    (by J. Mauss, QTronic)
   - Dec. 17, 2009: Changed extension "me" to "fmi" (by Martin Otter, DLR).
   - Dez. 14, 2009: Added eventInfo to meInitialize and added
                    meGetNominalContinuousStates (by Martin Otter, DLR)
   - Sept. 9, 2009: Added DllExport (according to Peter Nilsson's suggestion)
                    (by A. Junghanns, QTronic)
   - Sept. 9, 2009: Changes according to FMI-meeting on July 21:
                    meInquireModelTypesVersion     -> meGetModelTypesPlatform
                    meInquireModelFunctionsVersion -> meGetModelFunctionsVersion
                    meSetStates                    -> meSetContinuousStates
                    meGetStates                    -> meGetContinuousStates
                    removal of meInitializeModelClass
                    removal of meGetTime
                    change of arguments of meInstantiateModel
                    change of arguments of meCompletedIntegratorStep
                    (by Martin Otter, DLR):
   - July 19, 2009: Added "me" as prefix to file names (by Martin Otter, DLR).
   - March 2, 2009: Changed function definitions according to the last design
                    meeting with additional improvements (by Martin Otter, DLR).
   - Dec. 3 , 2008: First version by Martin Otter (DLR) and Hans Olsson (Dynasim).

   Copyright © 2008-2011 MODELISAR consortium,
               2012-2013 Modelica Association Project "FMI"
               All rights reserved.
   This file is licensed by the copyright holders under the BSD 2-Clause License
   (http://www.opensource.org/licenses/bsd-license.html):

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

   with the extension:

   You may distribute or publicly perform any modification only under the
   terms of this license.
   (Note, this means that if you distribute a modified file,
    the modified file must also be provided under this license).
*/

#include "fmi2ModelTypes.h"
#include <stdlib.h>

/* Export fmi functions on Windows */
#ifdef _MSC_VER
#define FMI2_Export __declspec( dllexport )
#else
#define FMI2_Export
#endif

/* do not prepend the name for dynamic linking like it was done in 1.0 */

/* Version number */
#define fmi2Version "2.0"


/***************************************************
Types for Common Functions
****************************************************/

/* Inquire version numbers of header files and setting logging status */
FMI2_Export const char* fmi2GetTypesPlatform(void);
FMI2_Export const char* fmi2GetVersion(void);

/* make sure all compiler use the same alignment policies for structures */
#ifdef WIN32
#pragma pack(push,8)
#endif

/* Type definitions */
typedef enum {
	fmi2OK,
	fmi2Warning,
	fmi2Discard,
	fmi2Error,
	fmi2Fatal,
	fmi2Pending
} fmi2Status;

typedef enum {
	fmi2ModelExchange,
	fmi2CoSimulation
} fmi2Type;

typedef enum {
	fmi2DoStepStatus,
	fmi2PendingStatus,
	fmi2LastSuccessfulTime,
	fmi2Terminated
} fmi2StatusKind;

typedef enum {
    modelStartAndEnd        = 1<<0,
    modelInstantiated       = 1<<1,
    modelInitializationMode = 1<<2,

    // ME states
    modelEventMode          = 1<<3,
    modelContinuousTimeMode = 1<<4,
    // CS states
    modelStepComplete       = 1<<5,
    modelStepInProgress     = 1<<6,
    modelStepFailed         = 1<<7,
    modelStepCanceled       = 1<<8,

    modelTerminated         = 1<<9,
    modelError              = 1<<10,
    modelFatal              = 1<<11,
} ModelState;

typedef void      (*fmi2CallbackLogger)        (fmi2ComponentEnvironment, fmi2String,
						fmi2Status, fmi2String, fmi2String, ...);
typedef void*     (*fmi2CallbackAllocateMemory)(size_t, size_t);
typedef void      (*fmi2CallbackFreeMemory)    (void*);
typedef void      (*fmi2StepFinished)          (fmi2ComponentEnvironment, fmi2Status);

typedef struct {
	const fmi2CallbackLogger         logger;
	const fmi2CallbackAllocateMemory allocateMemory;
	const fmi2CallbackFreeMemory     freeMemory;
	const fmi2StepFinished           stepFinished;
	const fmi2ComponentEnvironment   componentEnvironment;
} fmi2CallbackFunctions;

typedef struct {
	 fmi2Boolean newDiscreteStatesNeeded;
	fmi2Boolean terminateSimulation;
	fmi2Boolean nominalsOfContinuousStatesChanged;
	fmi2Boolean valuesOfContinuousStatesChanged;
	fmi2Boolean nextEventTimeDefined;
	fmi2Real    nextEventTime;
} fmi2EventInfo;

/* reset alignment policy to the one set before reading this file */
#ifdef WIN32
#pragma pack(pop)
#endif

FMI2_Export fmi2Status  fmi2SetDebugLogging(fmi2Component c,
					    fmi2Boolean loggingOn,
					    size_t n_Categories,
					    const fmi2String categories[]);

/* Creation and destruction of FMU instances and setting debug status */
FMI2_Export fmi2Component fmi2Instantiate (fmi2String instanceName,
					   fmi2Type   fmuType,
					   fmi2String fmuGUID,
					   fmi2String fmuResourceLocation,
					   const fmi2CallbackFunctions* functions,
					   fmi2Boolean visible,
					   fmi2Boolean loggingOn);

FMI2_Export void          fmi2FreeInstance(fmi2Component c);

/* Enter and exit initialization mode, terminate and reset */
FMI2_Export fmi2Status fmi2SetupExperiment (fmi2Component c,
					    fmi2Boolean toleranceDefined,
					    fmi2Real tolerance,
					    fmi2Real startTime,
					    fmi2Boolean stopTimeDefined,
					    fmi2Real stopTime);
FMI2_Export fmi2Status fmi2EnterInitializationMode(fmi2Component c);
FMI2_Export fmi2Status fmi2ExitInitializationMode (fmi2Component c);
FMI2_Export fmi2Status fmi2Terminate              (fmi2Component c);
FMI2_Export fmi2Status fmi2Reset                  (fmi2Component c);

/* Getting and setting variable values */
FMI2_Export fmi2Status fmi2GetReal   (fmi2Component c,   const fmi2ValueReference vr[],
				      size_t        nvr, fmi2Real                 value[]);
FMI2_Export fmi2Status fmi2GetInteger(fmi2Component c,   const fmi2ValueReference vr[],
				      size_t        nvr, fmi2Integer              value[]);
FMI2_Export fmi2Status fmi2GetBoolean(fmi2Component c,   const fmi2ValueReference vr[],
				      size_t        nvr, fmi2Boolean              value[]);
FMI2_Export fmi2Status fmi2GetString (fmi2Component c  , const fmi2ValueReference vr[],
				      size_t        nvr, fmi2String               value[]);

FMI2_Export fmi2Status fmi2SetReal   (fmi2Component c  , const fmi2ValueReference vr[],
				      size_t        nvr, const fmi2Real           value[]);
FMI2_Export fmi2Status fmi2SetInteger(fmi2Component c  , const fmi2ValueReference vr[],
				      size_t        nvr, const fmi2Integer        value[]);
FMI2_Export fmi2Status fmi2SetBoolean(fmi2Component c  , const fmi2ValueReference vr[],
				      size_t        nvr, const fmi2Boolean        value[]);
FMI2_Export fmi2Status fmi2SetString (fmi2Component c  , const fmi2ValueReference vr[],
				      size_t        nvr, const fmi2String         value[]);

/* Getting and setting the internal FMU state */
FMI2_Export fmi2Status fmi2GetFMUstate           (fmi2Component c, fmi2FMUstate* FMUState);
FMI2_Export fmi2Status fmi2SetFMUstate           (fmi2Component c, fmi2FMUstate  FMUState);
FMI2_Export fmi2Status fmi2FreeFMUstate          (fmi2Component c, fmi2FMUstate* FMUState);
FMI2_Export fmi2Status fmi2SerializedFMUstateSize(fmi2Component c, fmi2FMUstate  FMUState,
						  size_t* size);
FMI2_Export fmi2Status fmi2SerializeFMUstate     (fmi2Component c, fmi2FMUstate  FMUState,
						  fmi2Byte serializedState[], size_t size);
FMI2_Export fmi2Status fmi2DeSerializeFMUstate   (fmi2Component c, const fmi2Byte serializedState[],
						  size_t size, fmi2FMUstate* FMUState);

/* Getting partial derivatives */
FMI2_Export fmi2Status fmi2GetDirectionalDerivative(fmi2Component c,
						    const fmi2ValueReference vUnknown_ref[],
						    size_t nUnknown,
						    const fmi2ValueReference vKnown_ref[],
						    size_t nKnown,
						    const fmi2Real dvKnown[], fmi2Real dvUnknown[]);

/***************************************************
Types for Functions for FMI2 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
FMI2_Export fmi2Status fmi2EnterEventMode         (fmi2Component c);
FMI2_Export fmi2Status fmi2NewDiscreteStates      (fmi2Component c, fmi2EventInfo* fmi2EventInfo);
FMI2_Export fmi2Status fmi2EnterContinuousTimeMode(fmi2Component c);
FMI2_Export fmi2Status fmi2CompletedIntegratorStep(fmi2Component c,
						   fmi2Boolean noSetFMUStatePriorToCurrentPoint,
						   fmi2Boolean* enterEventMode,
						   fmi2Boolean* terminateSimulation);
//                                                    fmi2Boolean* terminateSimulation);

/* Providing independent variables and re-initialization of caching */
FMI2_Export fmi2Status fmi2SetTime            (fmi2Component c, fmi2Real time);
FMI2_Export fmi2Status fmi2SetContinuousStates(fmi2Component c, const fmi2Real x[], size_t nx);

/* Evaluation of the model equations */
FMI2_Export fmi2Status fmi2GetDerivatives               (fmi2Component c,
							 fmi2Real derivatives[],
							 size_t nx);
FMI2_Export fmi2Status fmi2GetEventIndicators           (fmi2Component c,
							 fmi2Real eventIndicators[],
							 size_t ni);
FMI2_Export fmi2Status fmi2GetContinuousStates          (fmi2Component c, fmi2Real x[], size_t nx);
FMI2_Export fmi2Status fmi2GetNominalsOfContinuousStates(fmi2Component c, fmi2Real x_nominal[], size_t nx);


/***************************************************
Types for Functions for FMI2 for Co-Simulation
****************************************************/

/* Simulating the slave */
FMI2_Export fmi2Status fmi2SetRealInputDerivatives (fmi2Component c,
						    const fmi2ValueReference vr[],
						    size_t nvr,
						    const fmi2Integer order[],
						    const fmi2Real value[]);
FMI2_Export fmi2Status fmi2GetRealOutputDerivatives(fmi2Component c,
						    const fmi2ValueReference vr[],
						    size_t nvr,
						    const fmi2Integer order[],
						    fmi2Real value[]);

FMI2_Export fmi2Status fmi2DoStep     (fmi2Component c,
				       fmi2Real currentCommunicationPoint,
				       fmi2Real communicationPointStepSize,
				       fmi2Boolean noSetFMUStatePriorToCurrentPoint);
FMI2_Export fmi2Status fmi2CancelStep (fmi2Component c);

/* Inquire slave status */
FMI2_Export fmi2Status fmi2GetStatus       ( fmi2Component c, const fmi2StatusKind s,
					     fmi2Status*  value );
FMI2_Export fmi2Status fmi2GetRealStatus   ( fmi2Component c, const fmi2StatusKind s,
					     fmi2Real* vlaue );
FMI2_Export fmi2Status fmi2GetIntegerStatus( fmi2Component c, const fmi2StatusKind s,
					     fmi2Integer* value );
FMI2_Export fmi2Status fmi2GetBooleanStatus( fmi2Component c, const fmi2StatusKind s,
					     fmi2Boolean* value );
FMI2_Export fmi2Status fmi2GetStringStatus ( fmi2Component c, const fmi2StatusKind s,
					     fmi2String* value );


#endif // fmi2ModelFunctions_h
