/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file FMUModelExchange.cpp
 */

#ifdef FMI_DEBUG
#include <iostream>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <cassert>
#include <limits>
#include <cmath>

#include "common/FMIPPConfig.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/fmi_v1.0/fmi_me.h"

#include "import/base/include/FMUModelExchange.h"
#include "import/base/include/ModelManager.h"

#include "import/integrators/include/Integrator.h"


static me::fmiCallbackFunctions functions = { FMUModelExchange::logger, calloc, free };


using namespace std;


FMUModelExchange::FMUModelExchange( const string& fmuPath,
				    const string& modelName,
				    fmiBoolean stopBeforeEvent,
				    fmiReal eventSearchPrecision ) :
	instance_( NULL ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision )
{
#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif

	ModelManager& manager = ModelManager::getModelManager();
	fmu_ = manager.getModel( fmuPath, modelName );
	readModelDescription();

	integrator_ = new Integrator( this, Integrator::dp );

#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::ctor] DONE." << endl;
#endif
}


FMUModelExchange::FMUModelExchange( const string& xmlPath,
	  const string& dllPath,
	  const string& modelName,
	  fmiBoolean stopBeforeEvent,
	  fmiReal eventSearchPrecision ) :
	instance_( NULL ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision )
{
#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif

	ModelManager& manager = ModelManager::getModelManager();
	fmu_ = manager.getModel( xmlPath, dllPath, modelName );
	readModelDescription();

	integrator_ = new Integrator( this, Integrator::dp );

#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::ctor] done." << endl;
#endif
}


FMUModelExchange::FMUModelExchange( const FMUModelExchange& aFMU ) :
	instance_( NULL ),
	fmu_( aFMU.fmu_ ),
	nStateVars_( aFMU.nStateVars_ ),
	nEventInds_( aFMU.nEventInds_ ),
	nValueRefs_( aFMU.nValueRefs_ ),
	varMap_( aFMU.varMap_ ),
	varTypeMap_( aFMU.varTypeMap_ ),
	stopBeforeEvent_( aFMU.stopBeforeEvent_ ),
	eventSearchPrecision_( aFMU.eventSearchPrecision_ )
{
#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::ctor]" << endl; fflush( stdout );
#endif

	integrator_ = new Integrator( this, aFMU.integrator_->type() );

#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::ctor] DONE." << endl; fflush( stdout );
#endif
}


FMUModelExchange::~FMUModelExchange()
{
	delete integrator_;

	if ( instance_ ) {
		delete[] eventsind_;
		delete[] preeventsind_;
		delete eventinfo_;

		fmu_->functions->terminate( instance_ );
#ifndef MINGW
		// EW: This call causes a seg fault with OpenModelica FMUs under MINGW ...
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
		logger( fmiDiscard, ret );
		return fmiTypeUnknown;
	}

	return it->second;
}


fmiStatus FMUModelExchange::instantiate(const string& instanceName, fmiBoolean loggingOn)
{
	instanceName_ = instanceName;

	if (fmu_ == 0) {
		return fmiError;
	}

#ifdef FMI_DEBUG
	// General information ...
	cout << "[FMUModelExchange::instantiate] Types Platform: " << fmu_->functions->getModelTypesPlatform()
	     << ", FMI Version:  " << fmu_->functions->getVersion() << endl; fflush( stdout );
#endif

	// Basic settings: @todo from a menu.
	time_ = 0.;
	tnextevent_ = numeric_limits<fmiTime>::infinity();

	// Memory allocation.
#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::instantiate] nStateVars_ = " << nStateVars_ << " -  nEventInds_ = "
	     << nEventInds_ << endl; fflush( stdout );
#endif

	eventsind_    = new fmiReal[nEventInds_];
	preeventsind_ = new fmiReal[nEventInds_];

	for ( size_t i = 0; i < nEventInds_; ++i ) {
		eventsind_[i] = 0;
		preeventsind_[i] = 0;
	}

	eventinfo_ = new fmiEventInfo;

	// Instantiation of the model: @todo from menu.
	// get this right ;) !!!
	const string& guid = fmu_->description->getGUID();

#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::instantiate] GUID = " << guid << endl; fflush( stdout );
	cout << "[FMUModelExchange::instantiate] instanceName = " << instanceName_ << endl; fflush( stdout );
#endif

	instance_ = fmu_->functions->instantiateModel( instanceName_.c_str(), guid.c_str(), functions, fmiTrue );

	if ( 0 == instance_ ) {
#ifdef FMI_DEBUG
		cout << "[FMUModelExchange::instantiate] instantiateModel failed. " << endl; fflush( stdout );
#endif
		return fmiError;
	}

#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::instantiate] instance_ = " << instance_ << endl; fflush( stdout );
#endif

	fmiStatus status = fmu_->functions->setDebugLogging( instance_, loggingOn );

	if (loggingOn) {
		functions.logger( instance_, instanceName_.c_str(), status, "?", "Model instance initialized"); fflush( stdout );
	}

#ifdef FMI_DEBUG
	cout << "[FMUModelExchange::instantiate] DONE. status = " << status << endl; fflush( stdout );
#endif

	return status;
}


