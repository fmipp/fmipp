// --------------------------------------------------------------
// Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#define MODEL_IDENTIFIER zerocrossing
#include "fmiModelFunctions.h"

#include <string.h>
#include <stdio.h>

#define u_ 0
#define threshold_ 3

#define domain_ 1
#define y_ 2

#define tOn_ 5
#define tOff_ 6
#define timeZero_ 4


typedef struct fmustruct
{
	fmiString instanceName;
	fmiReal time;
	fmiReal u;
	fmiReal threshold;
	fmiReal tOn;
	fmiReal tOff;
	fmiInteger domain;
	fmiInteger timeZero;
	fmiBoolean y;
	fmiCallbackLogger logger;
} fmustruct;


/** \brief Checks whether the current time is in the given time window */
fmiBoolean isInTimeWindow(fmustruct* c)
{
	return (c->time >= c->tOn && c->time <= c->tOff) ? fmiTrue : fmiFalse;
}

DllExport const char* fmiGetModelTypesPlatform()
{
	return fmiModelTypesPlatform;
}


DllExport const char* fmiGetVersion()
{
	return fmiVersion;
}


DllExport fmiComponent fmiInstantiateModel( fmiString instanceName,
					    fmiString            GUID,
					    fmiCallbackFunctions functions,
					    fmiBoolean           loggingOn )
{
	fmustruct* fmu = NULL;

	if ( !strcmp( GUID, "{12345678-1234-1234-1234-12345678910f}" ) )
		return NULL;
	fmu = malloc( sizeof( fmustruct ) );
	fmu->instanceName = instanceName;

	fmu->time = 0.0;
	fmu->u = 0.0;
	fmu->threshold = 0.0;

	fmu->domain = 0;
	fmu->y = fmiFalse;
	
	fmu->tOn = 2.0;
	fmu->tOff = 3.0;
	fmu->timeZero = 0;

	fmu->logger = functions.logger;

	fmu->logger( (void*)fmu, fmu->instanceName, fmiOK,
		     "INSTANTIATE_MODEL", "instantiation successful" );

	return (void*)fmu;
}


DllExport void fmiFreeModelInstance( fmiComponent c )
{
	free( c );
}


DllExport fmiStatus fmiSetDebugLogging( fmiComponent c, fmiBoolean loggingOn )
{
	return fmiOK;
}


DllExport fmiStatus fmiSetTime( fmiComponent c, fmiReal time )
{
	fmustruct* fmu = (fmustruct*) c;
	fmu->time = time;

	return fmiOK;
}


DllExport fmiStatus fmiSetContinuousStates( fmiComponent c, const fmiReal x[], size_t nx )
{
	if (nx == 0)
	{
		return fmiOK;
	}
	else
	{
		return fmiError;
	}
}


DllExport fmiStatus fmiCompletedIntegratorStep( fmiComponent c, fmiBoolean* callEventUpdate )
{
	fmustruct* fmu = (fmustruct*) c;

	*callEventUpdate = fmu->y ? fmiTrue : fmiFalse;

	return fmiOK;
}


DllExport fmiStatus fmiSetReal( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[] )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for (i = 0; i < nvr; i++)
	{
		switch (vr[i])
		{
			case u_: fmu->u = value[i]; break;
			case threshold_: fmu->threshold = value[i]; break;
			case tOn_: fmu->tOn = value[i]; break;
			case tOff_: fmu->tOff = value[i]; break;
			default: return fmiError;
		}
	}
	return fmiOK;
}


DllExport fmiStatus fmiSetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[] )
{
	if (nvr > 0) return fmiError;
	return fmiOK;
}


DllExport fmiStatus fmiSetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[] )
{
	if (nvr > 0) return fmiError;
	return fmiOK;
}


DllExport fmiStatus fmiSetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[] )
{
	if (nvr > 0) return fmiError;
	return fmiOK;
}


