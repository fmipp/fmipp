/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file FMUModelExchange.cpp
 */
#include <iostream> /// \FIXME remove
#include <cassert>
#include <limits>

#if defined( WIN32 ) // Windows.
#include <algorithm>
#define fmin min
#else // Linux, Unix, etc.
#include <cmath>
#endif

#include "common/FMIPPConfig.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/fmi_v1.0/fmi_me.h"

#include "import/base/include/FMUModelExchange.h"
#include "import/base/include/ModelManager.h"


using namespace std;


FMUModelExchange::FMUModelExchange( const string& fmuPath,
				    const string& modelName,
				    const fmiBoolean stopBeforeEvent,
				    const fmiReal eventSearchPrecision,
				    const IntegratorType type ) :
	instance_( 0 ),
	nStateVars_( numeric_limits<size_t>::quiet_NaN() ),
	nEventInds_( numeric_limits<size_t>::quiet_NaN() ),
	nValueRefs_( numeric_limits<size_t>::quiet_NaN() ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision ),
	integrator_( 0 ),
	intStates_( 0 ),
	intDerivatives_( 0 ),
	time_( numeric_limits<fmiReal>::quiet_NaN() ),
	tnextevent_( numeric_limits<fmiReal>::quiet_NaN() ),
	lastEventTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	lastCompletedIntegratorStepTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	firstFailedIntegratorStepTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	eventinfo_( 0 ),
	eventsind_( 0 ),
	preeventsind_( 0 ),
	stateEvent_( fmiFalse ),
	timeEvent_( fmiFalse ),
	raisedEvent_( fmiFalse ),
	eventFlag_( fmiFalse ),
	intEventFlag_( fmiFalse ),
	lastStatus_( fmiOK )
{
	ModelManager& manager = ModelManager::getModelManager();
	fmu_ = manager.getModel( fmuPath, modelName );
	if ( 0 != fmu_ ) {
		readModelDescription();
		integrator_ = new Integrator( this, type );
	}
}


FMUModelExchange::FMUModelExchange( const string& xmlPath,
				    const string& dllPath,
				    const string& modelName,
				    const fmiBoolean stopBeforeEvent,
				    const fmiReal eventSearchPrecision,
				    const IntegratorType type ) :
	instance_( 0 ),
	nStateVars_( numeric_limits<size_t>::quiet_NaN() ),
	nEventInds_( numeric_limits<size_t>::quiet_NaN() ),
	nValueRefs_( numeric_limits<size_t>::quiet_NaN() ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision ),
	integrator_( 0 ),
	intStates_( 0 ),
	intDerivatives_( 0 ),
	time_( numeric_limits<fmiReal>::quiet_NaN() ),
	tnextevent_( numeric_limits<fmiReal>::quiet_NaN() ),
	lastEventTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	lastCompletedIntegratorStepTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	eventinfo_( 0 ),
	eventsind_( 0 ),
	preeventsind_( 0 ),
	stateEvent_( fmiFalse ),
	timeEvent_( fmiFalse ),
	raisedEvent_( fmiFalse ),
	eventFlag_( fmiFalse ),
	intEventFlag_( fmiFalse ),
	lastStatus_( fmiOK )
{
	ModelManager& manager = ModelManager::getModelManager();
	fmu_ = manager.getModel( xmlPath, dllPath, modelName );
	if ( 0 != fmu_ ) {
		readModelDescription();
		integrator_ = new Integrator( this, type );
	}
}


FMUModelExchange::FMUModelExchange( const FMUModelExchange& aFMU ) :
	instance_( 0 ),
	fmu_( aFMU.fmu_ ),
	nStateVars_( aFMU.nStateVars_ ),
	nEventInds_( aFMU.nEventInds_ ),
	nValueRefs_( aFMU.nValueRefs_ ),
	varMap_( aFMU.varMap_ ),
	varTypeMap_( aFMU.varTypeMap_ ),
	stopBeforeEvent_( aFMU.stopBeforeEvent_ ),
	eventSearchPrecision_( aFMU.eventSearchPrecision_ ),
	integrator_( 0 ),
	intStates_( 0 ),
	intDerivatives_( 0 ),
	time_( numeric_limits<fmiReal>::quiet_NaN() ),
	tnextevent_( numeric_limits<fmiReal>::quiet_NaN() ),
	lastEventTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	lastCompletedIntegratorStepTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	eventinfo_( 0 ),
	eventsind_( 0 ),
	preeventsind_( 0 ),
	stateEvent_( fmiFalse ),
	timeEvent_( fmiFalse ),
	raisedEvent_( fmiFalse ),
	eventFlag_( fmiFalse ),
	intEventFlag_( fmiFalse ),
	lastStatus_( fmiOK )
{
	if ( 0 != fmu_ ) integrator_ = new Integrator( this, aFMU.integrator_->type() );
}


