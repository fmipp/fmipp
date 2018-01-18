// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file FMUModelExchange_v1.cpp
 */
#include <assert.h>
#include <set>
#include <sstream>
#include <iostream>
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

#include "import/base/include/FMUModelExchange_v1.h"
#include "import/base/include/ModelDescription.h"
#include "import/base/include/ModelManager.h"
#include "import/base/include/CallbackFunctions.h"


using namespace std;

namespace fmi_1_0 {


// Constructor. Loads the FMU via the model manager (if needed).
FMUModelExchange::FMUModelExchange( const string& fmuDirUri,
		const string& modelIdentifier,
		const fmiBoolean loggingOn,
		const fmiBoolean stopBeforeEvent,
		const fmiTime eventSearchPrecision,
		const IntegratorType type ) :
	FMUModelExchangeBase( loggingOn ),
	instance_( 0 ),
	nStateVars_( numeric_limits<size_t>::quiet_NaN() ),
	nEventInds_( numeric_limits<size_t>::quiet_NaN() ),
	nValueRefs_( numeric_limits<size_t>::quiet_NaN() ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision ),
	intStates_( 0 ),
	intDerivatives_( 0 ),
	time_( numeric_limits<fmiReal>::quiet_NaN() ),
	tnextevent_( numeric_limits<fmiReal>::quiet_NaN() ),
	lastEventTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	tstart_( numeric_limits<fmiReal>::quiet_NaN() ),
	tlaststop_( numeric_limits<fmiReal>::quiet_NaN() ),
	eventinfo_( 0 ),
	eventsind_( 0 ),
	preeventsind_( 0 ),
	stateEvent_( fmiFalse ),
	timeEvent_( fmiFalse ),
	raisedEvent_( fmiFalse ),
	eventFlag_( fmiFalse ),
	intEventFlag_( fmiFalse ),
	lastStatus_( fmiOK ),
	upcomingEvent_( fmiFalse )
{
	// Get the model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Load the FMU.
	FMUType fmuType = invalid;
	ModelManager::LoadFMUStatus loadStatus = manager.loadFMU( modelIdentifier, fmuDirUri, loggingOn, fmuType );

	if ( ( ModelManager::success != loadStatus ) && ( ModelManager::duplicate != loadStatus ) ) { // Loading failed.
		stringstream message;
		message << "unable to load FMU (model identifier = '" << modelIdentifier
			<< "', FMU dir URI = '" << fmuDirUri << "')";			
		cerr << message.str() << endl;
		return;
	} else if ( fmi_1_0_me != fmuType ) { // Wrong type of FMU.
		cerr << "wrong type of FMU" << endl;
		return;
	}

	// Retrieve bare FMU from model manager.
	fmu_ = manager.getModel( modelIdentifier );
	
	// Set default callback functions.
	using namespace callback;
	callbacks_.logger = loggingOn ? verboseLogger : succinctLogger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;

	if ( 0 != fmu_ ) {
		readModelDescription();
		integrator_->initialize();
		integrator_->setType( type );
	}
}


// Constructor. Requires the FMU to be already loaded (via the model manager).
FMUModelExchange::FMUModelExchange( const string& modelIdentifier,
		const bool loggingOn,
		const bool stopBeforeEvent,
		const fmiTime eventSearchPrecision,
		const IntegratorType type ) :
	FMUModelExchangeBase( loggingOn ),
	instance_( 0 ),
	nStateVars_( numeric_limits<size_t>::quiet_NaN() ),
	nEventInds_( numeric_limits<size_t>::quiet_NaN() ),
	nValueRefs_( numeric_limits<size_t>::quiet_NaN() ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision ),
	intStates_( 0 ),
	intDerivatives_( 0 ),
	time_( numeric_limits<fmiReal>::quiet_NaN() ),
	tnextevent_( numeric_limits<fmiReal>::quiet_NaN() ),
	lastEventTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	tstart_( numeric_limits<fmiReal>::quiet_NaN() ),
	tlaststop_( numeric_limits<fmiReal>::quiet_NaN() ),
	eventinfo_( 0 ),
	eventsind_( 0 ),
	preeventsind_( 0 ),
	stateEvent_( fmiFalse ),
	timeEvent_( fmiFalse ),
	raisedEvent_( fmiFalse ),
	eventFlag_( fmiFalse ),
	intEventFlag_( fmiFalse ),
	lastStatus_( fmiOK ),
	upcomingEvent_( fmiFalse )
{
	// Get the model manager.
	ModelManager& manager = ModelManager::getModelManager();
	
	// Retrieve bare FMU from model manager.
	fmu_ = manager.getModel( modelIdentifier );

	// Set default callback functions.
	using namespace callback;
	callbacks_.logger = loggingOn ? verboseLogger : succinctLogger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;

	if ( 0 != fmu_ ) {
		readModelDescription();
		integrator_->initialize();
		integrator_->setType( type );
	}
}


FMUModelExchange::FMUModelExchange( const FMUModelExchange& fmu ) :
	FMUModelExchangeBase( fmu.loggingOn_ ),
	instance_( 0 ),
	fmu_( fmu.fmu_ ),
	callbacks_( fmu.callbacks_ ),
	nStateVars_( fmu.nStateVars_ ),
	nEventInds_( fmu.nEventInds_ ),
	nValueRefs_( fmu.nValueRefs_ ),
	varMap_( fmu.varMap_ ),
	varTypeMap_( fmu.varTypeMap_ ),
	stopBeforeEvent_( fmu.stopBeforeEvent_ ),
	eventSearchPrecision_( fmu.eventSearchPrecision_ ),
	intStates_( 0 ),
	intDerivatives_( 0 ),
	time_( numeric_limits<fmiReal>::quiet_NaN() ),
	tnextevent_( numeric_limits<fmiReal>::quiet_NaN() ),
	lastEventTime_( numeric_limits<fmiReal>::quiet_NaN() ),
	tstart_( numeric_limits<fmiReal>::quiet_NaN() ),
	tlaststop_( numeric_limits<fmiReal>::quiet_NaN() ),
	eventinfo_( 0 ),
	eventsind_( 0 ),
	preeventsind_( 0 ),
	stateEvent_( fmiFalse ),
	timeEvent_( fmiFalse ),
	raisedEvent_( fmiFalse ),
	eventFlag_( fmiFalse ),
	intEventFlag_( fmiFalse ),
	lastStatus_( fmiOK ),
	upcomingEvent_( fmiFalse )
{
	if ( 0 != fmu_ ){
		// Initialize integrator.
		integrator_->initialize();
		integrator_->setType( fmu.integrator_->getProperties().type );
	}
}


FMUModelExchange::~FMUModelExchange()
{
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
	providesJacobian_ = false;

	const Properties& modelVariables = description->getModelVariables();

	Properties::const_iterator itVar = modelVariables.begin();
	Properties::const_iterator itEnd = modelVariables.end();

	// List of all variable names -> check if names are unique.
	set<string> allVariableNames;
	pair< set<string>::iterator, bool > varNamesInsert;

	// List of all variable value references -> check if value references are unique.
	set<fmiValueReference> allVariableValRefs; 
	pair< set<fmiValueReference>::iterator, bool > varValRefsInsert;

	for ( ; itVar != itEnd; ++itVar )
	{
		const Properties& varAttributes = getAttributes( itVar );

		string varName = varAttributes.get<string>( "name" );
		fmiValueReference varValRef = varAttributes.get<fmiValueReference>( "valueReference" );

		varNamesInsert = allVariableNames.insert( varName );
		if ( false == varNamesInsert.second ) { // Check if variable name is unique.
			string message = string( "multiple definitions of variable name '" ) +
				varName + string( "' found" );
			logger( fmiWarning, "WARNING", message );
		}

		varValRefsInsert = allVariableValRefs.insert( varValRef );
		if ( false == varValRefsInsert.second ) { // Check if value reference is unique.
			stringstream message;
			message << "multiple definitions of value reference '"
				<< varValRef << "' found";
			logger( fmiWarning, "WARNING", message.str() );
		}

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

	if ( fmu_->description->hasDefaultExperiment() ){
		Integrator::Properties properties = integrator_->getProperties();
		double startTime;
		double stopTime;     // \FIXME: currently unused
		double tolerance;
		double stepSize;     // \FIXME: currently unused
		fmu_->description->getDefaultExperiment( startTime, stopTime, tolerance,
								 stepSize );
		if ( tolerance == tolerance ){
			properties.reltol = properties.abstol = tolerance;
			integrator_->setProperties( properties );
		}
		if ( startTime == startTime ) {
			time_ = startTime;
		} else {
			time_ = 0.0;
			}
	} else {
		time_ = 0.0;
	}

	nValueRefs_ = varMap_.size();
}


FMIVariableType FMUModelExchange::getType( const string& variableName ) const
{
	map<string,FMIVariableType>::const_iterator it = varTypeMap_.find( variableName );

	if ( it == varTypeMap_.end() ) {
		string ret = variableName + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return fmiTypeUnknown;
	}

	return it->second;
}


fmiStatus FMUModelExchange::instantiate( const string& instanceName )
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

	instance_ = fmu_->functions->instantiateModel( instanceName_.c_str(),
		guid.c_str(), callbacks_, loggingOn_ );

	if ( 0 == instance_ ) return lastStatus_ = fmiError;

	lastStatus_ = fmu_->functions->setDebugLogging( instance_, loggingOn_ );

	return lastStatus_;
}


fmiStatus FMUModelExchange::initialize( bool toleranceDefined, double tolerance )
{
	// NB: If instance_ != 0 then also fmu_ != 0.
	if ( 0 == instance_ ) return fmiError;

	if ( true == toleranceDefined ) {
		stringstream message;
		message << "initialize FMU with tolerance = " << tolerance;
		logger( fmiOK, "INFO", message.str() );
	}

	// Basic settings.
	fmu_->functions->setTime( instance_, time_ );
	lastStatus_ = fmu_->functions->initialize( instance_, static_cast<fmiBoolean>( toleranceDefined ), tolerance, eventinfo_ );

	if ( fmiTrue == eventinfo_->upcomingTimeEvent ) {
		tnextevent_ = eventinfo_->nextEventTime;
	}

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
	if (!fmu_) {
		return fmiFatal;
	} else {
		return lastStatus_;
	}
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


fmiReal FMUModelExchange::integrate( fmiReal tend, double deltaT )
{
	// if there are no continuous states, skip integration
	if ( nStateVars_ == 0 ){
		if ( stopBeforeEvent_ ){
			// in the case of stopBeforeEvent, completedIntegratorStep is called at the
			// beginning of the integration reather than the end
			// also event handling is done before the actual integration
			completedIntegratorStep();
			if ( upcomingEvent_ ){
				handleEvents();
				saveEventIndicators();
				upcomingEvent_ = fmiFalse;
			}
		}

		// determine wether a time event will happen in the time horizon we want to integrate
		timeEvent_ = ( fmiTrue == eventinfo_->upcomingTimeEvent ) && eventinfo_->nextEventTime <= tend;
		if ( timeEvent_ ){
			tend = eventinfo_->nextEventTime;
		}
		setTime( tend );

		stateEvent_ = checkStateEvent();
		if ( !stopBeforeEvent_ ){
			completedIntegratorStep();
			if ( timeEvent_ || callEventUpdate_ || stateEvent_ ){
				handleEvents();
				saveEventIndicators();
			}
		} else{
			// set a flag so the eventhandling will be done at the beginning of the next step
			if (  timeEvent_ || callEventUpdate_ || stateEvent_ )
				upcomingEvent_ = fmiTrue;
		}
		return( tend );
	}

	// if we stopped because of an event, we have to trigger and handle
	// it before we start integrating again
	if ( stopBeforeEvent_ && upcomingEvent_ )
		stepOverEvent();

	// check whether time events prevent the integration to tend and adjust tend
	// in case it is too big
	timeEvent_ = checkTimeEvent() && getTimeEvent() <= tend;
	if ( timeEvent_ ) tend = getTimeEvent() - eventSearchPrecision_/2.0;

	// save the current event indicators for the integrator
	saveEventIndicators();

	// integrate the fmu. Receive informations about state and time events
	Integrator::EventInfo eventInfo = integrator_->integrate( ( tend - time_ ), deltaT, eventSearchPrecision_ );

	// update the event flags
	stateEvent_= eventInfo.stateEvent;

	// \TODO: respond to terminateSimulation = true

	if ( eventInfo.stepEvent )
		// make event iterations
		handleEvents();

	else if ( stateEvent_ ){
		// ask the integrator for an possibly small interval containing the eventTime
		integrator_->getEventHorizon( time_, tend_ );
		if ( ! stopBeforeEvent_ ){
			// trigger the event
			stepOverEvent();
		} else{
			// set a flag so the events are handled at the beginning of the next integrate call
			upcomingEvent_ = fmiTrue;
		}
	}
	else if ( timeEvent_ ){
		// Some FMUs require exactly the time of the time event when eventUpdate is
		// called. Quick fix: Setting tend_ to the event time introduces a non-
		// symmetric epsilon environment at the event but allows an exact event 
		// time when the event update function is called.
		tend_ = getTimeEvent();
		if ( !stopBeforeEvent_ )
			stepOverEvent();
		else
			upcomingEvent_ = fmiTrue;
	}

	eventFlag_ = timeEvent_ || stateEvent_ || upcomingEvent_ || eventInfo.stepEvent;
	return time_;
}


fmiBoolean FMUModelExchange::stepOverEvent()
{
	if ( !stateEvent_ && !timeEvent_ )
		return false;
	getContinuousStates( intStates_ );
	getDerivatives( intDerivatives_ );
	for ( unsigned int i = 0; i < nStateVars_; i++ ){
	        intStates_[i] += ( tend_ - time_ )*intDerivatives_[ i ];
	}
	setTime( tend_ );
	setContinuousStates( intStates_ );

	completedIntegratorStep();
	handleEvents();

	upcomingEvent_ = false;

	saveEventIndicators();
	return true;
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
	fmiBoolean stateEvent = DynamicalSystem::checkStateEvent();

	intEventFlag_ |= stateEvent;
	eventFlag_    |= stateEvent;

	return stateEvent;
}


fmiBoolean FMUModelExchange::checkTimeEvent()
{
	if ( fmiTrue == eventinfo_->upcomingTimeEvent ) {
		tnextevent_ = eventinfo_->nextEventTime;
	} else {
		tnextevent_ = numeric_limits<fmiTime>::infinity();
	}

	return eventinfo_->upcomingTimeEvent;
}


bool FMUModelExchange::checkStepEvent()
{
	completedIntegratorStep();
	return ( callEventUpdate_ == fmiTrue );
}


fmiStatus FMUModelExchange::resetEventIndicators()
{
	fmiStatus status1 = getEventIndicators( preeventsind_ );
	fmiStatus status2 = getEventIndicators( eventsind_ );

	return lastStatus_ = ( ( ( status1 == fmiOK ) && ( status2 == fmiOK ) ) ? fmiOK : fmiFatal ) ;
}



void FMUModelExchange::handleEvents()
{
	eventinfo_->iterationConverged = fmiFalse;
	while ( fmiFalse == eventinfo_->iterationConverged )
		fmu_->functions->eventUpdate( instance_, fmiTrue, eventinfo_ );
}


fmiStatus FMUModelExchange::completedIntegratorStep()
{
	// Inform the model about an accepted step.
	return lastStatus_ = fmu_->functions->completedIntegratorStep( instance_, &callEventUpdate_ );
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


void FMUModelExchange::resetEventFlags()
{
	eventFlag_ = fmiFalse;
	intEventFlag_ = fmiFalse;
	timeEvent_ = fmiFalse;
	stateEvent_ = fmiFalse;
	upcomingEvent_ = fmiFalse;
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

const ModelDescription* FMUModelExchange::getModelDescription() const
{
	assert(getLastStatus() != fmiOK || fmu_);
	if (fmu_) {
		assert(fmu_->description != NULL);
		return fmu_->description;
	} else {
		return NULL;
	}
}

void FMUModelExchange::logger( fmiStatus status, const string& category, const string& msg ) const
{
	callbacks_.logger( instance_, instanceName_.c_str(), status, category.c_str(), msg.c_str() );
}


void FMUModelExchange::logger( fmiStatus status, const char* category, const char* msg ) const
{
	callbacks_.logger( instance_, instanceName_.c_str(), status, category, msg );
}


fmiStatus FMUModelExchange::setCallbacks( me::fmiCallbackLogger logger,
		me::fmiCallbackAllocateMemory allocateMemory,
		me::fmiCallbackFreeMemory freeMemory )
{
	if ( ( 0 == logger ) || ( 0 == allocateMemory ) || ( 0 == freeMemory ) ) {
		this->logger( fmiError, "ERROR", "callback function pointer(s) invalid" );
		return fmiError;
	}

	callbacks_.logger = logger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;

	return fmiOK;
}


void
FMUModelExchange::sendDebugMessage( const std::string& msg ) const
{
	logger( fmiOK, "DEBUG", msg );
}


} // namespace fmi_1_0
