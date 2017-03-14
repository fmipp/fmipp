/**
 * \file FMIEventTrigger.c
 * External C-Interface for dynamic models for PowerFactory (function prototypes).
 *
 * \author Edmund Widl
 */
 
#define DESCRIPTION "FMIEventTrigger"
#define VERSION "0.1"
#define CREATED "08.03.2017"
#define AUTHOR "Edmund Widl"
#define COMPANY "AIT Austrian Institute of Technology GmbH"
#define COPYRIGHT "AIT Austrian Institute of Technology GmbH"
#define CHECKSUM "3A3F-8C8F-C2E5-5EEF"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "digusermodel.h"

// Forward function declaration.
bool rmsSimEventQueueIsEmpty();

ModelInfo g_modelInfo = {
	DESCRIPTION,
	VERSION,
	CREATED,
	AUTHOR,
	COMPANY,
	COPYRIGHT,
	CHECKSUM
};

SignalDescription g_outputSignals[1] = {
	{ "trigger", "", "trigger signal for FMI events" }
};

struct ModelDefinition g_modelDefinition = {
	&g_modelInfo,// model information structure
	0,           // no. of input signals
	1,           // no. of output signals
	0,           // no. of state variables
	0,           // no. of limited state variables
	0,           // no. of parameter
	0,           // no. of internal parameter
	0,           // no. of internal signals
	0,           // no. of discrete states
	0,           // no. of delays
	0,           // no. of picdrops
	0,           // no. of selects
	0,           // no. of selfixes
	0,           // no. of limits
	1,           // no. of flipflops
	0,           // no. of look-up arrays
	0,           // no. of look-up tables
	0,           // solver type (not supported)
	0,           // interface type (not supported)
	0,           // prediction type (not supported)
	0.,          // user-defined integration step (not supported)
	NULL,
	g_outputSignals,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

#define ___trigger 0
#define trigger *(pInstance->m_outputSigs[0].m_value)
#define trigger___init(val) *(pInstance->m_outputSigs[0].m_value)=val

// Veto on the trigger.
bool veto_trigger = false;


int Initialise( ModelInstance* pInstance, double tnow )
{
	InitInfo* mInfo___trigger = &( pInstance->m_outputSigs[___trigger].m_initInfo );

	init_output( pInstance, ___trigger, -1. );
	init_flipflop( pInstance, 0 , 0., 1. );

	// Delay any event scheduled at the very beginning to the first integrator step.
	veto_trigger = rmsSimEventQueueIsEmpty() ? false : true;
	
	return 1;
}

int EvaluateEquations( ModelInstance* pInstance, double tnow )
{
	double set = 0.;
	double reset = 1.;
	
	if ( true == veto_trigger )
	{
		veto_trigger = false; // Reset veto on FMI trigger.
	}
	else if ( false == rmsSimEventQueueIsEmpty() )
	{
		set = 1.;
		reset = 0.;
		// print_pf( "sim event queue NOT EMPTY", MSG_INFO );
	}

	trigger = -1. + 2. * eval_flipflop( pInstance, 0 , set, reset );
	
	return 0;
}

int UpdateVariables( ModelInstance* pInstance, double tnow )
{
	return 0;
}

int CheckAndScheduleTransitions( ModelInstance* pInstance, double tnow, double tevent )
{
	return 0;
}

int CheckAndApplyTransitions( ModelInstance* pInstance, double tnow, double tevent )
{
	return 0;
}

int ApplyTransitions( ModelInstance* pInstance, double tevent )
{
	return 0;
}

const struct ModelDefinition* GetModelDefinition()
{
	return &g_modelDefinition;
}

int InitInstance( ModelInstance* pInstance )
{
	pInstance->m_userData = NULL;
	return 0;
}

void ResetInstance( ModelInstance* pInstance )
{
	pInstance->m_userData = NULL;
}

void SetInitProperty( ModelInstance* pInstance )
{
	pInstance->m_outputSigs[___trigger].m_initInfo.m_propinc = 1;
}

int CheckInputs( ModelInstance* pInstance )
{
	return -1;
}

int CheckOutputs( ModelInstance* pInstance )
{
	return -1;
}
