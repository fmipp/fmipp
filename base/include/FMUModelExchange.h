/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMU_MODELEXCHANGE_H
#define _FMIPP_FMU_MODELEXCHANGE_H


#include <map>

#include "base/include/FMUModelExchangeBase.h"


struct BareFMUModelExchange;
class Integrator;


/**
 * \file FMUModelExchange.h 
 *
 * \class FMUModelExchange FMUModelExchange.h 
 * Implementation of abstract base class FMUModelExchangeBase.  
 *  
 * The FMI standard requires to define the macro MODEL_IDENTIFIER for each entity of FMU ME
 * seperately. This is not done here, because this class links dynamically during run-time.
 */


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
	 */
	FMUModelExchange( const std::string& fmuPath,
			  const std::string& modelName,
			  fmiBoolean stopBeforeEvent = fmiFalse,
			  fmiReal eventSearchPrecision = 1e-4 );

	/**
	 * Constructor.
	 *
	 * @param[in]  xmlPath  path to XML model description (as URI)
	 * @param[in]  dllPath  path to shared library (as URI)
	 * @param[in]  modelName  model name
	 * @param[in]  stopBeforeEvent  if true, integration stops immediately before an event
	 * @param[in]  eventSearchPrecision  numerical search precision for events during integration
	 */
	FMUModelExchange( const std::string& xmlPath,
			  const std::string& dllPath,
			  const std::string& modelName,
			  fmiBoolean stopBeforeEvent = fmiFalse,
			  fmiReal eventSearchPrecision = 1e-4 );

	/// Copy constructor.
	FMUModelExchange( const FMUModelExchange& aFMU );

	/// Destructor.
	virtual ~FMUModelExchange();

	/// @copydoc FMUModelExchangeBase::instantiate
	virtual fmiStatus instantiate( const std::string& instanceName,
				       fmiBoolean loggingOn = fmiFalse );

	/// \copydoc FMUModelExchangeBase::initialize
	virtual fmiStatus initialize();

	/// \copydoc FMUModelExchangeBase::getContinuousStates 
	virtual fmiStatus getContinuousStates( fmiReal* val ) const;

	/// \copydoc FMUModelExchangeBase::setContinuousStates
	virtual fmiStatus setContinuousStates( const fmiReal* val );

	/// \copydoc FMUModelExchangeBase::getDerivatives
	virtual fmiStatus getDerivatives( fmiReal* val ) const;

	/// \copydoc FMUModelExchangeBase::getEventIndicators
	virtual fmiStatus getEventIndicators( fmiReal* eventsind ) const;

	/// \copydoc FMUModelExchangeBase::integrate( fmiReal tend, unsigned int nsteps )
	virtual fmiReal integrate( fmiReal tend, unsigned int nsteps );

	/// \copydoc FMUModelExchangeBase::integrate( fmiReal tend, double deltaT = 1e-5 )
	virtual fmiReal integrate( fmiReal tend, double deltaT = 1e-5 );

	/// \copydoc FMUModelExchangeBase::completedIntegratorStep
	virtual fmiStatus completedIntegratorStep();

	/// \copydoc FMUModelExchangeBase::getEventFlag
	virtual fmiBoolean getEventFlag();

	/// \copydoc FMUModelExchangeBase::setEventFlag
	virtual void setEventFlag( fmiBoolean flag );

	/// \copydoc FMUModelExchangeBase::getIntEvent
	virtual fmiBoolean getIntEvent();

	/// \copydoc FMUModelExchangeBase::raiseEvent
	virtual void raiseEvent();

	/// \copydoc FMUModelExchangeBase::checkStateEvent
	virtual fmiBoolean checkStateEvent();

	/// \copydoc FMUModelExchangeBase::handleEvents
	virtual void handleEvents( fmiTime tstop );


	/// \copydoc FMUModelExchangeBase::setTime
	virtual void setTime( fmiReal time );

	/// \copydoc FMUModelExchangeBase::rewindTime
	virtual void rewindTime( fmiReal deltaRewindTime );

	/// \copydoc FMUBase::getTime()
	virtual fmiReal getTime() const;

	/// \copydoc FMUBase::getType()
	virtual FMIType getType( const std::string& variableName ) const;

	/// \copydoc FMUBase::getValueRef
	virtual fmiValueReference getValueRef( const std::string& name ) const;

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiReal& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, fmiReal& val ) const;

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiInteger& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, fmiInteger& val ) const;

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiBoolean& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, fmiBoolean& val ) const;

	/// \copydoc FMUBase::getValue( fmiValueReference valref, std::string& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, std::string& val ) const;

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const;

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) const;

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) const;

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, std::string* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, std::string* val, std::size_t ival ) const;

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiReal& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiReal& val ) const;

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiInteger& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiInteger& val ) const;

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiBoolean& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiBoolean& val ) const;

	/// \copydoc FMUBase::getValue( const std::string& name,  std::string& val ) const 
	virtual fmiStatus getValue( const std::string& name, std::string& val ) const;

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


	void logger( fmiStatus status, const char* msg ) const;        /// Send message to FMU logger.	
	void logger( fmiStatus status, const std::string& msg ) const; /// Send message to FMU logger.
	static void logger( fmiComponent m, fmiString instanceName,
			    fmiStatus status, fmiString category,
			    fmiString message, ... ); /// Logger function handed to the internal FMU instance.


private:

	FMUModelExchange(); ///< Prevent calling the default constructor.

	std::string instanceName_;  ///< name of the instantiated FMU 

	fmiComponent instance_; ///< Internal FMU instance.

	BareFMUModelExchange *fmu_; ///< Internal pointer to bare FMU ME functionalities and model description.

	std::size_t nStateVars_; ///< Number of state variables.
	std::size_t nEventInds_; ///< Number of event indivators.
	std::size_t nValueRefs_; ///< Number of value references.

	/// \FIXME Maps should be handled via ModelManager, to avoid duplication 
	///        of this (potentially large) map with every instance.
	std::map<std::string,fmiValueReference> varMap_; /// Maps variable names and value references.
	std::map<std::string,FMIType> varTypeMap_; /// Maps variable names and their types.

	fmiBoolean stopBeforeEvent_;

	fmiReal eventSearchPrecision_;

	Integrator* integrator_; ///< Integrator instance.

	fmiReal time_; 
	fmiReal tnextevent_;
	fmiReal lastEventTime_;
	fmiReal lastCompletedIntegratorStepTime_;

	fmiEventInfo* eventinfo_;
	fmiReal*      eventsind_;
	fmiReal*      preeventsind_;

	fmiBoolean callEventUpdate_;
	fmiBoolean stateEvent_;
	fmiBoolean timeEvent_;
	fmiBoolean raisedEvent_;
	fmiBoolean eventFlag_;
	fmiBoolean intEventFlag_;

	/**
	 *  Update eventsind_ and preeventsind_ with event indicators from FMU according to
	 *  the current continuous states. Needed to "reset" internal event indicators.
	 */
	fmiStatus resetEventIndicators();

	void readModelDescription();

	static const unsigned int maxEventIterations_ = 5;

};

#endif // _FMIPP_FMU_MODELEXCHANGE_H
