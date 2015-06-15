/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMU_MODEL_EXCHANGE_BASE_H
#define _FMIPP_FMU_MODEL_EXCHANGE_BASE_H


#include "import/base/include/FMUBase.h"
#include "common/fmi_v1.0/fmi_me.h"
#include "import/base/include/DynamicalSystem.h"


/** 
 * \file FMUModelExchangeBase.h
 *
 * \class FMUModelExchangeBase FMUModelExchangeBase.h  Abstract base class for wrappers handling FMUs for ME.
 * 
 * This class defines the main functions that need to be implemented by any class used for
 * handling FMUs CS. This includes ...
 *
 **/


class __FMI_DLL FMUModelExchangeBase : public FMUBase, public DynamicalSystem
{

public:

	/** Constructor.
	 *  @param[in]  loggingOn  turn logging on if true
	 */
        FMUModelExchangeBase( fmiBoolean loggingOn ) : loggingOn_( loggingOn ) {}

	/// Destructor.
        virtual ~FMUModelExchangeBase() {}

	/**
	 * Instantiate the FMU. This function has to be called successfully (i.e., with return
	 * status fmiOK) before any other function is called.
	 * 
	 * @param[in]  instanceName  name of the FMI instance 
	 * @return the status 
	 */
	virtual fmiStatus instantiate( const std::string& instanceName ) = 0;

	/**
	 * Initialize the FMU (after model parameters and start values have been set).
	 * @return the status.
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

	/// When stopBeforeEvent == TRUE, use this function to get the right-sided limit of an event.
	virtual fmiBoolean stepOverEvent() = 0;
	
	/// Raise an event, i.e., notify the FMU ME handle that an event has occured.
	virtual void raiseEvent() = 0;

	/// Check if any kind of event has happened.
	virtual fmiBoolean checkEvents() = 0;
	
	/// Check if a state event happened.
	virtual fmiBoolean checkStateEvent() = 0;

	/// Check if a time event happened.
	virtual fmiBoolean checkTimeEvent() = 0;
	
	/// Handle events. Just call this function if there actually is an event.
	virtual void handleEvents() = 0;

	/// Complete an integration step
	virtual fmiStatus completedIntegratorStep() = 0;

	/// Set event flag explicitely (use with care).
	virtual void setEventFlag( fmiBoolean flag ) = 0;

	/// Get event flag.
	virtual fmiBoolean getEventFlag() = 0;

	/// The integrator needs to check for events that happened during the integration.
	virtual fmiBoolean getIntEvent() = 0;

	/// Get the time of the next time event (infinity if no time event is returned by the FMU):
	virtual fmiReal getTimeEvent() = 0;

	/// Get the number of continuous states
	virtual std::size_t nStates() const = 0;

	/**
	 * Set callback functions of ME FMU. Call before instantiate(...).
	 *
	 * @param[in]  logger  logger function
	 * @param[in]  allocateMemory  memory allocation function
	 * @param[in]  freeMemory  memory de-allocation function
	 */
	virtual fmiStatus setCallbacks( me::fmiCallbackLogger logger,
					me::fmiCallbackAllocateMemory allocateMemory,
					me::fmiCallbackFreeMemory freeMemory ) = 0;

	virtual fmiBoolean callEventUpdate()
	{
		return( callEventUpdate_ );
	};

	virtual fmiReal getEventSearchPrecision() = 0;

	void setIntegratorProperties( Integrator::Properties& properties ){
		integrator_->setProperties( properties );
	}

 protected:

	fmiBoolean callEventUpdate_;  ///< Internal flag indicationg to call an event update.
	const fmiBoolean loggingOn_;

};


#endif // _FMIPP_FMU_MODEL_EXCHANGE_BASE_H
