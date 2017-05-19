/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMU_MODELEXCHANGE_H
#define _FMIPP_FMU_MODELEXCHANGE_H

#include <cstdio>
#include <map>
#include <memory>

#include "import/base/include/FMUModelExchangeBase.h"
#include "import/integrators/include/Integrator.h"


struct BareFMUModelExchange;


/**
 * \file FMUModelExchange_v1.h 
 *
 * Defines the class FMUModelExchange which provides importing and integrating of FMUs for ModelExchange
 *
 * \class FMUModelExchange FMUModelExchange_v1.h
 * Implementation of abstract base class FMUModelExchangeBase.  
 *  
 * The FMI standard requires to define the macro MODEL_IDENTIFIER for each entity of FMU ME
 * seperately. This is not done here, because this class links dynamically during run-time.
 */

namespace fmi_1_0 {

class __FMI_DLL FMUModelExchange : public FMUModelExchangeBase
{

public:

	/**
	 * Constructor.
	 *
	 * @param[in]  fmuPath  path to FMU (as URI)
	 * @param[in]  modelName  model name
	 * @param[in]  stopBeforeEvent  if true, integration stops immediately before an event
	 * @param[in]  eventSearchPrecision  numerical search precision for events during integration
	 * @param[in]  type  integrator type
	 */
	FMUModelExchange( const std::string& fmuPath,
			  const std::string& modelName,
			  const fmiBoolean loggingOn = fmiFalse,
			  const fmiBoolean stopBeforeEvent = fmiFalse,
			  const fmiTime eventSearchPrecision = 1e-4,
#ifdef USE_SUNDIALS
			  const IntegratorType type = IntegratorType::bdf
#else
			  const IntegratorType type = IntegratorType::dp
#endif
			  );

	/**
	 * Constructor.
	 *
	 * @param[in]  xmlPath  path to XML model description (as URI)
	 * @param[in]  dllPath  path to shared library (as URI)
	 * @param[in]  modelName  model name
	 * @param[in]  stopBeforeEvent  if true, integration stops immediately before an event
	 * @param[in]  eventSearchPrecision  numerical search precision for events during integration
	 * @param[in]  type  integrator type
	 */
	FMUModelExchange( const std::string& xmlPath,
			  const std::string& dllPath,
			  const std::string& modelName,
			  const fmiBoolean loggingOn = fmiFalse,
			  const fmiBoolean stopBeforeEvent = fmiFalse,
			  const fmiTime eventSearchPrecision = 1e-4,

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
	virtual fmiStatus initialize();

	/// \copydoc FMUModelExchangeBase::getContinuousStates 
	virtual fmiStatus getContinuousStates( fmiReal* val );

	/// \copydoc FMUModelExchangeBase::setContinuousStates
	virtual fmiStatus setContinuousStates( const fmiReal* val );

	/// \copydoc FMUModelExchangeBase::getDerivatives
	virtual fmiStatus getDerivatives( fmiReal* val );

	/// \copydoc FMUModelExchangeBase::getEventIndicators
	virtual fmiStatus getEventIndicators( fmiReal* eventsind );

	/// \copydoc FMUModelExchangeBase::integrate( fmiTime tend, unsigned int nsteps )
	virtual fmiTime integrate( fmiTime tend, unsigned int nsteps );

	/// \copydoc FMUModelExchangeBase::integrate( fmiTime tend, fmiTime deltaT = 1e-5 )
	virtual fmiTime integrate( fmiTime tend, fmiTime deltaT = 1e-5 );

	/// \copydoc FMUModelExchangeBase::stepOverEvent
	virtual fmiBoolean stepOverEvent();

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
	virtual fmiTime getTimeEvent();

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

	/// \copydoc FMUModelExchangeBase::handleEvents
	virtual void handleEvents();

	/// \copydoc FMUModelExchangeBase::setTime
	virtual void setTime( fmiTime time );

	/// \copydoc FMUModelExchangeBase::rewindTime
	virtual void rewindTime( fmiTime deltaRewindTime );

	/// \copydoc FMUBase::getTime()
	virtual fmiTime getTime() const;

	/// \copydoc FMUBase::getType()
	virtual FMIType getType( const std::string& variableName ) const;

	/// \copydoc FMUBase::getValueRef
	virtual fmiValueReference getValueRef( const std::string& name ) const;

	/// \copydoc FMUBase::getLastStatus
	virtual fmiStatus getLastStatus() const;

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiReal& val )
	virtual fmiStatus getValue( fmiValueReference valref, fmiReal& val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiInteger& val )
	virtual fmiStatus getValue( fmiValueReference valref, fmiInteger& val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiBoolean& val )
	virtual fmiStatus getValue( fmiValueReference valref, fmiBoolean& val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, std::string& val )
	virtual fmiStatus getValue( fmiValueReference valref, std::string& val );

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

