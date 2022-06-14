// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMU2_MODELEXCHANGE_H
#define _FMIPP_FMU2_MODELEXCHANGE_H

#include <cstdio>
#include <map>
#include <vector>

#include "import/base/include/BareFMU.h"
#include "import/base/include/FMUModelExchangeBase.h"
#include "import/integrators/include/Integrator.h"

struct BareFMU2;

/**
 * \file FMUModelExchange_v2.h
 *
 * \class FMUModelExchange FMUModelExchange_v2.h
 * Implementation of abstract base class FMU2ModelExchangeBase.
 *
 * The FMI standard requires to define the macro MODEL_IDENTIFIER for each entity of FMU2 ME
 * seperately. This is not done here, because this class links dynamically during run-time.
 *
 */

namespace fmi_2_0{

class __FMI_DLL FMUModelExchange : public FMUModelExchangeBase
{

public:

	/**
	 * Constructor. Loads the FMU via the model manager (if needed).
	 *
	 * @param[in]  fmuDirUri             path to unzipped FMU directory (as URI)
	 * @param[in]  modelIdentifier       FMI model identifier
	 * @param[in]  loggingOn             if true, tell the FMU to log all calls to the fmi2XXX functons
	 * @param[in]  stopBeforeEvent       if true, integration stops immediately before an event
	 * @param[in]  eventSearchPrecision  numerical search precision for events during integration
	 * @param[in]  type                  the numerical method for solving ODEs
	 */
	FMUModelExchange( const fmippString& fmuDirUri,
		const fmippString& modelIdentifier,
		const fmippBoolean loggingOn = fmippFalse,
		const fmippBoolean stopBeforeEvent = fmippFalse,
		const fmippTime eventSearchPrecision = 1e-4,
#ifdef USE_SUNDIALS
		const IntegratorType type = IntegratorType::bdf
#else
		const IntegratorType type = IntegratorType::dp
#endif
	);


	/**
	 * Constructor. Requires the FMU to be already loaded (via the model manager).
	 *
	 * @param[in]  modelIdentifier       FMI model identifier.
	 * @param[in]  loggingOn             if true, tell the FMU to log all calls to the fmi2XXX functons
	 * @param[in]  stopBeforeEvent       if true, integration stops immediately before an event
	 * @param[in]  eventSearchPrecision  numerical search precision for events during integration
	 * @param[in]  type                  the numerical method for solving ODEs
	 */
	FMUModelExchange( const fmippString& modelIdentifier,
		const fmippBoolean loggingOn = fmippFalse,
		const fmippBoolean stopBeforeEvent = fmippFalse,
		const fmippTime eventSearchPrecision = 1e-4,
#ifdef USE_SUNDIALS
		const IntegratorType type = IntegratorType::bdf
#else
		const IntegratorType type = IntegratorType::dp
#endif
	);


	/// Copy constructor.
	FMUModelExchange( const FMUModelExchange& aFMU );

	/// Destructor.
	virtual ~FMUModelExchange();

	/// @copydoc FMUModelExchangeBase::instantiate
	virtual fmippStatus instantiate( const fmippString& instanceName );

	/// \copydoc FMUModelExchangeBase::initialize
	virtual fmippStatus initialize( fmippBoolean toleranceDefined = false, fmippReal tolerance = 1e-5 );

	/// \copydoc FMUModelExchangeBase::getContinuousStates
	virtual fmippStatus getContinuousStates( fmippReal* val );

	/// \copydoc FMUModelExchangeBase::setContinuousStates
	virtual fmippStatus setContinuousStates( const fmippReal* val );

	/// \copydoc FMUModelExchangeBase::getDerivatives
	virtual fmippStatus getDerivatives( fmippReal* val );

	/// Retrieve vector containing the value references of the derivatives vector elements.
	virtual std::vector<fmippValueReference> getDerivativesRefs() const;

	/// Retrieve vector containing the names of the derivatives vector elements.
	virtual std::vector<fmippString> getDerivativesNames() const;

	/// \copydoc DynamicalSystem::getJac( fmippReal* J )
	virtual	fmippStatus getJac( fmippReal* J );

	/// \copydoc FMUModelExchangeBase::getEventIndicators
	virtual fmippStatus getEventIndicators( fmippReal* eventsind );