FMUModelExchange::~FMUModelExchange()
{
	if ( integrator_ ) delete integrator_;

	if ( instance_ ) {

		if ( eventsind_ ) delete[] eventsind_;
		if ( preeventsind_ ) delete[] preeventsind_;

		delete eventinfo_;

		if ( intStates_ ) delete[] intStates_;
		if ( intDerivatives_ ) delete[] intDerivatives_;

		fmu_->functions->terminate( instance_ );
#ifndef MINGW
		/// \FIXME This call causes a seg fault with OpenModelica FMUs under MINGW ...
		fmu_->functions->freeModelInstance( instance_ );
#endif
	}
}


void FMUModelExchange::readModelDescription()
{
	using namespace ModelDescriptionUtilities;
	typedef ModelDescription::Properties Properties;

	const ModelDescription* description = fmu_->description;

	nStateVars_ = description->getNumberOfContinuousStates();
	nEventInds_ = description->getNumberOfEventIndicators();

	const Properties& modelVariables = description->getModelVariables();

	Properties::const_iterator itVar = modelVariables.begin();
	Properties::const_iterator itEnd = modelVariables.end();

	for ( ; itVar != itEnd; ++itVar )
	{
		const Properties& varAttributes = getAttributes( itVar );

		string varName = varAttributes.get<string>( "name" );
		fmiValueReference varValRef = varAttributes.get<int>( "valueReference" );

		// Map name to value reference.
		varMap_.insert( make_pair( varName, varValRef ) );

		// Map name to value type.
		if ( hasChild( itVar, "Real" ) ) {
			varTypeMap_.insert( make_pair( varName, fmiTypeReal ) );
		} else if ( hasChild( itVar, "Integer" ) ) {
			varTypeMap_.insert( make_pair( varName, fmiTypeInteger ) );
		} else if ( hasChild( itVar, "Boolean" ) ) {
			varTypeMap_.insert( make_pair( varName, fmiTypeBoolean ) );
		} else if ( hasChild( itVar, "String" ) ) {
			varTypeMap_.insert( make_pair( varName, fmiTypeString ) );
		} else {
			varTypeMap_.insert( make_pair( varName, fmiTypeUnknown ) );
		}
	}

	nValueRefs_ = varMap_.size();
}


FMIType FMUModelExchange::getType( const string& variableName ) const
{
	map<string,FMIType>::const_iterator it = varTypeMap_.find( variableName );

	if ( it == varTypeMap_.end() ) {
		string ret = variableName + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return fmiTypeUnknown;
	}

	return it->second;
}


fmiStatus FMUModelExchange::instantiate(const string& instanceName, fmiBoolean loggingOn)
{
	instanceName_ = instanceName;

	if ( fmu_ == 0 ) return lastStatus_ = fmiError;

	time_ = 0.;
	tnextevent_ = numeric_limits<fmiTime>::infinity();

	// Memory allocation.
	if ( nEventInds_ > 0 ) {
		eventsind_ = new fmiReal[nEventInds_];
		preeventsind_ = new fmiReal[nEventInds_];
	}

	if ( nStateVars_ > 0 ) {
		intStates_ = new fmiReal[nStateVars_];
		intDerivatives_ = new fmiReal[nStateVars_];
	}

	for ( size_t i = 0; i < nEventInds_; ++i ) {
		eventsind_[i] = 0;
		preeventsind_[i] = 0;
	}

	eventinfo_ = new fmiEventInfo;

	const string& guid = fmu_->description->getGUID();

	instance_ = fmu_->functions->instantiateModel( instanceName_.c_str(), guid.c_str(),
						       *fmu_->callbacks, loggingOn );

	if ( 0 == instance_ ) return lastStatus_ = fmiError;

	lastStatus_ = fmu_->functions->setDebugLogging( instance_, loggingOn );

	return lastStatus_;
}


