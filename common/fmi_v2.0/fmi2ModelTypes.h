/**
 * \file fmi2ModelTypes.h
 *
 * Standard header file to define the argument types of the functions of the
 * Model Execution Interface. This header file must be utilized both by the
 * model and by the simulation engine.
 *
 * ----------------------------------------------------------------------------
 *
 * Revisions:
 * - Nov 13, 2014: adapted to fit the needs of FMI++ (by G. de Cillia, AIT)
 *
 * ----------------------------------------------------------------------------
 * Copyright © 2008-2009, MODELISAR consortium. All rights reserved.
 *
 * This file is licensed by the copyright holders under the BSD License
 * (http://www.opensource.org/licenses/bsd-license.html):
 *
 * ----------------------------------------------------------------------------
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the copyright holders nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 *
 * with the extension:
 *
 * You may distribute or publicly perform any modification only under the
 * terms of this license.
 *
 * ----------------------------------------------------------------------------*/

#ifndef _FMIPP_FMI2MODELTYPES_H
#define _FMIPP_FMI2MODELTYPES_H

#define fmi2TypesPlatform "default"

/* Type definitions of variables passed as arguments
   Version "default" means:

   fmi2Component           : an opaque object pointer
   fmi2ComponentEnvironment: an opaque object pointer
   fmi2FMUstate            : an opaque object pointer
   fmi2ValueReference      : handle to the value of a variable
   fmi2Real                : double precision floating-point data type
   fmi2Integer             : basic signed integer data type
   fmi2Boolean             : basic signed integer data type
   fmi2Char                : character data type
   fmi2String              : a pointer to a vector of fmi2Char characters
                             ('\0' terminated, UTF8 encoded)
   fmi2Byte                : smallest addressable unit of the machine, typically one byte.
*/
   typedef void*           fmi2Component;               /* Pointer to FMU instance       */
   typedef void*           fmi2ComponentEnvironment;    /* Pointer to FMU environment    */
   typedef void*           fmi2FMUstate;                /* Pointer to internal FMU state */
   typedef unsigned int    fmi2ValueReference;
   typedef double          fmi2Real   ;
   typedef int             fmi2Integer;
   typedef int             fmi2Boolean;
   typedef char            fmi2Char;
   typedef const fmi2Char* fmi2String;
   typedef char            fmi2Byte;

/* Values for fmi2Boolean  */
#define fmi2True  1
#define fmi2False 0

#define fmi2UndefinedValueReference (fmi2ValueReference)(-1)

/* Type definitions */
typedef struct {
	fmi2Boolean newDiscreteStatesNeeded;
	fmi2Boolean terminateSimulation;
	fmi2Boolean nominalsOfContinuousStatesChanged;
	fmi2Boolean valuesOfContinuousStatesChanged;
	fmi2Boolean nextEventTimeDefined;
	fmi2Real    nextEventTime;
} fmi2EventInfo;

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

#endif // _FMIPP_FMI2MODELTYPES_H
