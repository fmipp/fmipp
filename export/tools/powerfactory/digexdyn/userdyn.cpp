/**
 * \file userdyn.cpp 
 * Link to an external event driven C/C++ function for PowerFactory (user-defined functions).
 *  
 * \authors Roman Schwalbe, Matthias Stifter, Edmund Widl
 */

// Standard includes.
#include <stdio.h>
#include <math.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h>

// Project includes.
#include "RmsSimEventQueue.h"

/** 
 * Register user-defined dynamic functions number ifun:
 * return name (cnam) and version number (iVersion)
 * Return !=0 if ifun exceeds the number of available functions.
 */
int __stdcall RegisterFunctions( int ifun, char* cnam, int* iVersion )
{ 
	if ( !cnam || !iVersion ) return 1;

	if ( ifun == 0 ) {
		strcpy( cnam, "FMIEvent" );
		*iVersion = 0;
	} else {
		return 1;
	}

	return 0;
}


/**
 * double tEvent: calling time in seconds (simulation time)
 * double** dParams: parameter array (includes parameters and internal parameters, sorted by name)
 * const double** dOuts: output signals array (interpolated values), values cannot be modified
 * const double** dIntSigs: not used
 * char** eventstr: char*[5]  max. 5 event strings can be defined, max. event string length char[100]
 * char* msg: event message string to output a message in the PowerFactory output window
 * int nParams: number of parameters
 * int nOuts: number of outputs
 * int nIntSigs: not used
 */
void __cdecl FMIEvent( double tEvent,
	double** dParams,
	const double** dOuts,
	const double** dIntSigs,
	char** eventstr,
	char* msg,
	int nParams,
	int nOuts,
	int nIntSigs )
{
	using namespace pf_api;

	std::string eventString;

	for ( size_t i = 0; i < 5 && !( eventString = RmsSimEventQueue::getNextEvent() ).empty(); ++i )
	{	
		sprintf( eventstr[i], eventString.c_str() );
		sprintf( msg, eventString.c_str() );
	}
}
