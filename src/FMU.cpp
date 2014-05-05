/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file FMU.cpp
 */

#ifdef FMI_DEBUG
#include <iostream>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <cassert>
#include <limits>
#include <cmath>

#include "FMU.h"
#include "ModelManager.h"
#include "FMUIntegrator.h"


static  fmiCallbackFunctions functions = { FMU::logger, calloc, free };


using namespace std;


FMU::FMU( const string& fmuPath,
	  const string& modelName,
	  fmiBoolean stopBeforeEvent,
	  fmiReal eventSearchPrecision ) :
	instance_( NULL ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision )
{
#ifdef FMI_DEBUG
	cout << "[FMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif

	ModelManager& manager = ModelManager::getModelManager();
	fmuFun_ = manager.getModel( fmuPath, modelName );
	readModelDescription();

	integrator_ = new FMUIntegrator( this, FMUIntegrator::dp );

#ifdef FMI_DEBUG
	cout << "[FMU::ctor] DONE." << endl;
#endif
}


FMU::FMU( const string& xmlPath,
	  const string& dllPath,
	  const string& modelName,
	  fmiBoolean stopBeforeEvent,
	  fmiReal eventSearchPrecision ) :
	instance_( NULL ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision )
{
#ifdef FMI_DEBUG
	cout << "[FMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif

	ModelManager& manager = ModelManager::getModelManager();
	fmuFun_ = manager.getModel( xmlPath, dllPath, modelName );
	readModelDescription();

	integrator_ = new FMUIntegrator( this, FMUIntegrator::dp );

#ifdef FMI_DEBUG
	cout << "[FMU::ctor] done." << endl;
#endif
}


FMU::FMU( const FMU& aFMU ) :
	instance_( NULL ),
	fmuFun_( aFMU.fmuFun_ ),
	nStateVars_( aFMU.nStateVars_ ),
	nEventInds_( aFMU.nEventInds_ ),
	nValueRefs_( aFMU.nValueRefs_ ),
	varMap_( aFMU.varMap_ ),
	varTypeMap_( aFMU.varTypeMap_ ),
	stopBeforeEvent_( aFMU.stopBeforeEvent_ ),
	eventSearchPrecision_( aFMU.eventSearchPrecision_ )
{
#ifdef FMI_DEBUG
	cout << "[FMU::ctor]" << endl; fflush( stdout );
#endif

	integrator_ = new FMUIntegrator( this, aFMU.integrator_->type() );

#ifdef FMI_DEBUG
	cout << "[FMU::ctor] DONE." << endl; fflush( stdout );
#endif
}


FMU::~FMU()
{
	delete integrator_;

	if ( instance_ ) {
		delete[] eventsind_;
		delete[] preeventsind_;
		delete eventinfo_;

		fmuFun_->terminate( instance_ );
#ifndef MINGW
		// EW: This call causes a seg fault with OpenModelica FMUs under MINGW ...
		fmuFun_->freeModelInstance( instance_ );
#endif
	}
}


void FMU::readModelDescription() {
	nStateVars_ = getNumberOfStates( fmuFun_->modelDescription );
	nEventInds_ = getNumberOfEventIndicators( fmuFun_->modelDescription );

	for ( size_t i = 0; fmuFun_->modelDescription->modelVariables[i]; ++i ) {
		ScalarVariable* var = (ScalarVariable*) fmuFun_->modelDescription->modelVariables[i];
		string varName = getString( var,att_name );

		// Map name to value reference.
		varMap_.insert( make_pair( varName, getValueReference( var ) ) );

		// Map name to value type.
		switch ( var->typeSpec->type ) {
		case  elm_Real:
			varTypeMap_.insert( make_pair( varName, fmiTypeReal ) );
			break;
		case elm_Integer:
			varTypeMap_.insert( make_pair( varName, fmiTypeInteger ) );
			break;
		case elm_Boolean:
			varTypeMap_.insert( make_pair( varName, fmiTypeBoolean ) );
			break;
		case elm_String:
			varTypeMap_.insert( make_pair( varName, fmiTypeString ) );
			break;
		default:
			varTypeMap_.insert( make_pair( varName, fmiTypeUnknown ) );
			break;
		}
	}

	nValueRefs_ = varMap_.size();
}


