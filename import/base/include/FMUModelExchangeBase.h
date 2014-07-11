/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMU_MODEL_EXCHANGE_BASE_H
#define _FMIPP_FMU_MODEL_EXCHANGE_BASE_H


#include "import/base/include/FMUBase.h"
#include "common/fmi_v1.0/fmi_me.h"


/** 
 * \file FMUModelExchangeBase.h
 *
 * \class FMUModelExchangeBase FMUModelExchangeBase.h  Abstract base class for wrappers handling FMUs for ME.
 * 
 * This class defines the main functions that need to be implemented by any class used for
 * handling FMUs CS. This includes ...
 *
 **/


class __FMI_DLL FMUModelExchangeBase : public FMUBase
{

public:

	/// Destructor.
        virtual ~FMUModelExchangeBase() {}

	/**
	 * Instantiate the FMU.
	 * 
	 * @param[in]  instanceName  name of the fmi instance 
	 * @param[in]  loggingOn 
	 * @return the instantation status 
	 */
	virtual fmiStatus instantiate( const std::string& instanceName, fmiBoolean loggingOn ) = 0;

	/**
	 * Initialize the FMU (after model parameters and start values have been set).
	 * @return initilization status.
	 */
	virtual fmiStatus initialize() = 0;

	/** 
	 * Set current time.
	 * Set current time affects only the value of the internal FMU time, not the internal state.
	 *
	 * @param[in] time new time point to be set 
	 */
	virtual void setTime( fmiReal time ) = 0;

	/** 
	 * Rewind current time.
	 * This affects only the value of the internal FMU time, not the internal state.
	 *
	 * @param[in] deltaRewindTime amount of time to be set back
	 */
	virtual void rewindTime( fmiReal deltaRewindTime ) = 0;

	/// Get continuous states.
	virtual fmiStatus getContinuousStates( fmiReal* val ) = 0;

	/// Set continuous states.
	virtual fmiStatus setContinuousStates( const fmiReal* val ) = 0;

	/// Get derivatives.
	virtual fmiStatus getDerivatives( fmiReal* val )  = 0;

	/// Get event indicators.
	virtual fmiStatus getEventIndicators( fmiReal* eventsind ) = 0;

	/// Integrate internal state.
	virtual fmiReal integrate( fmiReal tend,
				   unsigned int nsteps ) = 0;

	/// Integrate internal state. 
	virtual fmiReal integrate( fmiReal tend,
				   double deltaT ) = 0;

	/// Raise an event, i.e., notify the FMU ME handle that an event has occured.
	virtual void raiseEvent() = 0;

	/// Check if a state event happened.
	virtual fmiBoolean checkStateEvent() = 0;

	/// Handle events.
	virtual void handleEvents( fmiTime tstop ) = 0;

	/// Complete an integration step
	virtual fmiStatus completedIntegratorStep() = 0;

	/// Set event flag explicitely (use with care).
	virtual void setEventFlag( fmiBoolean flag ) = 0;

	/// Get event flag.
	virtual fmiBoolean getEventFlag() = 0;

	/// The integrator needs to check for events that happened during the integration.
	virtual fmiBoolean getIntEvent() = 0;


	/**
	 * Set callback functions of ME FMU.
	 *
	 * @param[in]  logger  logger function
	 * @param[in]  allocateMemory  memory allocation function
	 * @param[in]  freeMemory  memory de-allocation function
	 */
	virtual void setCallbacks( me::fmiCallbackLogger logger,
				   me::fmiCallbackAllocateMemory allocateMemory,
				   me::fmiCallbackFreeMemory freeMemory ) = 0;
};


#endif // _FMIPP_FMU_MODEL_EXCHANGE_BASE_H
