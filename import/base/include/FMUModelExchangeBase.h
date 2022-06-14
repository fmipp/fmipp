// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMU_MODEL_EXCHANGE_BASE_H
#define _FMIPP_FMU_MODEL_EXCHANGE_BASE_H

#include "import/base/include/FMUBase.h"
#include "import/base/include/DynamicalSystem.h"

#include <assert.h>

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
    FMUModelExchangeBase( fmippBoolean loggingOn ) : loggingOn_( loggingOn ) {}

	/// Destructor.
    virtual ~FMUModelExchangeBase() {}

	/**
	 * Instantiate the FMU. This function has to be called successfully (i.e., with return
	 * status fmiOK) before any other function is called.
	 * 
	 * @param[in]  instanceName  name of the FMI instance 
	 */
	virtual fmippStatus instantiate( const fmippString& instanceName ) = 0;

	/**
	 * Initialize the FMU (after model parameters and start values have been set).
	 */
	virtual fmippStatus initialize( fmippBoolean toleranceDefined, fmippReal tolerance ) = 0;

	/** 
	 * Set current time.
	 * Set current time affects only the value of the internal FMU time, not the internal state.
	 *
	 * @param[in] time new time point to be set 
	 */
	virtual fmippStatus setTime( fmippTime time ) = 0;

	/**
	 * Get the current FMU time
	 *
	 * \returns the current FMU time
	 */
	virtual fmippTime getTime() const = 0;

	/** 
	 * Rewind current time.
	 * This affects only the value of the internal FMU time, not the internal state.
	 *
	 * @param[in] deltaRewindTime amount of time to be set back
	 */
	virtual void rewindTime( fmippTime deltaRewindTime ) = 0;

	/// Get continuous states.
	virtual fmippStatus getContinuousStates( fmippReal* val ) = 0;

	/// Set continuous states.
	virtual fmippStatus setContinuousStates( const fmippReal* val ) = 0;

	/// Get derivatives.
	virtual fmippStatus getDerivatives( fmippReal* val )  = 0;

	/// Get event indicators.
	virtual fmippStatus getEventIndicators( fmippReal* eventsind ) = 0;

	/**
	 * Integrate internal state.
	 *
	 * Integrates the fmu until tend or until the first event. The exact behaviour depends on
	 * the flag stopBeforeEvent_
	 *
	 * \param[in] tend    Stop time for the integration
	 * \param[in] nsteps  Number of integrator steps to be *recommended* to the integrator. Bigger values lead
	 *                    to more accuracy
	 *
	 */
	virtual fmippTime integrate( fmippTime tend, unsigned int nsteps ) = 0;

	/**
	 * Integrate internal state.
	 *
	 * Integrates the fmu until tend or until the first event. The exact behaviour depends on
	 * the flag stopBeforeEvent_
	 *
	 * \param[in] tend    Stop time for the integration
	 * \param[in] deltaT  Starting step size to be used by the integrator. Smaller values lead
	 *                    to more accuracy
	 *
	 */
	virtual fmippTime integrate( fmippTime tend, fmippTime deltaT ) = 0;

	/// When stopBeforeEvent == TRUE, use this function to get the right-sided limit of an event.
	virtual fmippBoolean stepOverEvent() = 0;
	
	/// Raise an event, i.e., notify the FMU ME handle that an event has occured.
	virtual void raiseEvent() = 0;

	/// Check if any kind of event has happened.
	virtual fmippBoolean checkEvents() = 0;
	
	/// Check if a state event happened.
	virtual fmippBoolean checkStateEvent() = 0;

	/// Check if a time event happened.
	virtual fmippBoolean checkTimeEvent() = 0;
	
	/// Handle events. Just call this function if there actually is an event.
	virtual void handleEvents() = 0;

	/// Complete an integration step
	virtual fmippStatus completedIntegratorStep() = 0;

	/// Set event flag explicitely (use with care).
	virtual void setEventFlag( fmippBoolean flag ) = 0;

	/// Get event flag.
	virtual fmippBoolean getEventFlag() = 0;

	/// Reset all internal flags related to event handling.
	virtual void resetEventFlags() = 0;

	/// The integrator needs to check for events that happened during the integration.
	virtual fmippBoolean getIntEvent() = 0;

	/// Get the time of the next time event (infinity if no time event is returned by the FMU):
	virtual fmippTime getTimeEvent() = 0;

	/// Get the number of continuous states
	virtual fmippSize nStates() const = 0;

	/// Get the value of the EventSearchPrecision
	virtual fmippTime getEventSearchPrecision() = 0;

	/// \copydoc Integrator::setProperties
	void setIntegratorProperties( Integrator::Properties& properties ){
		assert( integrator_ );
		integrator_->setProperties( properties );
	}

	/// \copydoc Integrator::getProperties
	Integrator::Properties getIntegratorProperties() const {
		assert( integrator_ );
		return integrator_->getProperties();
	}

 protected:

	const fmippBoolean loggingOn_;

};


#endif // _FMIPP_FMU_MODEL_EXCHANGE_BASE_H