fmiStatus FMUModelExchange::initialize()
{
	if ( 0 == instance_ ) {
		return fmiError;
	}

	// Basic settings.
	fmu_->functions->setTime( instance_, time_ );
	fmiStatus status = fmu_->functions->initialize( instance_, fmiFalse, 1e-5, eventinfo_ );

	stateEvent_ = fmiFalse;
	eventFlag_ = fmiFalse;
	intEventFlag_ = fmiFalse;
	timeEvent_ = fmiFalse;
	callEventUpdate_ = fmiFalse;
	raisedEvent_ = fmiFalse;

	return status;
}


fmiReal FMUModelExchange::getTime() const
{
	return time_;
}


void FMUModelExchange::setTime( fmiReal time )
{
	time_ = time;
	fmu_->functions->setTime( instance_, time_ );
}


void FMUModelExchange::rewindTime( fmiReal deltaRewindTime )
{
	time_ -= deltaRewindTime;
	fmu_->functions->setTime( instance_, time_ );
	//fmu_->functions->eventUpdate( instance_, fmiFalse, eventinfo_ );
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, fmiReal& val )
{
	return fmu_->functions->setReal( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, fmiInteger& val )
{
	return fmu_->functions->setInteger( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, fmiBoolean& val )
{
	return fmu_->functions->setBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, std::string& val )
{
	const char* cString = val.c_str();
	return fmu_->functions->setString( instance_, &valref, 1, &cString );
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, fmiReal* val, size_t ival)
{
	return fmu_->functions->setReal(instance_, valref, ival, val);
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, fmiInteger* val, size_t ival)
{
	return fmu_->functions->setInteger(instance_, valref, ival, val);
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, fmiBoolean* val, size_t ival)
{
	return fmu_->functions->setBoolean(instance_, valref, ival, val);
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, std::string* val, size_t ival)
{
	const char** cStrings = new const char*[ival];
	fmiStatus status;

	for ( std::size_t i = 0; i < ival; i++ ) {
		cStrings[i] = val[i].c_str();
	}
	status = fmu_->functions->setString(instance_, valref, ival, cStrings);
	delete [] cStrings;
	return status;
}


fmiStatus FMUModelExchange::setValue( const string& name, fmiReal val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmu_->functions->setReal( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMUModelExchange::setValue( const string& name, fmiInteger val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmu_->functions->setInteger( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMUModelExchange::setValue( const string& name, fmiBoolean val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmu_->functions->setBoolean( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMUModelExchange::setValue( const string& name, std::string val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	const char* cString = val.c_str();

	if ( it != varMap_.end() ) {
		return fmu_->functions->setString( instance_, &it->second, 1, &cString );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, fmiReal& val ) const
{
	return fmu_->functions->getReal( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, fmiInteger& val ) const
{
	return fmu_->functions->getInteger( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, fmiBoolean& val ) const
{
	return fmu_->functions->getBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, std::string& val ) const
{
	const char* cString;
	fmiStatus status = fmu_->functions->getString( instance_, &valref, 1, &cString );
	val = std::string( cString );
	return status;
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, fmiReal* val, size_t ival ) const
{
	return fmu_->functions->getReal( instance_, valref, ival, val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, fmiInteger* val, size_t ival ) const
{
	return fmu_->functions->getInteger( instance_, valref, ival, val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, fmiBoolean* val, size_t ival ) const
{
	return fmu_->functions->getBoolean( instance_, valref, ival, val );
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, std::string* val, size_t ival ) const
{
	const char** cStrings = 0;

	fmiStatus status = fmu_->functions->getString( instance_, valref, ival, cStrings );

	if ( 0 != cStrings ) {
		for ( std::size_t i = 0; i < ival; i++ ) {
			val[i] = std::string( cStrings[i] );
		}
	}

	return status;
}


fmiStatus FMUModelExchange::getValue( const string& name, fmiReal& val ) const
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmu_->functions->getReal( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMUModelExchange::getValue( const string& name, fmiInteger& val ) const
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmu_->functions->getInteger( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMUModelExchange::getValue( const string& name, fmiBoolean& val ) const
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmu_->functions->getBoolean( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMUModelExchange::getValue( const string& name, std::string& val ) const
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	const char* cString;
	fmiStatus status;

	if ( it != varMap_.end() ) {
		status = fmu_->functions->getString( instance_, &it->second, 1, &cString );
		val = std::string( cString );
		return status;
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMUModelExchange::getContinuousStates( fmiReal* val ) const
{
	return fmu_->functions->getContinuousStates( instance_, val, nStateVars_ );
}


fmiStatus FMUModelExchange::setContinuousStates( const fmiReal* val )
{
	return fmu_->functions->setContinuousStates( instance_, val, nStateVars_ );
}


fmiStatus FMUModelExchange::getDerivatives( fmiReal* val ) const
{
	return fmu_->functions->getDerivatives( instance_, val, nStateVars_ );
}


fmiValueReference FMUModelExchange::getValueRef( const string& name ) const {
	map<string,fmiValueReference>::const_iterator it = varMap_.find(name);

	if ( it != varMap_.end() ) {
		return it->second;
	} else {
		return fmiUndefinedValueReference;
	}
}


fmiStatus FMUModelExchange::getEventIndicators( fmiReal* eventsind ) const
{
	fmiStatus status = fmu_->functions->getEventIndicators(instance_, eventsind, nEventInds());
	return status;
}


fmiReal FMUModelExchange::integrate( fmiReal tstop, unsigned int nsteps )
{
	assert( nsteps > 0 );
	double deltaT = ( tstop - getTime() ) / nsteps;
	return integrate( tstop, deltaT );
}


fmiReal FMUModelExchange::integrate( fmiReal tstop, double deltaT )
{
	static fmiReal tstart;
	static fmiReal tlaststop;
	fmiReal dt;

	assert( deltaT > 0 );

	lastEventTime_ = numeric_limits<fmiTime>::infinity();

	if ( 0 != nStateVars_ ) {

		// if we stopped before an event, we have to handle it befor we integrate again
		if ( stopBeforeEvent_ && intEventFlag_ && getTime() == tlaststop ) {
			fmiBoolean flag = eventFlag_; // save the state of the event flag, it might have been reset before

			// integrate one step with explicit euler to just trigger the event _once_
			fmiReal* states = new fmiReal[nStateVars_];
			fmiReal* derivatives = new fmiReal[nStateVars_];

			setTime( tstart );
			getContinuousStates( states );
			getDerivatives( derivatives );

			for ( size_t i = 0; i < nStateVars_; i++ ) {
				states[i] = states[i] + ( tlaststop - tstart ) * derivatives[i];
			}

			setContinuousStates( states );
			setTime( tlaststop );
			completedIntegratorStep();
			handleEvents( tlaststop );

			tstart = tlaststop; // start where our integration step stopped
			intEventFlag_ = fmiFalse; // otherwise the integration would do nothing
			eventFlag_ = flag; // reset the state of the event flag

		} 
		else if ( stopBeforeEvent_ && getTime() != tlaststop ) {
			intEventFlag_ = fmiFalse;
		}

		integrator_->integrate( ( tstop - getTime() ), deltaT );

		if ( intEventFlag_ ) { // if we stopped because of an event, start searching for it

			// start were the last eventless integration step stopped
			tstart = lastCompletedIntegratorStepTime_; 

			while ( ( tstop - tstart > eventSearchPrecision_ ) && ( tstart < tstop ) ) {

				setTime( tstart );
				intEventFlag_ = fmiFalse;
				resetEventIndicators();
				dt = ( tstop - tstart ) / 2;

				// try integrating the interval
				integrator_->integrate( dt, std::min( deltaT, dt/2 ) );
				checkStateEvent();
				if ( intEventFlag_ ) {
					tstart = lastCompletedIntegratorStepTime_;
					tstop = ( tstop + tstart ) / 2;
				} else {
					tstart = lastCompletedIntegratorStepTime_; // Equal to (tstop+tstart)/2.
				}

				intEventFlag_ = fmiTrue;
			}

			if ( ! stopBeforeEvent_ ) {
				// integrate one step with explicit euler to just trigger the event _once_
				fmiReal* states = new fmiReal[nStateVars_];
				fmiReal* derivatives = new fmiReal[nStateVars_];

				getContinuousStates( states );
				getDerivatives( derivatives );

				for ( size_t i = 0; i < nStateVars_; ++i ) {
					states[i] = states[i] + ( tstop - tstart ) * derivatives[i];
				}

				setContinuousStates( states );
				setTime( tstop );
				completedIntegratorStep();
				handleEvents( tstop );

				intEventFlag_ = fmiFalse; // just handled the event
			} else {
				tlaststop = tstop;
			}
			
			setTime( tstop );

			fmiReal* states = new fmiReal[nStateVars_];
			getContinuousStates( states );
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


fmiStatus FMUModelExchange::resetEventIndicators()
{
	fmiStatus status1 = getEventIndicators( preeventsind_ );
	fmiStatus status2 = getEventIndicators( eventsind_ );

	return ( ( status1 == fmiOK ) && ( status2 == fmiOK ) ) ? fmiOK : fmiFatal;
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
			tnextevent_ = ( eventinfo_->nextEventTime < tStop ) ? eventinfo_->nextEventTime : tStop;
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
	return fmu_->functions->completedIntegratorStep( instance_, &callEventUpdate_ );
}


fmiBoolean FMUModelExchange::getIntEvent()
{
	return intEventFlag_;
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


void FMUModelExchange::logger( fmiStatus status, const string& msg ) const
{
	functions.logger( instance_, instanceName_.c_str(), status, "?", msg.c_str() );
}


void FMUModelExchange::logger( fmiStatus status, const char* msg ) const
{
	functions.logger( instance_, instanceName_.c_str(), status, "?", msg );
}


void FMUModelExchange::logger( fmiComponent m, fmiString instanceName,
		  fmiStatus status, fmiString category,
		  fmiString message, ... )
{
	char msg[4096];
	char buf[4096];
	int len;
	int capacity;

	va_list ap;
	va_start( ap, message );
	capacity = sizeof(buf) - 1;
#if defined(_MSC_VER) && _MSC_VER>=1400
	len = _snprintf_s( msg, capacity, _TRUNCATE, "%s: %s", instanceName, message );
	if ( len < 0 ) goto fail;
	len = vsnprintf_s( buf, capacity, _TRUNCATE, msg, ap );
	if ( len < 0 ) goto fail;
#elif defined(WIN32)
	len = _snprintf( msg, capacity, "%s: %s", instanceName, message );
	if ( len < 0 ) goto fail;
	len = vsnprintf( buf, capacity, msg, ap );
	if ( len < 0 ) goto fail;
#else
	len = snprintf( msg, capacity, "%s: %s", instanceName, message );
	if ( len < 0 ) goto fail;
	len = vsnprintf( buf, capacity, msg, ap );
	if ( len < 0 ) goto fail;
#endif
	/* append line break */
	buf[len] = '\n';
	buf[len + 1] = 0;
	va_end( ap );

	switch ( status ) {
	case fmiFatal:
		printf( buf );
		break;
	default:
		printf( buf );
		break;
	}
	return;

fail:
	printf( "logger failed, message too long?" );
}
