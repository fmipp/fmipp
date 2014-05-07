/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMU_H
#define _FMIPP_FMU_H


#include <map>

#include "FMUBase.h"

struct BareFMUModelExchange;
class FMUIntegrator;


/**
 *  \file FMU.h 
 *  \class FMU FMU.h 
 *   Main functionalities for self integration of FMUs. 
 *  
 *  The FMI standard requires to define the macro MODEL_IDENTIFIER for each
 *  type of FMU seperately. This is not done here, because this class links
 *  dynamically during run-time.
 */


class __FMI_DLL FMU : public FMUBase
{

public:

	FMU( const std::string& fmuPath,
	     const std::string& modelName,
	     fmiBoolean stopBeforEvent = fmiFalse,
	     fmiReal eventSearchPrecision = 1e-4 );

	FMU( const std::string& xmlPath,
	     const std::string& dllPath,
	     const std::string& modelName,
	     fmiBoolean stopBeforEvent = fmiFalse,
	     fmiReal eventSearchPrecision = 1e-4 );

	FMU( const FMU& aFMU );

	~FMU();


	virtual FMIType getType( const std::string& variableName ) const;

	virtual fmiStatus instantiate( const std::string& instanceName,
			       fmiBoolean loggingOn = fmiFalse ); ///< @copydoc FMUBase::instantiate
	virtual fmiStatus initialize();       ///< \copydoc FMUBase::initialize

	virtual fmiReal getTime() const;      ///< \copydoc FMUBase::getTime()
	virtual void setTime( fmiReal time ); ///< \copydoc FMUBase::setTime
	virtual void rewindTime( fmiReal deltaRewindTime ); ///< \copydoc rewindTime

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

	///  \copydoc FMUBase::setValue( const std::string& name,  fmiReal val )
	virtual fmiStatus setValue( const std::string& name, fmiReal val );

	///  \copydoc FMUBase::setValue( const std::string& name,  fmiInteger val )
	virtual fmiStatus setValue( const std::string& name, fmiInteger val );

	///  \copydoc FMUBase::setValue( const std::string& name,  fmiBoolean val )
	virtual fmiStatus setValue( const std::string& name, fmiBoolean val );

	///  \copydoc FMUBase::setValue( const std::string& name,  std::string val )
	virtual fmiStatus setValue( const std::string& name, std::string val );

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

	fmiReal getValue( char* n ) const;

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiInteger& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiInteger& val ) const;

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiBoolean& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiBoolean& val ) const;

	/// \copydoc FMUBase::getValue( const std::string& name,  std::string& val ) const 
	virtual fmiStatus getValue( const std::string& name, std::string& val ) const;

	virtual fmiValueReference getValueRef( const std::string& name ) const; /// \copydoc FMUBase::getValueRef
	virtual fmiStatus getContinuousStates( fmiReal* val ) const; ///< \copydoc FMUBase::getContinuousStates 
	virtual fmiStatus setContinuousStates( const fmiReal* val ); ///< \copydoc FMUBase::setContinuousStates

	virtual fmiStatus getDerivatives( fmiReal* val ) const; ///< \copydoc FMUBase::getDerivatives
	virtual fmiStatus getEventIndicators( fmiReal* eventsind ) const; ///< \copydoc FMUBase::getEventIndicators

	virtual fmiReal integrate( fmiReal tend, unsigned int nsteps );   ///< \copydoc FMUBase::integrate( fmiReal tend, unsigned int nsteps )
	virtual fmiReal integrate( fmiReal tend, double deltaT = 1e-5 );  ///< \copydoc FMUBase::integrate( fmiReal tend, double deltaT = 1e-5 )

	virtual void raiseEvent();  ///< \copydoc FMUBase::raiseEvent
	virtual fmiBoolean checkStateEvent();  ///< \copydoc FMUBase::checkStateEvent
	virtual void handleEvents( fmiTime tstop );  ///< \copydoc FMUBase::handleEvents
	virtual fmiStatus completedIntegratorStep();      ///< \copydoc FMUBase::completedIntegratorStep

	virtual std::size_t nStates() const;        ///< \copydoc FMUBase::nStates
	virtual std::size_t nEventInds() const;     ///< \copydoc FMUBase::nEventInds
	virtual std::size_t nValueRefs() const;     ///< \copydoc FMUBase::nValueRefs 

	fmiBoolean getEventFlag();             ///< \copydoc FMUBase::getEventFlag
	void setEventFlag( fmiBoolean flag );  ///< \copydoc FMUBase::setEventFlag

	fmiBoolean getIntEvent(); ///< \copydoc FMUBase::getIntEvent

	void logger( fmiStatus status, const char* msg ) const;        ///< Send message to FMU logger.	
	void logger( fmiStatus status, const std::string& msg ) const; ///< Send message to FMU logger.
	static void logger( fmiComponent m, fmiString instanceName,
			    fmiStatus status, fmiString category,
			    fmiString message, ... ); ///< Logger function handed to the internal FMU instance.


private:

	FMU(); ///< Prevent calling the default constructor.

	std::string instanceName_;  ///< name of the instantiated FMU 

	fmiComponent instance_; ///< Internal FMU instance.

	BareFMUModelExchange *fmu_; ///< Internal pointer to bare FMU ME functionalities and model description.

	std::size_t nStateVars_; ///< Number of state variables.
	std::size_t nEventInds_; ///< Number of event indivators.
	std::size_t nValueRefs_; ///< Number of value references.

	std::map<std::string,fmiValueReference> varMap_; ///< Maps variable names and value references.
	std::map<std::string,FMIType> varTypeMap_; ///< Maps variable names and their types.

	fmiBoolean stopBeforeEvent_;

	fmiReal eventSearchPrecision_;

	FMUIntegrator* integrator_; ///< Integrator instance.

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

#endif // _FMIPP_FMU_H