	/// @copydoc FMUModelExchangeBase::setCallbacks
	virtual fmiStatus setCallbacks( me::fmiCallbackLogger logger,
					me::fmiCallbackAllocateMemory allocateMemory,
					me::fmiCallbackFreeMemory freeMemory );

	/// Call logger to issue a debug message.
	virtual void sendDebugMessage( const std::string& msg ) const;

        /// Send message to FMU logger.
	void logger( fmiStatus status, const char* category, const char* msg ) const;

	/// Send message to FMU logger.
	void logger( fmiStatus status, const std::string& category, const std::string& msg ) const;

	fmiTime getEventSearchPrecision(){
		return eventSearchPrecision_;
	}

private:

	FMUModelExchange();		///< Prevent calling the default constructor.

	std::string instanceName_;	///< name of the instantiated FMU

	fmiComponent instance_;		///< Internal FMU instance.

	std::shared_ptr<BareFMUModelExchange> fmu_;	///< Internal pointer to bare FMU ME functionalities and model description.

	std::size_t nStateVars_;	///< Number of state variables.
	std::size_t nEventInds_;	///< Number of event indivators.
	std::size_t nValueRefs_;	///< Number of value references.

	/// \FIXME Maps should be handled via ModelManager, to avoid duplication 
	///        of this (potentially large) map with every instance.
	std::map<std::string,fmiValueReference> varMap_;	/// Maps variable names and value references.
	std::map<std::string,FMIType> varTypeMap_;		/// Maps variable names and their types.

	fmiBoolean stopBeforeEvent_;			///< Flag determining internal event handling.

	fmiTime eventSearchPrecision_;			///< Search precision for events.

	fmiReal* intStates_;				///< Internal vector used for integration.
	fmiReal* intDerivatives_;			///< Internal vector used for integration.

	fmiTime time_;					///< Internal time.
	fmiTime tnextevent_;				///< Time of next scheduled event.
	fmiTime lastEventTime_;				///< Time of last event.

	fmiTime tstart_;			///< for determining event times and handling events
	fmiTime tlaststop_;			///< for determining event times and handling events

	fmiEventInfo* eventinfo_;		///< Internal event info.
	fmiReal*      eventsind_;		///< Current event indicators (internally used for event detection).
	fmiReal*      preeventsind_;		///< Previous event indicators (internally used for event detection).

	fmiBoolean stateEvent_; 		///< Internal flag indicationg that a state event has occured.
	fmiBoolean timeEvent_; 			///< Internal flag indicationg that a time event has occured.
	fmiBoolean raisedEvent_;		///< Internal flag indicationg that an event might have occured.
	fmiBoolean eventFlag_;
	fmiBoolean intEventFlag_; 		///< Internal flag indicationg that the integrator has found an event.

	fmiStatus lastStatus_;		        ///< Last status returned from an FMI function.

	fmiBoolean upcomingEvent_;              ///< Internal flag indicationg that a state event has to be
	                                        ///  handled before the next integration ( stopBeforeEvent )


	/**
	 *  Update eventsind_ and preeventsind_ with event indicators from FMU according to
	 *  the current continuous states. Needed to "reset" internal event indicators.
	 */
	fmiStatus resetEventIndicators();

	void readModelDescription();		 ///< Extract specific information from the mode description.

	static const unsigned int maxEventIterations_ = 5; ///< Maximum number of internal event iterations.

	fmiTime tend_;                           ///< in case of an int event, tend_ gives is used as an upper
	                                         ///  limit for the event time
};

} // namespace fmi_1_0

#endif // _FMIPP_FMU_MODELEXCHANGE_H
