// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMU2_MODELEXCHANGE_H
#define _FMIPP_FMU2_MODELEXCHANGE_H

#include <cstdio>
#include <map>

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
	FMUModelExchange( const std::string& fmuDirUri,
		const std::string& modelIdentifier,
		const fmi2Boolean loggingOn = fmi2False,
		const bool stopBeforeEvent = fmi2False,
		const fmi2Time eventSearchPrecision = 1e-4,
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
	FMUModelExchange( const std::string& modelIdentifier,
		const bool loggingOn = fmi2False,
		const bool stopBeforeEvent = fmi2False,
		const fmi2Time eventSearchPrecision = 1e-4,
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
	virtual fmiStatus instantiate( const std::string& instanceName );

	/// \copydoc FMUModelExchangeBase::initialize
	virtual fmiStatus initialize( bool toleranceDefined = false, double tolerance = 1e-5 );

	/// \copydoc FMUModelExchangeBase::getContinuousStates
	virtual fmiStatus getContinuousStates( fmiReal* val );

	/// \copydoc FMUModelExchangeBase::setContinuousStates
	virtual fmiStatus setContinuousStates( const fmiReal* val );

	/// \copydoc FMUModelExchangeBase::getDerivatives
	virtual fmiStatus getDerivatives( fmiReal* val );

	/// \copydoc DynamicalSystem::getJac( fmiReal* J )
	virtual	fmiStatus getJac( fmiReal* J );

	/// \copydoc FMUModelExchangeBase::getEventIndicators
	virtual fmiStatus getEventIndicators( fmiReal* eventsind );

	/// \copydoc FMUModelExchangeBase::integrate( fmiTime tend, unsigned int nsteps )
	virtual fmiTime integrate( fmiTime tend, unsigned int nsteps );

	/// \copydoc FMUModelExchangeBase::integrate( fmiReal tend, fmiTime deltaT = 1e-5 )
	virtual fmiTime integrate( fmiTime tend, fmiTime deltaT = 1e-5 );

	/**
	 * Handle events
	 *
	 * make event iterations ( using newDiscreteStates ) until convergence has been archieved.
	 */
	void handleEvents();

	/// \copydoc FMUModelExchangeBase::completedIntegratorStep
	virtual fmiStatus completedIntegratorStep();

	/// \copydoc FMUModelExchangeBase::getEventFlag
	virtual fmiBoolean getEventFlag();

	/// \copydoc FMUModelExchangeBase::setEventFlag
	virtual void setEventFlag( fmiBoolean flag );

	/// \copydoc FMUModelExchangeBase::resetEventFlag
	virtual void resetEventFlags();

	/// \copydoc FMUModelExchangeBase::getIntEvent
	virtual fmiBoolean getIntEvent();

	/// \copydoc FMUModelExchangeBase::getTimeEvent
	virtual fmiReal getTimeEvent();

	/// \copydoc FMUModelExchangeBase::raiseEvent
	virtual void raiseEvent();

	/// \copydoc FMUModelExchangeBase::checkEvents
	virtual fmiBoolean checkEvents();

	/// \copydoc FMUModelExchangeBase::checkStateEvent
	virtual fmiBoolean checkStateEvent();

	/// \copydoc FMUModelExchangeBase::checkTimeEvent
	virtual fmiBoolean checkTimeEvent();

	/// \copydoc DynamicalSystem::checkStepEvent
	bool checkStepEvent();

	/// \copydoc FMUModelExchangeBase::setTime
	virtual void setTime( fmiReal time );

	/// \copydoc FMUModelExchangeBase::rewindTime
	virtual void rewindTime( fmiTime deltaRewindTime );

	/// \copydoc FMUBase::getTime()
	virtual fmi2Time getTime() const;

	/// \copydoc FMUBase::getType()
	virtual FMIVariableType getType( const std::string& variableName ) const;

	/// \copydoc FMUBase::getValueRef
	virtual fmi2ValueReference getValueRef( const std::string& name ) const;

	/// \copydoc FMUBase::getLastStatus
	virtual fmiStatus getLastStatus() const;

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiReal& val )
	virtual fmiStatus getValue( fmiValueReference valref, fmiReal& val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiInteger& val )
	virtual fmiStatus getValue( fmi2ValueReference valref, fmi2Integer& val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiBoolean& val )
	virtual fmiStatus getValue( fmi2ValueReference valref, fmiBoolean& val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, std::string& val )
	virtual fmiStatus getValue( fmi2ValueReference valref, std::string& val );

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival )
	virtual fmiStatus getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival );

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival )
	virtual fmiStatus getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival );

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival )
	virtual fmiStatus getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival );

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, std::string* val, std::size_t ival )
	virtual fmiStatus getValue( fmiValueReference* valref, std::string* val, std::size_t ival );

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiReal& val )
	virtual fmiStatus getValue( const std::string& name, fmiReal& val );

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiInteger& val )
	virtual fmiStatus getValue( const std::string& name, fmiInteger& val );

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiBoolean& val )
	virtual fmiStatus getValue( const std::string& name, fmiBoolean& val );

	/// \copydoc FMUBase::getValue( const std::string& name,  std::string& val )
	virtual fmiStatus getValue( const std::string& name, std::string& val );

	/// \copydoc FMUBase::getRealValue( const std::string& name )
	virtual fmiReal getRealValue( const std::string& name );

	/// \copydoc FMUBase::getIntegerValue( const std::string& name )
	virtual fmiInteger getIntegerValue( const std::string& name );

	/// \copydoc FMUBase::getBooleanValue( const std::string& name )
	virtual fmiBoolean getBooleanValue( const std::string& name );

	/// \copydoc FMUBase::getStringValue( const std::string& name )
	virtual fmiString getStringValue( const std::string& name );

	/// \copydoc FMUBase::setValue( fmiValueReference valref, fmiReal& val )
	virtual fmiStatus setValue( fmiValueReference valref, fmiReal& val );

	/// \copydoc FMUBase::setValue( fmiValueReference valref, fmiInteger& val )
	virtual fmiStatus setValue( fmiValueReference valref, fmiInteger& val );

	/// \copydoc FMUBase::setValue( fmiValueReference valref, fmiBoolean& val )
	virtual fmiStatus setValue( fmiValueReference valref, fmiBoolean& val );

	/// \copydoc FMUBase::setValue( fmiValueReference valref, std::string& val )
	virtual fmiStatus setValue( fmiValueReference valref, std::string& val );

	/// \copydoc FMUBase::setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival );

	/// \copydoc FMUBase::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival );

	/// \copydoc FMUBase::setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival );

	/// \copydoc FMUBase::setValue( fmiValueReference* valref, std::string* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, std::string* val, std::size_t ival );

	/// \copydoc FMUBase::setValue( const std::string& name,  fmiReal val )
	virtual fmiStatus setValue( const std::string& name, fmiReal val );

	/// \copydoc FMUBase::setValue( const std::string& name,  fmiInteger val )
	virtual fmiStatus setValue( const std::string& name, fmiInteger val );

	/// \copydoc FMUBase::setValue( const std::string& name,  fmiBoolean val )
	virtual fmiStatus setValue( const std::string& name, fmiBoolean val );

	/// \copydoc FMUBase::setValue( const std::string& name,  std::string val )
	virtual fmiStatus setValue( const std::string& name, std::string val );

	/// \copydoc FMUBase::nStates
	virtual std::size_t nStates() const;

	/// \copydoc FMUBase::nEventInds
	virtual std::size_t nEventInds() const;

	/// \copydoc FMUBase::nValueRefs
	virtual std::size_t nValueRefs() const;

	/// \copydoc FMUBase::getModelDescription
	virtual const ModelDescription* getModelDescription() const;

	/// Call logger to issue a debug message.
	virtual void sendDebugMessage( const std::string& msg ) const;

        /// Send message to FMU logger.
	void logger( fmi2Status status, const char* category, const char* msg ) const;

	/// Send message to FMU logger.
	void logger( fmi2Status status, const std::string& category, const std::string& msg ) const;

	/// \copydoc FMUModelExchangeBase::getEventSearchPrecision()
	fmiReal getEventSearchPrecision(){
		return eventSearchPrecision_;
	}

	/************ Unique functions for FMI 2.0 ************/

	/// Set callback functions.
	virtual fmiStatus setCallbacks( fmi2::fmi2CallbackLogger logger,
		fmi2::fmi2CallbackAllocateMemory allocateMemory,
		fmi2::fmi2CallbackFreeMemory freeMemory );

	/// Set component environment.
	virtual void setComponentEnvironment( fmi2ComponentEnvironment env ) {
		callbacks_.componentEnvironment = env;
	}

	void enterContinuousTimeMode(); ///< Change the mode of the FMU to continuousTimeMode.

	fmiBoolean stepOverEvent(); ///< Make a step from tLower_ to tUpper_ using explicit euler here, tLower and tUpper are provided by the Integrator.

