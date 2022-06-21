// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
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
FMUModelExchange::FMUModelExchange( const fmippString& fmuDirUri,
	const fmippString& modelIdentifier,
	const fmippBoolean loggingOn,
	const fmippBoolean stopBeforeEvent,
	const fmippTime eventSearchPrecision,
	const IntegratorType type ) :
		FMUModelExchangeBase( loggingOn ),
		instance_( 0 ),
		nStateVars_( numeric_limits<fmippSize>::quiet_NaN() ),
		nEventInds_( numeric_limits<fmippSize>::quiet_NaN() ),
		nValueRefs_( numeric_limits<fmippSize>::quiet_NaN() ),
		stopBeforeEvent_( (fmiBoolean) stopBeforeEvent ),
		eventSearchPrecision_( eventSearchPrecision ),
		time_( numeric_limits<fmippTime>::quiet_NaN() ),
		tnextevent_( numeric_limits<fmippTime>::quiet_NaN() ),
		lastEventTime_( numeric_limits<fmippTime>::quiet_NaN() ),
		tstart_( numeric_limits<fmippTime>::quiet_NaN() ),
		tlaststop_( numeric_limits<fmippTime>::quiet_NaN() ),
		intStates_( 0 ),
		intDerivatives_( 0 ),
		eventinfo_( 0 ),
		eventsind_( 0 ),
		preeventsind_( 0 ),
		stateEvent_( fmiFalse ),
		timeEvent_( fmiFalse ),
		raisedEvent_( fmiFalse ),
		eventFlag_( fmiFalse ),
		intEventFlag_( fmiFalse ),
		callEventUpdate_( fmiFalse ),
		upcomingEvent_( fmiFalse ),
		lastStatus_( fmiOK )
{
	// Get the model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Load the FMU.
	FMUType loadedFMUType = invalid;
	string loadedModelIdentifier;

	ModelManager::LoadFMUStatus loadStatus = manager.loadFMU( fmuDirUri, loggingOn, loadedFMUType, loadedModelIdentifier );

	if ( ( ModelManager::success != loadStatus ) && ( ModelManager::duplicate != loadStatus ) ) { // Loading failed.
		stringstream message;
		message << "unable to load FMU (model identifier = '" << modelIdentifier
			<< "', FMU dir URI = '" << fmuDirUri << "')";
		cerr << message.str() << endl;
		lastStatus_ = fmiFatal;
		return;
	} else if ( fmi_1_0_me != loadedFMUType ) { // Wrong type of FMU.
		cerr << "wrong type of FMU (expected FMI ME v1)" << endl;
		lastStatus_ = fmiFatal;
		return;
	}

	// Retrieve bare FMU from model manager.
	fmu_ = manager.getModel( loadedModelIdentifier );

	// Set default callback functions.
	using namespace callback;
	callbacks_.logger = loggingOn ? verboseLogger : succinctLogger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;

	// Issue a warning in case the model identifiers do not match.
	if ( modelIdentifier != loadedModelIdentifier ) {
		lastStatus_ = fmiWarning;
		stringstream message;
		message << "model identifier of loaded FMU (" << loadedModelIdentifier << ") "
			<< "does not match mode identifier provided by user (" << modelIdentifier << ")";
		logger( fmiWarning, "WARNING", message.str() );
	}

	if ( 0 != fmu_ ) {
		readModelDescription();
		if ( 0 != nStateVars_ ) {
			integrator_->initialize();
			integrator_->setType( type );
		}
	} else {
		lastStatus_ = fmiFatal;
	}
}