FMIType FMU::getType( const string& variableName ) const
{
	map<string,FMIType>::const_iterator it = varTypeMap_.find( variableName );

	if ( it == varTypeMap_.end() ) {
		string ret = variableName + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiTypeUnknown;
	}

	return it->second;
}


fmiStatus FMU::instantiate(const string& instanceName, fmiBoolean loggingOn)
{
	instanceName_ = instanceName;

	if (fmuFun_ == 0) {
		return fmiError;
	}

#ifdef FMI_DEBUG
	// General information ...
	cout << "[FMU::instantiate] Types Platform: " << fmuFun_->getModelTypesPlatform()
	     << ", FMI Version:  " << fmuFun_->getVersion() << endl; fflush( stdout );
#endif

	// Basic settings: @todo from a menu.
	time_ = 0.;
	tnextevent_ = numeric_limits<fmiTime>::infinity();

	// Memory allocation.
#ifdef FMI_DEBUG
	cout << "[FMU::instantiate] nStateVars_ = " << nStateVars_ << " -  nEventInds_ = "
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
	const char* guid = getString( fmuFun_->modelDescription, att_guid );

#ifdef FMI_DEBUG
	cout << "[FMU::instantiate] GUID = " << guid << endl; fflush( stdout );
	cout << "[FMU::instantiate] instanceName = " << instanceName_ << endl; fflush( stdout );
#endif

	instance_ = fmuFun_->instantiateModel( instanceName_.c_str(), guid, functions, fmiTrue );

	if (0 == instance_) {
#ifdef FMI_DEBUG
		cout << "[FMU::instantiate] instantiateModel failed. " << endl; fflush( stdout );
#endif
		return fmiError;
	}

#ifdef FMI_DEBUG
	cout << "[FMU::instantiate] instance_ = " << instance_ << endl; fflush( stdout );
#endif

	fmiStatus status = fmuFun_->setDebugLogging( instance_, loggingOn );

	if (loggingOn) {
		functions.logger( instance_, instanceName_.c_str(), status, "?", "Model instance initialized"); fflush( stdout );
	}

#ifdef FMI_DEBUG
	cout << "[FMU::instantiate] DONE. status = " << status << endl; fflush( stdout );
#endif

	return status;
}


fmiStatus FMU::initialize()
{
	if ( 0 == instance_ ) {
		return fmiError;
	}

	// Basic settings.
	fmuFun_->setTime( instance_, time_ );
	fmiStatus status = fmuFun_->initialize( instance_, fmiFalse, 1e-5, eventinfo_ );

	stateEvent_ = fmiFalse;
	eventFlag_ = fmiFalse;
	intEventFlag_ = fmiFalse;
	timeEvent_ = fmiFalse;
	callEventUpdate_ = fmiFalse;
	raisedEvent_ = fmiFalse;

	return status;
}


fmiReal FMU::getTime() const
{
	return time_;
}


void FMU::setTime( fmiReal time )
{
	time_ = time;
	fmuFun_->setTime( instance_, time_ );
}


void FMU::rewindTime( fmiReal deltaRewindTime )
{
	time_ -= deltaRewindTime;
	fmuFun_->setTime( instance_, time_ );
	//fmuFun_->eventUpdate( instance_, fmiFalse, eventinfo_ );
}


fmiStatus FMU::setValue( fmiValueReference valref, fmiReal& val )
{
	return fmuFun_->setReal( instance_, &valref, 1, &val );
}


fmiStatus FMU::setValue( fmiValueReference valref, fmiInteger& val )
{
	return fmuFun_->setInteger( instance_, &valref, 1, &val );
}