	/// \copydoc FMUModelExchangeBase::integrate( fmippTime tend, unsigned int nsteps )
	virtual fmippTime integrate( fmippTime tend, unsigned int nsteps );

	/// \copydoc FMUModelExchangeBase::integrate( fmippReal tend, fmippTime deltaT = 1e-5 )
	virtual fmippTime integrate( fmippTime tend, fmippTime deltaT = 1e-5 );

	/**
	 * Handle events
	 *
	 * make event iterations ( using newDiscreteStates ) until convergence has been achieved.
	 */
	void handleEvents();

	/// \copydoc FMUModelExchangeBase::completedIntegratorStep
	virtual fmippStatus completedIntegratorStep();

	/// \copydoc FMUModelExchangeBase::getEventFlag
	virtual fmippBoolean getEventFlag();

	/// \copydoc FMUModelExchangeBase::setEventFlag
	virtual void setEventFlag( fmippBoolean flag );

	/// \copydoc FMUModelExchangeBase::resetEventFlag
	virtual void resetEventFlags();

	/// \copydoc FMUModelExchangeBase::getIntEvent
	virtual fmippBoolean getIntEvent();

	/// \copydoc FMUModelExchangeBase::getTimeEvent
	virtual fmippReal getTimeEvent();

	/// \copydoc FMUModelExchangeBase::raiseEvent
	virtual void raiseEvent();

	/// \copydoc FMUModelExchangeBase::checkEvents
	virtual fmippBoolean checkEvents();

	/// \copydoc FMUModelExchangeBase::checkStateEvent
	virtual fmippBoolean checkStateEvent();

	/// \copydoc FMUModelExchangeBase::checkTimeEvent
	virtual fmippBoolean checkTimeEvent();

	/// \copydoc DynamicalSystem::checkStepEvent
	fmippBoolean checkStepEvent();

	/// \copydoc FMUModelExchangeBase::setTime
	virtual fmippStatus setTime( fmippReal time );

	/// \copydoc FMUModelExchangeBase::rewindTime
	virtual void rewindTime( fmippTime deltaRewindTime );

	/// \copydoc FMUBase::getTime()
	virtual fmippTime getTime() const;

	/// \copydoc FMUBase::getType()
	virtual FMIPPVariableType getType( const fmippString& variableName ) const;

	/// \copydoc FMUBase::getValueRef
	virtual fmippValueReference getValueRef( const fmippString& name ) const;

	/// \copydoc FMUBase::getLastStatus
	virtual fmippStatus getLastStatus() const;

	/// \copydoc FMUBase::getValue( fmippValueReference valref, fmippReal& val )
	virtual fmippStatus getValue( fmippValueReference valref, fmippReal& val );

	/// \copydoc FMUBase::getValue( fmippValueReference valref, fmippInteger& val )
	virtual fmippStatus getValue( fmippValueReference valref, fmippInteger& val );

	/// \copydoc FMUBase::getValue( fmippValueReference valref, fmippBoolean& val )
	virtual fmippStatus getValue( fmippValueReference valref, fmippBoolean& val );

	/// \copydoc FMUBase::getValue( fmippValueReference valref, fmippString& val )
	virtual fmippStatus getValue( fmippValueReference valref, fmippString& val );

	/// \copydoc FMUBase::getValue( fmippValueReference* valref, fmippReal* val, fmippSize ival )
	virtual fmippStatus getValue( fmippValueReference* valref, fmippReal* val, fmippSize ival );

	/// \copydoc FMUBase::getValue( fmippValueReference* valref, fmippInteger* val, fmippSize ival )
	virtual fmippStatus getValue( fmippValueReference* valref, fmippInteger* val, fmippSize ival );

	/// \copydoc FMUBase::getValue( fmippValueReference* valref, fmippBoolean* val, fmippSize ival )
	virtual fmippStatus getValue( fmippValueReference* valref, fmippBoolean* val, fmippSize ival );

	/// \copydoc FMUBase::getValue( fmippValueReference* valref, fmippString* val, fmippSize ival )
	virtual fmippStatus getValue( fmippValueReference* valref, fmippString* val, fmippSize ival );