// Constructor. Requires the FMU to be already loaded (via the model manager).
FMUModelExchange::FMUModelExchange( const fmippString& modelIdentifier,
	const fmippBoolean loggingOn,
	const fmippBoolean stopBeforeEvent,
	const fmippTime eventSearchPrecision,
	const IntegratorType type ) :
		FMUModelExchangeBase( loggingOn ),
		instance_( 0 ),
		nStateVars_( numeric_limits<fmippSize>::quiet_NaN() ),
		nEventInds_( numeric_limits<fmippSize>::quiet_NaN() ),
		nValueRefs_( numeric_limits<fmippSize>::quiet_NaN() ),
		stopBeforeEvent_( (fmiBoolean) stopBeforeEvent ),
		eventSearchPrecision_( eventSearchPrecision ),
		time_( numeric_limits<fmippTime>::quiet_NaN() ),
		tnextevent_( numeric_limits<fmippTime>::quiet_NaN() ),
		lastEventTime_( numeric_limits<fmippTime>::quiet_NaN() ),
		tstart_( numeric_limits<fmippTime>::quiet_NaN() ),
		tlaststop_( numeric_limits<fmippTime>::quiet_NaN() ),
		intStates_( 0 ),
		intDerivatives_( 0 ),
		eventinfo_( 0 ),
		eventsind_( 0 ),
		preeventsind_( 0 ),
		stateEvent_( fmiFalse ),
		timeEvent_( fmiFalse ),
		raisedEvent_( fmiFalse ),
		eventFlag_( fmiFalse ),
		intEventFlag_( fmiFalse ),
		callEventUpdate_( fmiFalse ),
		upcomingEvent_( fmiFalse ),
		lastStatus_( fmiOK )
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
		if ( 0 != nStateVars_ ) {
			integrator_->initialize();
			integrator_->setType( type );
		}
	} else {
		lastStatus_ = fmiFatal;
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
		time_( numeric_limits<fmippTime>::quiet_NaN() ),
		tnextevent_( numeric_limits<fmippTime>::quiet_NaN() ),
		lastEventTime_( numeric_limits<fmippTime>::quiet_NaN() ),
		tstart_( numeric_limits<fmippTime>::quiet_NaN() ),
		tlaststop_( numeric_limits<fmippTime>::quiet_NaN() ),
		intStates_( 0 ),
		intDerivatives_( 0 ),
		eventinfo_( 0 ),
		eventsind_( 0 ),
		preeventsind_( 0 ),
		stateEvent_( fmiFalse ),
		timeEvent_( fmiFalse ),
		raisedEvent_( fmiFalse ),
		eventFlag_( fmiFalse ),
		intEventFlag_( fmiFalse ),
		callEventUpdate_( fmiFalse ),
		upcomingEvent_( fmiFalse ),
		lastStatus_( fmiOK )
{
	if ( 0 != fmu_ ){
		if ( 0 != nStateVars_ ) {
			// Initialize integrator.
			integrator_->initialize();
			integrator_->setType( fmu.integrator_->getProperties().type );
		}
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
	set<fmippString> allVariableNames;
	pair< set<fmippString>::iterator, fmippBoolean > varNamesInsert;

	// List of all variable value references -> check if value references are unique.
	set<fmippValueReference> allVariableValRefs;
	pair< set<fmippValueReference>::iterator, fmippBoolean > varValRefsInsert;

	for ( ; itVar != itEnd; ++itVar )
	{
		const Properties& varAttributes = getAttributes( itVar );

		fmippString varName = varAttributes.get<fmippString>( "name" );
		fmippValueReference varValRef = varAttributes.get<fmippValueReference>( "valueReference" );

		varNamesInsert = allVariableNames.insert( varName );

		if ( false == varNamesInsert.second ) { // Check if variable name is unique.
			fmippString message = fmippString( "multiple definitions of variable name '" ) +
				varName + fmippString( "' found" );
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
			varTypeMap_.insert( make_pair( varName, fmippTypeReal ) );
		} else if ( hasChild( itVar, "Integer" ) ) {
			varTypeMap_.insert( make_pair( varName, fmippTypeInteger ) );
		} else if ( hasChild( itVar, "Boolean" ) ) {
			varTypeMap_.insert( make_pair( varName, fmippTypeBoolean ) );
		} else if ( hasChild( itVar, "String" ) ) {
			varTypeMap_.insert( make_pair( varName, fmippTypeString ) );
		} else {
			varTypeMap_.insert( make_pair( varName, fmippTypeUnknown ) );
		}
	}
	if ( fmu_->description->hasDefaultExperiment() ){
		Integrator::Properties properties = integrator_->getProperties();

		fmippTime startTime;
		fmippTime stopTime;     // \FIXME: currently unused
		fmippTime tolerance;
		fmippTime stepSize;     // \FIXME: currently unused

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

FMIPPVariableType FMUModelExchange::getType( const fmippString& variableName ) const
{
	map<fmippString,FMIPPVariableType>::const_iterator it = varTypeMap_.find( variableName );
	if ( it == varTypeMap_.end() ) {
		fmippString ret = variableName + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return fmippTypeUnknown;
	}
	return it->second;
}

fmippStatus FMUModelExchange::instantiate( const fmippString& instanceName )
{
	instanceName_ = instanceName;

	if ( fmu_ == 0 ) {
		lastStatus_ = fmiError;
		return (fmippStatus) lastStatus_;
	}

	time_ = 0.;
	tnextevent_ = numeric_limits<fmippTime>::infinity();

	// Memory allocation.
	if ( nEventInds_ > 0 ) {
		eventsind_ = new fmiReal[nEventInds_];
		preeventsind_ = new fmiReal[nEventInds_];
	}

	if ( nStateVars_ > 0 ) {
		intStates_ = new fmiReal[nStateVars_];
		intDerivatives_ = new fmiReal[nStateVars_];
	}

	for ( fmippSize i = 0; i < nEventInds_; ++i ) {
		eventsind_[i] = 0;
		preeventsind_[i] = 0;
	}

	eventinfo_ = new fmiEventInfo;

	const fmippString& guid = fmu_->description->getGUID();

	instance_ = fmu_->functions->instantiateModel( instanceName_.c_str(),
		guid.c_str(), callbacks_, loggingOn_ );

	if ( 0 == instance_ ) {
		lastStatus_ = fmiError;
		return (fmippStatus) lastStatus_;
	}

	lastStatus_ = fmu_->functions->setDebugLogging( instance_, loggingOn_ );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::initialize( fmippBoolean toleranceDefined, fmippReal tolerance )
{
	// NB: If instance_ != 0 then also fmu_ != 0.
	if ( 0 == instance_ ) {
		lastStatus_ = fmiError;
		return (fmippStatus) lastStatus_;
	}

	if ( true == toleranceDefined ) {
		stringstream message;
		message << "initialize FMU with tolerance = " << tolerance;
		logger( fmiOK, "INFO", message.str() );
	}

	// Basic settings.
	fmu_->functions->setTime( instance_, time_ );

	lastStatus_ = fmu_->functions->initialize( instance_, static_cast<fmippBoolean>( toleranceDefined ), tolerance, eventinfo_ );

	saveEventIndicators();

	if ( fmiTrue == eventinfo_->upcomingTimeEvent ) {
		tnextevent_ = eventinfo_->nextEventTime;
	}

	return (fmippStatus) lastStatus_;
}

fmippReal FMUModelExchange::getTime() const
{
	return time_;
}

fmippStatus FMUModelExchange::setTime( fmippReal time )
{
	time_ = time;
	// NB: If instance_ != 0 then also fmu_ != 0.
	if ( 0 != instance_ ) return (fmippStatus) fmu_->functions->setTime( instance_, time_ );

	return fmippFatal;
}

void FMUModelExchange::rewindTime( fmippReal deltaRewindTime )
{
	time_ -= deltaRewindTime;
	fmu_->functions->setTime( instance_, time_ );
	//fmu_->functions->eventUpdate( instance_, fmippFalse, eventinfo_ );
}

fmippStatus FMUModelExchange::setValue( fmippValueReference valref, const fmippReal& val )
{
	lastStatus_ = fmu_->functions->setReal( instance_, &valref, 1, &val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue( fmippValueReference valref, const fmippInteger& val )
{
	lastStatus_ = fmu_->functions->setInteger( instance_, &valref, 1, &val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue( fmippValueReference valref, const fmippBoolean& val )
{
	fmiBoolean val2 = (fmiBoolean) val;
	lastStatus_ = fmu_->functions->setBoolean( instance_, &valref, 1, &val2 );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue( fmippValueReference valref, const fmippString& val )
{
	const char* cString = val.c_str();
	lastStatus_ = fmu_->functions->setString( instance_, &valref, 1, &cString );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue(fmippValueReference* valref, const fmippReal* val, fmippSize ival)
{
	lastStatus_ = fmu_->functions->setReal(instance_, valref, ival, val);
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue(fmippValueReference* valref, const fmippInteger* val, fmippSize ival)
{
	lastStatus_ = fmu_->functions->setInteger(instance_, valref, ival, val);
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue(fmippValueReference* valref, const fmippBoolean* val, fmippSize ival)
{
	fmiBoolean* val2 = new fmiBoolean[ival];
	for ( fmippSize i = 0; i < ival; ++i ) {
		val2[i] = (fmiBoolean) val[i];
	}
	lastStatus_ = fmu_->functions->setBoolean(instance_, valref, ival, val2);
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue(fmippValueReference* valref, const fmippString* val, fmippSize ival)
{
	const char** cStrings = new const char*[ival];
	for ( fmippSize i = 0; i < ival; i++ ) {
		cStrings[i] = val[i].c_str();
	}

	lastStatus_ = fmu_->functions->setString(instance_, valref, ival, cStrings);
	delete [] cStrings;

	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue( const fmippString& name, const fmippReal& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setReal( instance_, &it->second, 1, &val );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue( const fmippString& name, const fmippInteger& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setInteger( instance_, &it->second, 1, &val );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue( const fmippString& name, const fmippBoolean& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmiBoolean val2 = (fmiBoolean) val;
	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setBoolean( instance_, &it->second, 1, &val2 );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue( const fmippString& name, const fmippString& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	const char* cString = val.c_str();
	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setString( instance_, &it->second, 1, &cString );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( fmippValueReference valref, fmippReal& val )
{
	lastStatus_ = fmu_->functions->getReal( instance_, &valref, 1, &val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( fmippValueReference valref, fmippInteger& val )
{
	lastStatus_ = fmu_->functions->getInteger( instance_, &valref, 1, &val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( fmippValueReference valref, fmippBoolean& val )
{
	fmiBoolean val2 = (fmiBoolean) val;
	lastStatus_ = fmu_->functions->getBoolean( instance_, &valref, 1, &val2 );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( fmippValueReference valref, fmippString& val )
{
	const char* cString;
	lastStatus_ = fmu_->functions->getString( instance_, &valref, 1, &cString );
	val = fmippString( cString );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( fmippValueReference* valref, fmippReal* val, fmippSize ival )
{
	lastStatus_ = fmu_->functions->getReal( instance_, valref, ival, val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( fmippValueReference* valref, fmippInteger* val, fmippSize ival )
{
	lastStatus_ = fmu_->functions->getInteger( instance_, valref, ival, val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( fmippValueReference* valref, fmippBoolean* val, fmippSize ival )
{
	fmiBoolean* val2 = new fmiBoolean[ival];
	for ( fmippSize i = 0; i < ival; ++i ) {
		val2[i] = (fmiBoolean) val[i];
	}
	lastStatus_ = fmu_->functions->getBoolean( instance_, valref, ival, val2 );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( fmippValueReference* valref, fmippString* val, fmippSize ival )
{
	const char** cStrings = 0;
	lastStatus_ = fmu_->functions->getString( instance_, valref, ival, cStrings );
	if ( 0 != cStrings ) {
		for ( fmippSize i = 0; i < ival; i++ ) {
			val[i] = fmippString( cStrings[i] );
		}
	}
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( const fmippString& name, fmippReal& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getReal( instance_, &it->second, 1, &val );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( const fmippString& name, fmippInteger& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getInteger( instance_, &it->second, 1, &val );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( const fmippString& name, fmippBoolean& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		fmiBoolean val2 = (fmiBoolean) val;
		lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, &val2 );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( const fmippString& name, fmippString& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	const char* cString;
	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getString( instance_, &it->second, 1, &cString );
		val = fmippString( cString );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return (fmippStatus) lastStatus_;
}

fmippReal FMUModelExchange::getRealValue( const fmippString& name )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmippReal val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getReal( instance_, &it->second, 1, val );
	} else {
		val[0] = numeric_limits<fmippReal>::quiet_NaN();
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return val[0];
}

fmippInteger FMUModelExchange::getIntegerValue( const fmippString& name )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmippInteger val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getInteger( instance_, &it->second, 1, val );
	} else {
		val[0] = numeric_limits<fmippInteger>::quiet_NaN();
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return val[0];
}

fmippBoolean FMUModelExchange::getBooleanValue( const fmippString& name )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmiBoolean val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, val );
	} else {
		val[0] = fmiFalse;
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return (fmippBoolean) val[0];
}

fmippString FMUModelExchange::getStringValue( const fmippString& name )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmiString val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getString( instance_, &it->second, 1, val );
	} else {
		val[0] = 0;
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}

	return fmippString( val[0] );
}

fmippStatus FMUModelExchange::getLastStatus() const
{
	if ( !fmu_ ) {
		return fmippFatal;
	} else {
		return (fmippStatus) lastStatus_;
	}
}

fmippStatus FMUModelExchange::getContinuousStates( fmippReal* val )
{
	lastStatus_ = fmu_->functions->getContinuousStates( instance_, val, nStateVars_ );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setContinuousStates( const fmippReal* val )
{
	lastStatus_ = fmu_->functions->setContinuousStates( instance_, val, nStateVars_ );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getDerivatives( fmippReal* val )
{
	lastStatus_ = fmu_->functions->getDerivatives( instance_, val, nStateVars_ );
	return (fmippStatus) lastStatus_;
}

fmippValueReference FMUModelExchange::getValueRef( const fmippString& name ) const
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find(name);
	if ( it != varMap_.end() ) {
		return it->second;
	} else {
		return fmiUndefinedValueReference;
	}
}

fmippStatus FMUModelExchange::getEventIndicators( fmippReal* eventsind )
{
	lastStatus_ = fmu_->functions->getEventIndicators(instance_, eventsind, nEventInds());
	return (fmippStatus) lastStatus_;
}

fmippReal FMUModelExchange::integrate( fmippTime tstop, unsigned int nsteps )
{
	assert( nsteps > 0 );
	fmippTime deltaT = ( tstop - getTime() ) / nsteps;
	return integrate( tstop, deltaT );
}

fmippReal FMUModelExchange::integrate( fmippTime tend, fmippTime deltaT )
{
	// If there are no continuous states, skip integration.
	if ( nStateVars_ == 0 ){

		if ( stopBeforeEvent_ ){
			// In the case of stopBeforeEvent, completedIntegratorStep is called at the
			// beginning of the integration reather than the end also event handling is
			// done before the actual integration.
			completedIntegratorStep();

			if ( upcomingEvent_ ){
				handleEvents();
				saveEventIndicators();
				upcomingEvent_ = fmiFalse;
			}
		}

		// Determine wether a time event will happen in the time horizon we want to integrate.
		timeEvent_ = ( fmiTrue == eventinfo_->upcomingTimeEvent ) && eventinfo_->nextEventTime <= tend;
		if ( timeEvent_ ){
			tend = eventinfo_->nextEventTime;
		}

		setTime( tend );

		stateEvent_ = (fmiBoolean) checkStateEvent();

		if ( !stopBeforeEvent_ ){
			completedIntegratorStep();

			if ( timeEvent_ || callEventUpdate_ || stateEvent_ ){
				handleEvents();
				saveEventIndicators();
			}
		} else{
			// Set a flag so the eventhandling will be done at the beginning of the next step.
			if (  timeEvent_ || callEventUpdate_ || stateEvent_ )
				upcomingEvent_ = fmiTrue;
		}
		return( tend );
	}

	// If we stopped because of an event, we have to trigger and handle
	// it before we start integrating again.
	if ( stopBeforeEvent_ && upcomingEvent_ )
		stepOverEvent();

	// Check whether time events prevent the integration to tend and adjust tend
	// in case it is too big.
	timeEvent_ = checkTimeEvent() && getTimeEvent() <= tend;
	if ( timeEvent_ ) tend = getTimeEvent() - eventSearchPrecision_/2.0;

	// Save the current event indicators for the integrator.
	saveEventIndicators();

	// Integrate the FMU. Receive informations about state and time events.
	Integrator::EventInfo eventInfo = integrator_->integrate( ( tend - time_ ), deltaT, eventSearchPrecision_ );

	// Update the event flags.
	stateEvent_= eventInfo.stateEvent;

	// \TODO: respond to terminateSimulation == true
	if ( eventInfo.stepEvent )
		// Make event iterations.
		handleEvents();
	else if ( stateEvent_ ){
		// Ask the integrator for an possibly small interval containing the eventTime.
		integrator_->getEventHorizon( time_, tend_ );

		if ( ! stopBeforeEvent_ ){
			// Trigger the event.
			stepOverEvent();
		} else{
			// Set a flag so the events are handled at the beginning of the next integrate call.
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

fmippBoolean FMUModelExchange::stepOverEvent()
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

	upcomingEvent_ = fmiFalse;

	saveEventIndicators();

	return true;
}

void FMUModelExchange::raiseEvent()
{
	raisedEvent_ = fmippTrue;
}

fmippBoolean FMUModelExchange::checkEvents()
{
	fmippBoolean event = checkStateEvent() || checkTimeEvent();
	return event;
}

fmippBoolean FMUModelExchange::checkStateEvent()
{
	fmippBoolean stateEvent = DynamicalSystem::checkStateEvent();
	intEventFlag_ |= (fmiBoolean) stateEvent;
	eventFlag_    |= (fmiBoolean) stateEvent;
	return stateEvent;
}

fmippBoolean FMUModelExchange::checkTimeEvent()
{
	if ( fmiTrue == eventinfo_->upcomingTimeEvent ) {
		tnextevent_ = eventinfo_->nextEventTime;
	} else {
		tnextevent_ = numeric_limits<fmippTime>::infinity();
	}

	return ( eventinfo_->upcomingTimeEvent == fmiTrue );
}

fmippBoolean FMUModelExchange::checkStepEvent()
{
	completedIntegratorStep();
	return ( callEventUpdate_ == fmiTrue );
}

fmippStatus FMUModelExchange::resetEventIndicators()
{
	fmippStatus status1 = getEventIndicators( preeventsind_ );
	fmippStatus status2 = getEventIndicators( eventsind_ );
	lastStatus_ = ( ( ( status1 == fmippOK ) && ( status2 == fmippOK ) ) ? fmiOK : fmiFatal ) ;

	return (fmippStatus) lastStatus_;
}

void FMUModelExchange::handleEvents()
{
	eventinfo_->iterationConverged = fmiFalse;
	while ( fmiFalse == eventinfo_->iterationConverged )
		fmu_->functions->eventUpdate( instance_, fmiTrue, eventinfo_ );
}

fmippStatus FMUModelExchange::completedIntegratorStep()
{
	// Inform the model about an accepted step.
	lastStatus_ = fmu_->functions->completedIntegratorStep( instance_, &callEventUpdate_ );

	return (fmippStatus) lastStatus_;
}

fmippBoolean FMUModelExchange::getIntEvent()
{
	return (fmippBoolean) intEventFlag_;
}

fmippTime FMUModelExchange::getTimeEvent()
{
	return tnextevent_;
}

fmippBoolean FMUModelExchange::getEventFlag()
{
	return (fmippBoolean) eventFlag_;
}

void FMUModelExchange::setEventFlag( fmippBoolean flag )
{
	eventFlag_ = (fmiBoolean) flag;
}

void FMUModelExchange::resetEventFlags()
{
	eventFlag_ = fmiFalse;
	intEventFlag_ = fmiFalse;
	timeEvent_ = fmiFalse;
	stateEvent_ = fmiFalse;
	upcomingEvent_ = fmiFalse;
}

fmippSize FMUModelExchange::nStates() const
{
	return nStateVars_;
}

fmippSize FMUModelExchange::nEventInds() const
{
	return nEventInds_;
}

fmippSize FMUModelExchange::nValueRefs() const
{
	return nValueRefs_;
}

const ModelDescription* FMUModelExchange::getModelDescription() const
{
	assert(getLastStatus() != fmippOK || fmu_);

	if ( fmu_ ) {
		assert(fmu_->description != NULL);
		return fmu_->description;
	} else {
		return NULL;
	}
}

void FMUModelExchange::logger( fmiStatus status, const fmippString& category, const fmippString& msg ) const
{
	callbacks_.logger( instance_, instanceName_.c_str(), status, category.c_str(), msg.c_str() );
}

void FMUModelExchange::logger( fmiStatus status, const char* category, const char* msg ) const
{
	callbacks_.logger( instance_, instanceName_.c_str(), status, category, msg );
}

fmippStatus FMUModelExchange::setCallbacks( me::fmiCallbackLogger logger,
	me::fmiCallbackAllocateMemory allocateMemory,
	me::fmiCallbackFreeMemory freeMemory )
{
	if ( ( 0 == logger ) || ( 0 == allocateMemory ) || ( 0 == freeMemory ) ) {
		this->logger( fmiError, "ERROR", "callback function pointer(s) invalid" );
		return fmippError;
	}

	callbacks_.logger = logger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;

	return fmippOK;
}

void
FMUModelExchange::sendDebugMessage( const fmippString& msg ) const
{
	logger( fmiOK, "DEBUG", msg );
}

} // namespace fmi_1_0