private:

	FMUModelExchange(); ///< Prevent calling the default constructor.

	std::string instanceName_; ///< name of the instantiated FMU

	fmi2Component instance_; ///< Internal FMU instance.

	BareFMU2Ptr fmu_; ///< Internal pointer to bare FMU ME2 functionalities and model description.

	fmi2::fmi2CallbackFunctions callbacks_; ///< Internal struct to callback functions.

	std::size_t nStateVars_; ///< Number of state variables.
	std::size_t nEventInds_; ///< Number of event indivators.
	std::size_t nValueRefs_; ///< Number of value references.

	fmi2ValueReference* derivatives_refs_; ///< Vector containing the value references of all derivatives
	fmi2ValueReference* states_refs_; ///< Vector containing the value references of all states

	/// \FIXME Maps should be handled via ModelManager, to avoid duplication
	///        of this (potentially large) map with every instance.
	std::map<std::string,fmi2ValueReference> varMap_; ///< Maps variable names and value references.
	std::map<std::string,FMIVariableType> varTypeMap_; ///< Maps variable names and their types.

	bool stopBeforeEvent_; ///< Flag determining internal event handling.

	fmi2Time eventSearchPrecision_; ///< Search precision for events.

	fmi2Real* intStates_; ///< Internal vector used for integration.
	fmi2Real* intDerivatives_; ///< Internal vector used for integration.

	fmi2Time time_; ///< Internal time.
	fmi2Time tnextevent_; ///< Time of next scheduled event.
	fmi2Time lastEventTime_; ///< Time of last event.

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

	static const unsigned int maxEventIterations_ = 5; ///< Maximum number of internal event iterations.

	/// upper limit for the next event time
	fmi2Time tend_; ///< in case of an int event, tend_ gives is used as an upper limit for the event time

};

} // namespace fmi_2_0

#endif // _FMIPP_FMU2_MODELEXCHANGE_H