	/// \copydoc FMUBase::getValue( const fmippString& name,  fmippReal& val )
	virtual fmippStatus getValue( const fmippString& name, fmippReal& val );

	/// \copydoc FMUBase::getValue( const fmippString& name,  fmippInteger& val )
	virtual fmippStatus getValue( const fmippString& name, fmippInteger& val );

	/// \copydoc FMUBase::getValue( const fmippString& name,  fmippBoolean& val )
	virtual fmippStatus getValue( const fmippString& name, fmippBoolean& val );

	/// \copydoc FMUBase::getValue( const fmippString& name,  fmippString& val )
	virtual fmippStatus getValue( const fmippString& name, fmippString& val );

	/// \copydoc FMUBase::getRealValue( const fmippString& name )
	virtual fmippReal getRealValue( const fmippString& name );

	/// \copydoc FMUBase::getIntegerValue( const fmippString& name )
	virtual fmippInteger getIntegerValue( const fmippString& name );

	/// \copydoc FMUBase::getBooleanValue( const fmippString& name )
	virtual fmippBoolean getBooleanValue( const fmippString& name );

	/// \copydoc FMUBase::getStringValue( const fmippString& name )
	virtual fmippString getStringValue( const fmippString& name );

	/// \copydoc FMUBase::setValue( fmippValueReference valref, const fmippReal& val )
	virtual fmippStatus setValue( fmippValueReference valref, const fmippReal& val );

	/// \copydoc FMUBase::setValue( fmippValueReference valref, const fmippInteger& val )
	virtual fmippStatus setValue( fmippValueReference valref, const fmippInteger& val );

	/// \copydoc FMUBase::setValue( fmippValueReference valref, const fmippBoolean& val )
	virtual fmippStatus setValue( fmippValueReference valref, const fmippBoolean& val );

	/// \copydoc FMUBase::setValue( fmippValueReference valref, const fmippString& val )
	virtual fmippStatus setValue( fmippValueReference valref, const fmippString& val );

	/// \copydoc FMUBase::setValue( fmippValueReference* valref, const fmippReal* val, fmippSize ival )
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippReal* val, fmippSize ival );

	/// \copydoc FMUBase::setValue( fmippValueReference* valref, const fmippInteger* val, fmippSize ival )
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippInteger* val, fmippSize ival );

	/// \copydoc FMUBase::setValue( fmippValueReference* valref, const fmippBoolean* val, fmippSize ival )
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippBoolean* val, fmippSize ival );

	/// \copydoc FMUBase::setValue( fmippValueReference* valref, const fmippString* val, fmippSize ival )
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippString* val, fmippSize ival );

	/// \copydoc FMUBase::setValue( const fmippString& name, const fmippReal& val )
	virtual fmippStatus setValue( const fmippString& name, const fmippReal& val );

	/// \copydoc FMUBase::setValue( const fmippString& name, const fmippInteger& val )
	virtual fmippStatus setValue( const fmippString& name, const fmippInteger& val );

	/// \copydoc FMUBase::setValue( const fmippString& name, const fmippBoolean& val )
	virtual fmippStatus setValue( const fmippString& name, const fmippBoolean& val );

	/// \copydoc FMUBase::setValue( const fmippString& name, const fmippString& val )
	virtual fmippStatus setValue( const fmippString& name, const fmippString& val );

	/// \copydoc FMUBase::nStates
	virtual fmippSize nStates() const;

	/// Retrieve vector containing the value references of the state vector elements.
	virtual std::vector<fmippValueReference> getStatesRefs() const;

	/// Retrieve vector containing the names of the state vector elements.
	virtual std::vector<fmippString> getStatesNames() const;

	/// \copydoc FMUBase::nEventInds
	virtual fmippSize nEventInds() const;

	/// \copydoc FMUBase::nValueRefs
	virtual fmippSize nValueRefs() const;

	/// \copydoc FMUBase::getModelDescription
	virtual const ModelDescription* getModelDescription() const;

	/// Call logger to issue a debug message.
	virtual void sendDebugMessage( const fmippString& msg ) const;

        /// Send message to FMU logger.
	void logger( fmi2Status status, const fmippChar* category, const fmippChar* msg ) const;

	/// Send message to FMU logger.
	void logger( fmi2Status status, const fmippString& category, const fmippString& msg ) const;

	/// \copydoc FMUModelExchangeBase::getEventSearchPrecision()
	virtual fmippReal getEventSearchPrecision(){
		return eventSearchPrecision_;
	}

	/************ Unique functions for FMI 2.0 ************/

	/// Set callback functions.
	virtual fmippStatus setCallbacks( fmi2::fmi2CallbackLogger logger,
		fmi2::fmi2CallbackAllocateMemory allocateMemory,
		fmi2::fmi2CallbackFreeMemory freeMemory );

	/// Set component environment.
	virtual void setComponentEnvironment( fmi2ComponentEnvironment env ) {
		callbacks_.componentEnvironment = env;
	}

	fmippBoolean stepOverEvent(); ///< Make a step from tLower_ to tUpper_ using explicit euler here, tLower and tUpper are provided by the Integrator.