fmiStatus FMUModelExchange::initialize()
{
	// NB: If instance_ != 0 then also fmu_ != 0.
	if ( 0 == instance_ ) return fmiError;

	// Basic settings.
	fmu_->functions->setTime( instance_, time_ );
	lastStatus_ = fmu_->functions->initialize( instance_, fmiFalse, 1e-5, eventinfo_ );

	return lastStatus_;
}


fmiReal FMUModelExchange::getTime() const
{
	return time_;
}


void FMUModelExchange::setTime( fmiReal time )
{
	time_ = time;
	// NB: If instance_ != 0 then also fmu_ != 0.
	if ( 0 != instance_ ) fmu_->functions->setTime( instance_, time_ );
}


void FMUModelExchange::rewindTime( fmiReal deltaRewindTime )
{
	time_ -= deltaRewindTime;
	fmu_->functions->setTime( instance_, time_ );
	//fmu_->functions->eventUpdate( instance_, fmiFalse, eventinfo_ );
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, fmiReal& val )
{
	return lastStatus_ = fmu_->functions->setReal( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, fmiInteger& val )
{
	return lastStatus_ = fmu_->functions->setInteger( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, fmiBoolean& val )
{
	return lastStatus_ = fmu_->functions->setBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, string& val )
{
	const char* cString = val.c_str();
	return lastStatus_ = fmu_->functions->setString( instance_, &valref, 1, &cString );
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, fmiReal* val, size_t ival)
{
	return lastStatus_ = fmu_->functions->setReal(instance_, valref, ival, val);
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, fmiInteger* val, size_t ival)
{
	return lastStatus_ = fmu_->functions->setInteger(instance_, valref, ival, val);
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, fmiBoolean* val, size_t ival)
{
	return lastStatus_ = fmu_->functions->setBoolean(instance_, valref, ival, val);
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, string* val, size_t ival)
{
	const char** cStrings = new const char*[ival];

	for ( size_t i = 0; i < ival; i++ ) {
		cStrings[i] = val[i].c_str();
	}
	lastStatus_ = fmu_->functions->setString(instance_, valref, ival, cStrings);
	delete [] cStrings;
	return lastStatus_;
}


fmiStatus FMUModelExchange::setValue( const string& name, fmiReal val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return lastStatus_ = fmu_->functions->setReal( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiStatus FMUModelExchange::setValue( const string& name, fmiInteger val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return lastStatus_ = fmu_->functions->setInteger( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiStatus FMUModelExchange::setValue( const string& name, fmiBoolean val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return lastStatus_ = fmu_->functions->setBoolean( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiStatus FMUModelExchange::setValue( const string& name, string val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	const char* cString = val.c_str();

	if ( it != varMap_.end() ) {
		return lastStatus_ = fmu_->functions->setString( instance_, &it->second, 1, &cString );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, fmiReal& val )
{
	return lastStatus_ = fmu_->functions->getReal( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, fmiInteger& val )
{
	return lastStatus_ = fmu_->functions->getInteger( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, fmiBoolean& val )
{
	return lastStatus_ = fmu_->functions->getBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, string& val )
{
	const char* cString;
	lastStatus_ = fmu_->functions->getString( instance_, &valref, 1, &cString );
	val = string( cString );
	return lastStatus_;
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, fmiReal* val, size_t ival )
{
	return lastStatus_ = fmu_->functions->getReal( instance_, valref, ival, val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, fmiInteger* val, size_t ival )
{
	return lastStatus_ = fmu_->functions->getInteger( instance_, valref, ival, val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, fmiBoolean* val, size_t ival )
{
	return lastStatus_ = fmu_->functions->getBoolean( instance_, valref, ival, val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, string* val, size_t ival )
{
	const char** cStrings = 0;

	lastStatus_ = fmu_->functions->getString( instance_, valref, ival, cStrings );

	if ( 0 != cStrings ) {
		for ( size_t i = 0; i < ival; i++ ) {
			val[i] = string( cStrings[i] );
		}
	}

	return lastStatus_;
}


fmiStatus FMUModelExchange::getValue( const string& name, fmiReal& val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return lastStatus_ = fmu_->functions->getReal( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiStatus FMUModelExchange::getValue( const string& name, fmiInteger& val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return lastStatus_ = fmu_->functions->getInteger( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiStatus FMUModelExchange::getValue( const string& name, fmiBoolean& val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiStatus FMUModelExchange::getValue( const string& name, string& val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	const char* cString;

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getString( instance_, &it->second, 1, &cString );
		val = string( cString );
		return lastStatus_;
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiReal FMUModelExchange::getRealValue( const string& name )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	fmiReal val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getReal( instance_, &it->second, 1, val );
	} else {
		val[0] = numeric_limits<fmiReal>::quiet_NaN();
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return val[0];
}


fmiInteger FMUModelExchange::getIntegerValue( const string& name )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	fmiInteger val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getInteger( instance_, &it->second, 1, val );
	} else {
		val[0] = numeric_limits<fmiInteger>::quiet_NaN();
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return val[0];
}


fmiBoolean FMUModelExchange::getBooleanValue( const string& name )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	fmiBoolean val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, val );
	} else {
		val[0] = fmiFalse;
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return val[0];
}


fmiString FMUModelExchange::getStringValue( const string& name )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	fmiString val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getString( instance_, &it->second, 1, val );
	} else {
		val[0] = 0;
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return val[0];
}


fmiStatus FMUModelExchange::getLastStatus() const
{
	return lastStatus_;
}


fmiStatus FMUModelExchange::getContinuousStates( fmiReal* val )
{
	return lastStatus_ = fmu_->functions->getContinuousStates( instance_, val, nStateVars_ );
}


fmiStatus FMUModelExchange::setContinuousStates( const fmiReal* val )
{
	return lastStatus_ = fmu_->functions->setContinuousStates( instance_, val, nStateVars_ );
}


fmiStatus FMUModelExchange::getDerivatives( fmiReal* val )
{
	return lastStatus_ = fmu_->functions->getDerivatives( instance_, val, nStateVars_ );
}


fmiValueReference FMUModelExchange::getValueRef( const string& name ) const {
	map<string,fmiValueReference>::const_iterator it = varMap_.find(name);

	if ( it != varMap_.end() ) {
		return it->second;
	} else {
		return fmiUndefinedValueReference;
	}
}


fmiStatus FMUModelExchange::getEventIndicators( fmiReal* eventsind )
{
	return lastStatus_ = fmu_->functions->getEventIndicators(instance_, eventsind, nEventInds());
}


fmiReal FMUModelExchange::integrate( fmiReal tstop, unsigned int nsteps )
{
	assert( nsteps > 0 );
	double deltaT = ( tstop - getTime() ) / nsteps;
	return integrate( tstop, deltaT );
}


fmiReal FMUModelExchange::integrate( fmiReal tstop, double deltaT )
{
	fmiReal dt;

	assert( deltaT > 0 );

	lastEventTime_ = numeric_limits<fmiTime>::infinity();

	if ( 0 != nStateVars_ ) {

		// If we stopped before an event, we have to handle it befor we integrate again.
		if ( stopBeforeEvent_ ) {
			if ( intEventFlag_ && getTime() == tlaststop_ ) {
				// Save the state of the event flag, it might have been reset before.
				fmiBoolean flag = eventFlag_;

				// Integrate one step with explicit euler to just trigger the event _once_.
				setTime( tstart_ );
				getContinuousStates( intStates_ );
				getDerivatives( intDerivatives_ );

				for ( size_t i = 0; i < nStateVars_; i++ ) {
					intStates_[i] = intStates_[i] + ( tlaststop_ - tstart_ ) * intDerivatives_[i];
				}

				setContinuousStates( intStates_ );
				setTime( tlaststop_ );
				completedIntegratorStep();
				handleEvents( tlaststop_ );

				tstart_ = tlaststop_; // Start where our integration step stopped.
				intEventFlag_ = fmiFalse; // Otherwise the integration would do nothing.
				eventFlag_ = flag; // Reset the state of the event flag.

			} 
			else if ( getTime() != tlaststop_ ) {
				intEventFlag_ = fmiFalse;
			}
		}

		// forget Events that happened in the last time step.
		firstFailedIntegratorStepTime_  = tstop;
		integrator_->integrate( ( tstop - getTime() ), deltaT );

		if ( tnextevent_ < tstop ) {
			if ( stopBeforeEvent_ ) {
				tstart_ = tnextevent_ - getTime() - eventSearchPrecision_/2;
				integrator_->integrate( tstart_ , eventSearchPrecision_/8 );
				tlaststop_ = tnextevent_;
				setTime( tstart_ ); // set the time of the fmu and set the time of the member variable differently,
				time_ = tnextevent_; // because we check it when we look if we have to step over the event first.
				intEventFlag_ = fmiTrue;
			} else {
				integrator_->integrate( tnextevent_ - getTime(), eventSearchPrecision_/8 );
				raiseEvent(); // otherwise handleEvents won't know it has to handle the time-event
				setTime( tnextevent_ );
				completedIntegratorStep();
				handleEvents( tnextevent_ );
				intEventFlag_ = fmiFalse;
			}

			eventFlag_ = fmiTrue;
			return lastCompletedIntegratorStepTime_;
			
		} else if ( intEventFlag_ ) { // If we stopped because of an event, start searching for it.
			// Start were the last eventless integration step stopped.
			tstart_ = lastCompletedIntegratorStepTime_; 
			// Stop where integrator_->integrate detected the first problem
			tstop = firstFailedIntegratorStepTime_;

			while ( ( tstop - tstart_ > eventSearchPrecision_ ) && ( tstart_ < tstop ) ) {

				setTime( tstart_ );
				intEventFlag_ = fmiFalse;
				resetEventIndicators();
				dt = ( tstop - tstart_ ) / 2;

				// Try integrating the interval.
				integrator_->integrate( dt, std::min( deltaT, dt/2 ) );
				checkStateEvent();
				if ( intEventFlag_ ) {
					tstart_ = lastCompletedIntegratorStepTime_;
					tstop = ( tstop + tstart_ ) / 2;
				} else {
					tstart_ = lastCompletedIntegratorStepTime_; // Equal to (tstop+tstart_)/2.
				}

				intEventFlag_ = fmiTrue;
			}

			if ( ! stopBeforeEvent_ ) {

				// Integrate one step with explicit euler to just trigger the event _once_.
				getContinuousStates( intStates_ );
				getDerivatives( intDerivatives_ );

				for ( size_t i = 0; i < nStateVars_; ++i ) {
					intStates_[i] = intStates_[i] + ( tstop - tstart_ ) * intDerivatives_[i];
				}

				setContinuousStates( intStates_ );
				setTime( tstop );
				completedIntegratorStep();
				handleEvents( tstop );

				intEventFlag_ = fmiFalse; // Just handled the event.
			} else {
				tlaststop_ = tstop;
			}
			
			setTime( tstop );
			getContinuousStates( intStates_ );
		}
	} else { // No continuous states -> skip integration.
		setTime( tstop ); // TODO: event handling?
		completedIntegratorStep();
		handleEvents( tstop );
	}

	if ( lastEventTime_ != numeric_limits<fmiTime>::infinity() ) {
		return lastEventTime_;
	} else {
		return tstop;
	}
}


void FMUModelExchange::raiseEvent()
{
	raisedEvent_ = fmiTrue;
}


fmiBoolean FMUModelExchange::checkEvents()
{
	fmiBoolean event = checkStateEvent() || checkTimeEvent();
	return event;
}


fmiBoolean FMUModelExchange::checkStateEvent()
{
	fmiBoolean stateEvent = fmiFalse;

	for ( size_t i = 0; i < nEventInds_; ++i ) preeventsind_[i] = eventsind_[i];

	getEventIndicators( eventsind_ );

	for ( size_t i = 0; i < nEventInds_; ++i ) stateEvent = stateEvent || ( preeventsind_[i] * eventsind_[i] < 0 );

	intEventFlag_ |= stateEvent;
	eventFlag_ |= stateEvent;

	return stateEvent;
}


fmiBoolean FMUModelExchange::checkTimeEvent()
{
	fmu_->functions->eventUpdate( instance_, fmiTrue, eventinfo_ );
	if ( eventinfo_->upcomingTimeEvent ) {
		tnextevent_ = eventinfo_->nextEventTime;
	} else {
		tnextevent_ = numeric_limits<fmiTime>::infinity();
	}

	return eventinfo_->upcomingTimeEvent;
}


fmiStatus FMUModelExchange::resetEventIndicators()
{
	fmiStatus status1 = getEventIndicators( preeventsind_ );
	fmiStatus status2 = getEventIndicators( eventsind_ );

	return lastStatus_ = ( ( ( status1 == fmiOK ) && ( status2 == fmiOK ) ) ? fmiOK : fmiFatal ) ;
}



void FMUModelExchange::handleEvents( fmiTime tStop )
{
	// Get event indicators.

	// use checkStateEvent to set stateEvent_;
	stateEvent_ = checkStateEvent();

	if ( intEventFlag_ && lastEventTime_ == numeric_limits<fmiTime>::infinity() ) {
		lastEventTime_ = time_;
	}

	timeEvent_ = ( getTime() > tnextevent_ ); // abs( time - tnextevent_ ) <= EPS ;

#ifdef FMI_DEBUG
	if ( callEventUpdate_ || stateEvent_ || timeEvent_ || raisedEvent_ )
		cout << "[FMUModelExchange::handleEvents] An event occured: "
		     << "  event_update : " << callEventUpdate_
			//		     << " , stateEvent : "  << intEventFlag_ ???
		     << " , timeEvent : "  << timeEvent_
		     << " , raisedEvent : " << raisedEvent_ << endl;  fflush( stdout );
#endif

	if ( callEventUpdate_ || stateEvent_ || timeEvent_ || raisedEvent_ ) {
		eventinfo_->iterationConverged = fmiFalse;

		// Event time is identified and stored values get updated.
		unsigned int cnt = 0;
		while ( ( fmiFalse == eventinfo_->iterationConverged ) && ( cnt < maxEventIterations_ ) )
		{
			fmu_->functions->eventUpdate( instance_, fmiTrue, eventinfo_ );

			// If intermediate results need to be set.
			// if ( fmiFalse == eventinfo_->iterationConverged ) {
			// 	fmiStatus status = fmiGetReal( instance_, description_->valueRefsPtr_,
			// 				       description_->nValueRefs_, storedv_ );
			// }

			cnt++;
		}

		// Next time event is identified.
		if ( eventinfo_->upcomingTimeEvent ) {
			tnextevent_ = eventinfo_->nextEventTime; //( eventinfo_->nextEventTime < tStop ) ? eventinfo_->nextEventTime : tStop;
		} else {
			tnextevent_ = numeric_limits<fmiTime>::infinity();
		}

		raisedEvent_ = fmiFalse;
		stateEvent_ = fmiFalse;
	}
}


fmiStatus FMUModelExchange::completedIntegratorStep()
{
	lastCompletedIntegratorStepTime_ = getTime();
	// Inform the model about an accepted step.
	return lastStatus_ = fmu_->functions->completedIntegratorStep( instance_, &callEventUpdate_ );
}

void FMUModelExchange::failedIntegratorStep( fmiTime time )
{
	// use min to get the smallest upper limit for the event time
	firstFailedIntegratorStepTime_ = fmin( time, firstFailedIntegratorStepTime_ );
}


fmiBoolean FMUModelExchange::getIntEvent()
{
	return intEventFlag_;
}


fmiReal FMUModelExchange::getTimeEvent()
{
	return tnextevent_;
}


fmiBoolean FMUModelExchange::getEventFlag()
{
	return eventFlag_;
}


void FMUModelExchange::setEventFlag( fmiBoolean flag )
{
	eventFlag_ = flag;
}


size_t FMUModelExchange::nStates() const
{
	return nStateVars_;
}


size_t FMUModelExchange::nEventInds() const
{
	return nEventInds_;
}


size_t FMUModelExchange::nValueRefs() const
{
	return nValueRefs_;
}


void FMUModelExchange::logger( fmiStatus status, const string& category, const string& msg ) const
{
	fmu_->callbacks->logger( instance_, instanceName_.c_str(), status, category.c_str(), msg.c_str() );
}


void FMUModelExchange::logger( fmiStatus status, const char* category, const char* msg ) const
{
	fmu_->callbacks->logger( instance_, instanceName_.c_str(), status, category, msg );
}


fmiStatus FMUModelExchange::setCallbacks( me::fmiCallbackLogger logger,
					  me::fmiCallbackAllocateMemory allocateMemory,
					  me::fmiCallbackFreeMemory freeMemory )
{
	if ( ( 0 == logger ) || ( 0 == allocateMemory ) || ( 0 == freeMemory ) ) {
		this->logger( fmiError, "ERROR", "callback function pointer(s) invalid" );
		return fmiError;
	}

	fmu_->callbacks->logger = logger;
	fmu_->callbacks->allocateMemory = allocateMemory;
	fmu_->callbacks->freeMemory = freeMemory;

	return fmiOK;
}