fmiStatus FMU::setValue( fmiValueReference valref, fmiBoolean& val )
{
	return fmuFun_->setBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMU::setValue( fmiValueReference valref, std::string& val )
{
	const char* cString = val.c_str();
	return fmuFun_->setString( instance_, &valref, 1, &cString );
}


fmiStatus FMU::setValue(fmiValueReference* valref, fmiReal* val, size_t ival)
{
	return fmuFun_->setReal(instance_, valref, ival, val);
}


fmiStatus FMU::setValue(fmiValueReference* valref, fmiInteger* val, size_t ival)
{
	return fmuFun_->setInteger(instance_, valref, ival, val);
}


fmiStatus FMU::setValue(fmiValueReference* valref, fmiBoolean* val, size_t ival)
{
	return fmuFun_->setBoolean(instance_, valref, ival, val);
}


fmiStatus FMU::setValue(fmiValueReference* valref, std::string* val, size_t ival)
{
	const char** cStrings = new const char*[ival];
	fmiStatus status;

	for ( std::size_t i = 0; i < ival; i++ ) {
		cStrings[i] = val[i].c_str();
	}
	status = fmuFun_->setString(instance_, valref, ival, cStrings);
	delete [] cStrings;
	return status;
}


fmiStatus FMU::setValue( const string& name, fmiReal val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmuFun_->setReal( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMU::setValue( const string& name, fmiInteger val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmuFun_->setInteger( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMU::setValue( const string& name, fmiBoolean val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmuFun_->setBoolean( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMU::setValue( const string& name, std::string val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	const char* cString = val.c_str();

	if ( it != varMap_.end() ) {
		return fmuFun_->setString( instance_, &it->second, 1, &cString );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMU::getValue( fmiValueReference valref, fmiReal& val ) const
{
	return fmuFun_->getReal( instance_, &valref, 1, &val );
}


fmiStatus FMU::getValue( fmiValueReference valref, fmiInteger& val ) const
{
	return fmuFun_->getInteger( instance_, &valref, 1, &val );
}


fmiStatus FMU::getValue( fmiValueReference valref, fmiBoolean& val ) const
{
	return fmuFun_->getBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMU::getValue( fmiValueReference valref, std::string& val ) const
{
	const char* cString;
	fmiStatus status = fmuFun_->getString( instance_, &valref, 1, &cString );
	val = std::string( cString );
	return status;
}


fmiStatus FMU::getValue( fmiValueReference* valref, fmiReal* val, size_t ival ) const
{
	return fmuFun_->getReal( instance_, valref, ival, val );
}


fmiStatus FMU::getValue( fmiValueReference* valref, fmiInteger* val, size_t ival ) const
{
	return fmuFun_->getInteger( instance_, valref, ival, val );
}


fmiStatus FMU::getValue( fmiValueReference* valref, fmiBoolean* val, size_t ival ) const
{
	return fmuFun_->getBoolean( instance_, valref, ival, val );
}


fmiStatus FMU::getValue( fmiValueReference* valref, std::string* val, size_t ival ) const
{
	const char** cStrings = 0;

	fmiStatus status = fmuFun_->getString( instance_, valref, ival, cStrings );

	if ( 0 != cStrings ) {
		for ( std::size_t i = 0; i < ival; i++ ) {
			val[i] = std::string( cStrings[i] );
		}
	}

	return status;
}


fmiStatus FMU::getValue( const string& name, fmiReal& val ) const
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmuFun_->getReal( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiReal FMU::getValue( char * n ) const
{
	fmiReal val;

	std::string name = std::string(n);

	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		fmuFun_->getReal( instance_, &it->second, 1, &val );
		return val;
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return 0;
	}
}


fmiStatus FMU::getValue( const string& name, fmiInteger& val ) const
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmuFun_->getInteger( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMU::getValue( const string& name, fmiBoolean& val ) const
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		return fmuFun_->getBoolean( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMU::getValue( const string& name, std::string& val ) const
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	const char* cString;
	fmiStatus status;

	if ( it != varMap_.end() ) {
		status = fmuFun_->getString( instance_, &it->second, 1, &cString );
		val = std::string( cString );
		return status;
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiDiscard;
	}
}


fmiStatus FMU::getContinuousStates( fmiReal* val ) const
{
	return fmuFun_->getContinuousStates( instance_, val, nStateVars_ );
}


fmiStatus FMU::setContinuousStates( const fmiReal* val )
{
	return fmuFun_->setContinuousStates( instance_, val, nStateVars_ );
}


fmiStatus FMU::getDerivatives( fmiReal* val ) const
{
	return fmuFun_->getDerivatives( instance_, val, nStateVars_ );
}


fmiValueReference FMU::getValueRef( const string& name ) const {
	map<string,fmiValueReference>::const_iterator it = varMap_.find(name);

	if ( it != varMap_.end() ) {
		return it->second;
	} else {
		return fmiUndefinedValueReference;
	}
}


fmiStatus FMU::getEventIndicators( fmiReal* eventsind ) const
{
	fmiStatus status = fmuFun_->getEventIndicators(instance_, eventsind, nEventInds());
	return status;
}


fmiReal FMU::integrate( fmiReal tstop, unsigned int nsteps )
{
	assert( nsteps > 0 );
	double deltaT = ( tstop - getTime() ) / nsteps;
	return integrate( tstop, deltaT );
}


fmiReal FMU::integrate( fmiReal tstop, double deltaT )
{
	static fmiReal tstart; // FIXME: Why is this variable declared static and at the beginning of the function?
	static fmiReal tlaststop; // FIXME: Why is this variable declared static and at the beginning of the function?
	fmiReal dt;

	assert( deltaT > 0 );
	//	handleEvents( 0 ); // this seems to be wrong !!!

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


void FMU::raiseEvent()
{
	raisedEvent_ = fmiTrue;
}


fmiBoolean FMU::checkStateEvent()
{
	fmiBoolean stateEvent = fmiFalse;

	for ( size_t i = 0; i < nEventInds_; ++i ) preeventsind_[i] = eventsind_[i];

	getEventIndicators( eventsind_ );

	for ( size_t i = 0; i < nEventInds_; ++i ) stateEvent = stateEvent || ( preeventsind_[i] * eventsind_[i] < 0 );

	intEventFlag_ |= stateEvent;
	eventFlag_ |= stateEvent;

	return stateEvent;
}


fmiStatus FMU::resetEventIndicators()
{
	fmiStatus status1 = getEventIndicators( preeventsind_ );
	fmiStatus status2 = getEventIndicators( eventsind_ );

	return ( ( status1 == fmiOK ) && ( status2 == fmiOK ) ) ? fmiOK : fmiFatal;
}



void FMU::handleEvents( fmiTime tStop )
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
		cout << "[FMU::handleEvents] An event occured: "
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
			fmuFun_->eventUpdate( instance_, fmiTrue, eventinfo_ );

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


fmiStatus FMU::completedIntegratorStep()
{
	lastCompletedIntegratorStepTime_ = getTime();
	// Inform the model about an accepted step.
	return fmuFun_->completedIntegratorStep( instance_, &callEventUpdate_ );
}


fmiBoolean FMU::getIntEvent()
{
	return intEventFlag_;
}


fmiBoolean FMU::getEventFlag()
{
	return eventFlag_;
}


void FMU::setEventFlag( fmiBoolean flag )
{
	eventFlag_ = flag;
}


size_t FMU::nStates() const
{
	return nStateVars_;
}


size_t FMU::nEventInds() const
{
	return nEventInds_;
}


size_t FMU::nValueRefs() const
{
	return nValueRefs_;
}


void FMU::logger( fmiStatus status, const string& msg ) const
{
	functions.logger( instance_, instanceName_.c_str(), status, "?", msg.c_str() );
}


void FMU::logger( fmiStatus status, const char* msg ) const
{
	functions.logger( instance_, instanceName_.c_str(), status, "?", msg );
}


void FMU::logger( fmiComponent m, fmiString instanceName,
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