private:

	fmi2Status enterContinuousTimeMode(); ///< Change the mode of the FMU to ContinuousTimeMode.
	fmi2Status enterEventMode(); ///< Change the mode of the FMU to EventMode.

	FMUModelExchange(); ///< Prevent calling the default constructor.

	fmippString instanceName_; ///< name of the instantiated FMU

	fmi2Component instance_; ///< Internal FMU instance.

	BareFMU2Ptr fmu_; ///< Internal pointer to bare FMU ME2 functionalities and model description.

	fmi2::fmi2CallbackFunctions callbacks_; ///< Internal struct to callback functions.

	fmippSize nStateVars_; ///< Number of state variables.
	fmippSize nEventInds_; ///< Number of event indivators.
	fmippSize nValueRefs_; ///< Number of value references.

	fmippValueReference* derivatives_refs_; ///< Vector containing the value references of all derivatives
	fmippValueReference* states_refs_; ///< Vector containing the value references of all states

	/// \FIXME Maps should be handled via ModelManager, to avoid duplication
	///        of this (potentially large) map with every instance.
	std::map<fmippString,fmippValueReference> varMap_; ///< Maps variable names and value references.
	std::map<fmippString,FMIPPVariableType> varTypeMap_; ///< Maps variable names and their types.

	fmi2Boolean stopBeforeEvent_; ///< Flag determining internal event handling.

	fmippTime eventSearchPrecision_; ///< Search precision for events.

	fmi2Real* intStates_; ///< Internal vector used for integration.
	fmi2Real* intDerivatives_; ///< Internal vector used for integration.

	fmippTime time_; ///< Internal time.
	fmippTime tnextevent_; ///< Time of next scheduled event.
	fmippTime lastEventTime_; ///< Time of last event.

	fmi2EventInfo* eventinfo_; ///< Internal event info.
	fmi2Real* eventsind_; ///< Current event indicators (internally used for event detection).
	fmi2Real* preeventsind_; ///< Previous event indicators (internally used for event detection).

	fmi2Boolean callEventUpdate_; ///< Internal flag indicationg to call an event update.
	fmi2Boolean stateEvent_; ///< Internal flag indicationg that a state event has occured.
	fmi2Boolean timeEvent_; ///< Internal flag indicationg that a time event has occured.

	fmi2Boolean enterEventMode_; ///< gets activated when a step Event Happens
	fmi2Boolean terminateSimulation_; ///< can be fired by handleEvents2 and completedIntegratorStep termination is not performed so far
	fmi2Boolean upcomingEvent_; ///< in integrate: did the last call of integrate end with a stateEvent? only relevant if ( stopbeforeEvent == true )

	fmi2Boolean raisedEvent_; ///< Internal flag indicationg that an event might have occured.
	fmi2Boolean eventFlag_; ///< Internal flac indicating that any kind of event might have occured.
	fmi2Boolean intEventFlag_; ///< Internal flag indicationg that the integrator has found an event.

	fmi2Status lastStatus_; ///< Last status returned from an FMI function.

	void readModelDescription(); ///< Extract specific information from the mode description.

	static const fmippSize maxEventIterations_ = 5; ///< Maximum number of internal event iterations.

	/// upper limit for the next event time
	fmippTime tend_; ///< in case of an int event, tend_ gives is used as an upper limit for the event time

};

} // namespace fmi_2_0

#endif // _FMIPP_FMU2_MODELEXCHANGE_H