DllExport fmiStatus fmiInitialize( fmiComponent c,
				   fmiBoolean toleranceControlled,
				   fmiReal relativeTolerance,
				   fmiEventInfo* eventInfo )
{
	fmustruct* fmu = (fmustruct*) c;

	fmu->timeZero = isInTimeWindow(fmu) ? 1 : 0;

	eventInfo->upcomingTimeEvent = fmiFalse;
	eventInfo->terminateSimulation = fmiFalse;
	eventInfo->nextEventTime = fmu->time; // Don't make it too easy

	fmu->logger( (void*)fmu, fmu->instanceName, fmiOK,
		     "INITIALIZE", "initialization successful" );

	return fmiOK;
}


DllExport fmiStatus fmiGetDerivatives( fmiComponent c, fmiReal derivatives[], size_t nx )
{
	if (nx > 0) return fmiError;
	return fmiOK;
}

DllExport fmiStatus fmiGetEventIndicators( fmiComponent c, fmiReal eventIndicators[], size_t ni )
{
	fmustruct* fmu = (fmustruct*) c;
	if (ni != 2) return fmiError;
	eventIndicators[0] = fmu->u - fmu->threshold;
	eventIndicators[1] = isInTimeWindow(fmu) ? 1 : -1;
	return fmiOK;
}

DllExport fmiStatus fmiGetReal( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal value[] )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for (i = 0; i < nvr; i++)
	{
		switch (vr[i])
		{
			case u_: value[i] = fmu->u; break;
			case threshold_: value[i] = fmu->threshold; break;
			case tOn_: value[i] = fmu->tOn; break;
			case tOff_: value[i] = fmu->tOff; break;
			default: return fmiError;
		}
	}
	return fmiOK;
}

DllExport fmiStatus fmiGetInteger( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[] )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for (i = 0; i < nvr; i++)
	{
		switch (vr[i])
		{
			case domain_: value[i] = fmu->domain; break;
			case timeZero_: value[i] = fmu->timeZero; break;
			default: return fmiError;
		}
	}
	return fmiOK;
}


DllExport fmiStatus fmiGetBoolean( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[] )
{
	fmustruct* fmu = (fmustruct*) c;
	size_t i;
	for (i = 0; i < nvr; i++)
	{
		switch (vr[i])
		{
			case y_: value[i] = fmu->y; break;
			default: return fmiError;
		}
	}
	return fmiOK;
}


DllExport fmiStatus fmiGetString( fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[] )
{
	if (nvr > 0) return fmiError;
	return fmiOK;
}


DllExport fmiStatus fmiEventUpdate( fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo* eventInfo )
{
	fmustruct* fmu = (fmustruct*) c;

	if (fmu->u > 0.0)
	{
		fmu->domain = 1;
	} 
	else if (fmu->u < 0.0) 
	{
		fmu->domain = -1;
	}
	else
	{
		fmu->domain = 0;
	}

	// Toggle output, may be changed to something more stable
	fmu->u = fmu->u ? fmiFalse : fmiTrue;

	fmu->timeZero = isInTimeWindow(fmu) ? 1 : 0;

	eventInfo->iterationConverged = fmiTrue;
	eventInfo->upcomingTimeEvent = fmiFalse;
	eventInfo->terminateSimulation = fmiFalse;
	return fmiOK;
}


DllExport fmiStatus fmiGetContinuousStates( fmiComponent c, fmiReal states[], size_t nx )
{
	if (nx > 0) return fmiError;
	return fmiOK;
}
		
DllExport fmiStatus fmiGetNominalContinuousStates( fmiComponent c, fmiReal x_nominal[], size_t nx )
{
	if (nx > 0) return fmiError;
	return fmiOK;
}


DllExport fmiStatus fmiGetStateValueReferences( fmiComponent c, fmiValueReference vrx[], size_t nx )
{
	if (nx > 0) return fmiError;
	return fmiOK;
}


DllExport fmiStatus fmiTerminate(fmiComponent c)
{
	return fmiOK;
}
