// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file FMUModelExchange_v2.cpp
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
#include "common/fmi_v2.0/fmi2ModelTypes.h"
#include "common/fmi_v2.0/fmi_2.h"

#include "import/base/include/FMUModelExchange_v2.h"
#include "import/base/include/ModelDescription.h"
#include "import/base/include/ModelManager.h"
#include "import/base/include/CallbackFunctions.h"

using namespace std;

namespace fmi_2_0 {

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
		derivatives_refs_( 0 ),
		states_refs_( 0 ),
		stopBeforeEvent_( stopBeforeEvent ),
		eventSearchPrecision_( eventSearchPrecision ),
		intStates_( 0 ),
		intDerivatives_( 0 ),
		time_( numeric_limits<fmippTime>::quiet_NaN() ),
		tnextevent_( numeric_limits<fmippTime>::quiet_NaN() ),
		lastEventTime_( numeric_limits<fmippTime>::quiet_NaN() ),
		eventinfo_( 0 ),
		eventsind_( 0 ),
		preeventsind_( 0 ),
		callEventUpdate_( fmippFalse ),
		stateEvent_( fmippFalse ),
		timeEvent_( fmippFalse ),
		enterEventMode_( fmippFalse ),
		terminateSimulation_( fmippFalse ),
		upcomingEvent_( fmippFalse ),
		raisedEvent_( fmippFalse ),
		eventFlag_( fmippFalse ),
		intEventFlag_( fmippFalse ),
		lastStatus_( fmi2OK )
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
		lastStatus_ = fmi2Fatal;
		return;
	} else if ( ( fmi_2_0_me != loadedFMUType ) && ( fmi_2_0_me_and_cs != loadedFMUType ) ) { // Wrong type of FMU.
		cerr << "wrong type of FMU (expected FMI ME v2)" << endl;
		lastStatus_ = fmi2Fatal;
		return;
	}

	// Retrieve bare FMU from model manager.
	fmu_ = manager.getInstance( loadedModelIdentifier );

	// Set default callback functions.
	using namespace callback2;
	callbacks_.logger = loggingOn ? verboseLogger : succinctLogger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;
	callbacks_.stepFinished = 0; // Not used for ME, always set to NULL.
	callbacks_.componentEnvironment = 0; // Set to NULL as default.

	// Issue a warning in case the model identifiers do not match.
	if ( modelIdentifier != loadedModelIdentifier ) {
		lastStatus_ = fmi2Warning;
		stringstream message;
		message << "model identifier of loaded FMU (" << loadedModelIdentifier << ") "
			<< "does not match mode identifier provided by user (" << modelIdentifier << ")";
		logger( fmi2Warning, "WARNING", message.str() ); 
	}

	if ( 0 != fmu_ ) {
		readModelDescription();
		if ( 0 != nStateVars_ ) {
			integrator_->initialize();
			integrator_->setType( type );
		}
	} else {
		lastStatus_ = fmi2Fatal;
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
		derivatives_refs_( 0 ),
		states_refs_( 0 ),
		stopBeforeEvent_( stopBeforeEvent ),
		eventSearchPrecision_( eventSearchPrecision ),
		intStates_( 0 ),
		intDerivatives_( 0 ),
		time_( numeric_limits<fmippTime>::quiet_NaN() ),
		tnextevent_( numeric_limits<fmippTime>::quiet_NaN() ),
		lastEventTime_( numeric_limits<fmippTime>::quiet_NaN() ),
		eventinfo_( 0 ),
		eventsind_( 0 ),
		preeventsind_( 0 ),
		callEventUpdate_( fmippFalse ),
		stateEvent_( fmippFalse ),
		timeEvent_( fmippFalse ),
		enterEventMode_( fmippFalse ),
		terminateSimulation_( fmippFalse ),
		upcomingEvent_( fmippFalse ),
		raisedEvent_( fmippFalse ),
		eventFlag_( fmippFalse ),
		intEventFlag_( fmippFalse ),
		lastStatus_( fmi2OK )
{
	// Get the model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Retrieve bare FMU from model manager.
	fmu_ = manager.getInstance( modelIdentifier );

	// Set default callback functions.
	using namespace callback2;
	callbacks_.logger = loggingOn ? verboseLogger : succinctLogger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;
	callbacks_.stepFinished = 0; // Not used for ME, always set to NULL.
	callbacks_.componentEnvironment = 0; // Set to NULL as default.

	if ( 0 != fmu_ ) {
		readModelDescription();
		if ( 0 != nStateVars_ ) {
			integrator_->initialize();
			integrator_->setType( type );
		}
	} else {
		lastStatus_ = fmi2Fatal;
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
		derivatives_refs_( 0 ),
		states_refs_( 0 ),
		varMap_( fmu.varMap_ ),
		varTypeMap_( fmu.varTypeMap_ ),
		stopBeforeEvent_( fmu.stopBeforeEvent_ ),
		eventSearchPrecision_( fmu.eventSearchPrecision_ ),
		intStates_( 0 ),
		intDerivatives_( 0 ),
		time_( numeric_limits<fmippTime>::quiet_NaN() ),
		tnextevent_( numeric_limits<fmippTime>::quiet_NaN() ),
		lastEventTime_( numeric_limits<fmippTime>::quiet_NaN() ),
		eventinfo_( 0 ),
		eventsind_( 0 ),
		preeventsind_( 0 ),
		callEventUpdate_( fmippFalse ),
		stateEvent_( fmippFalse ),
		timeEvent_( fmippFalse ),
		enterEventMode_( fmippFalse ),
		terminateSimulation_( fmippFalse ),
		upcomingEvent_( fmippFalse ),
		raisedEvent_( fmippFalse ),
		eventFlag_( fmippFalse ),
		intEventFlag_( fmippFalse ),
		lastStatus_( fmi2OK )
{
	if ( 0 != fmu_ ){
		if ( 0 != nStateVars_ ) {
			// allocate memory for the integrator
			integrator_->initialize();
			// create the stepper
			integrator_->setType( fmu.integrator_->getProperties().type );
		}
	}
}

FMUModelExchange::~FMUModelExchange()
{
	// if instance is NULL, eventinfo_ must be NULL too:
	assert(instance_ || eventinfo_ == NULL);

	if ( eventsind_ )        delete[] eventsind_;
	if ( preeventsind_ )     delete[] preeventsind_;
	if ( intStates_ )        delete[] intStates_;
	if ( intDerivatives_ )   delete[] intDerivatives_;

	if ( derivatives_refs_ ) delete[] derivatives_refs_;
	if ( states_refs_ )      delete[] states_refs_;

	if ( instance_ ) {
		delete eventinfo_;

		fmu_->functions->terminate( instance_ );
#ifndef MINGW
		/// \bug This call causes a seg fault with OpenModelica FMUs under MINGW ...
		fmu_->functions->freeInstance( instance_ );
#endif
	}
}

void FMUModelExchange::readModelDescription()
{
	assert(derivatives_refs_ == NULL); // Will be initialized
	assert(states_refs_ == NULL); // Will be initialized

	using namespace ModelDescriptionUtilities;
	typedef ModelDescription::Properties Properties;

	const ModelDescription* description = fmu_->description;

	nStateVars_       = description->getNumberOfContinuousStates();
	nEventInds_       = description->getNumberOfEventIndicators();
	providesJacobian_ = description->providesJacobian();

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
			logger( fmi2Warning, "WARNING", message );
		}

		varValRefsInsert = allVariableValRefs.insert( varValRef );
		if ( false == varValRefsInsert.second ) { // Check if value reference is unique.
			stringstream message;
			message << "multiple definitions of value reference '"
				<< varValRef << "' found";
			logger( fmi2Warning, "WARNING", message.str() );
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
		fmippTime stopTime;
		fmippReal tolerance;
		fmippTime stepSize; // \FIXME: currently unused
		fmu_->description->getDefaultExperiment( startTime, stopTime, tolerance, stepSize );
		if ( tolerance == tolerance ) {
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

	// get the references of the states and derivatives for the Jacobian
	derivatives_refs_ = new fmippValueReference[nStateVars_];
	states_refs_ = new fmippValueReference[nStateVars_];
	if ( nStateVars_> 0 )
		description->getStatesAndDerivativesReferences( states_refs_, derivatives_refs_ );
}

FMIPPVariableType FMUModelExchange::getType( const fmippString& variableName ) const
{
	map<fmippString,FMIPPVariableType>::const_iterator it = varTypeMap_.find( variableName );

	if ( it == varTypeMap_.end() ) {
		fmippString ret = variableName + fmippString( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		return fmippTypeUnknown;
	}

	return it->second;
}

fmippStatus FMUModelExchange::instantiate( const fmippString& instanceName )
{
	// Assert no duplicate initialization:
	assert( eventsind_ == NULL );
	assert( preeventsind_ == NULL );
	assert( intStates_ == NULL );
	assert( intDerivatives_ == NULL );
	assert( eventinfo_ == NULL );

	instanceName_ = instanceName;

	if ( fmu_ == 0 ) {
		lastStatus_ = fmi2Error;
		return (fmippStatus) lastStatus_;
	}

	time_ = 0.;
	tnextevent_ = numeric_limits<fmippTime>::infinity();

	// Memory allocation.
	if ( nEventInds_ > 0 ) {
		eventsind_ = new fmi2Real[nEventInds_];
		preeventsind_ = new fmi2Real[nEventInds_];
	}

	if ( nStateVars_ > 0 ) {
		intStates_ = new fmi2Real[nStateVars_];
		intDerivatives_ = new fmi2Real[nStateVars_];
	}

	for ( fmippSize i = 0; i < nEventInds_; ++i ) {
		eventsind_[i] = 0;
		preeventsind_[i] = 0;
	}

	eventinfo_ = new fmi2EventInfo;

	const fmippString& guid = fmu_->description->getGUID();

	fmi2Boolean visible = fmi2False; // visible = false means that the FMU is executed in batch mode

	// call instantiate
	instance_ = fmu_->functions->instantiate( instanceName_.c_str(), fmi2ModelExchange,
		guid.c_str(), fmu_->fmuResourceLocation.c_str(), &callbacks_, visible, loggingOn_ );

	// check whether instantiate returned a non trivial object
	if ( 0 == instance_ ){
		lastStatus_ = fmi2Error;
		return (fmippStatus) lastStatus_;
	}

	/// \FIXME retrieve options for debug logging as defined in fmiModelDescription.LogCategories
	fmippSize nCategories = 0;
	char** categories = NULL;

	lastStatus_ = fmu_->functions->setDebugLogging( instance_,
							loggingOn_,
							nCategories,
							categories );

	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::initialize( fmippBoolean toleranceDefined, double tolerance )
{
	// NB: If instance_ != 0 then also fmu_ != 0.
	if ( 0 == instance_ ) {
		lastStatus_ = fmi2Error;
		return (fmippStatus) lastStatus_;
	}

	fmi2Boolean stopTimeDefined = fmi2False;
	fmippTime stopTime = 1;

	/* In case the tolerance is not defined, use the default experiment for setupExperiment if available. Open questions:
	 *
	 *   * What happens if the default stop time is available but we want to integrate past that?
	 *   * What if the tolerance is later changed?
	 *
	 */
	if ( fmu_->description->hasDefaultExperiment() ){
		Integrator::Properties properties = integrator_->getProperties();
		fmippTime startTime;
		fmippTime defaultStopTime;
		fmippReal defaultTolerance;
		fmippTime stepSize;           // \FIXME: currently unused
		fmu_->description->getDefaultExperiment( startTime, defaultStopTime, defaultTolerance,
							 stepSize );
		if ( defaultTolerance == defaultTolerance ){ // Check that value is not NaN.
			// Redefine in case the tolerance has not been defined by the user.
			if ( false == toleranceDefined ) {
				toleranceDefined = true;
				tolerance = static_cast<fmippReal>( defaultTolerance );
			}
		}
		if ( defaultStopTime == defaultStopTime ) { // Check that value is not NaN.
			stopTimeDefined = fmi2True;
			stopTime = defaultStopTime;
		}
	}

	if ( true == toleranceDefined ) {
		stringstream message;
		message << "initialize FMU with tolerance = " << tolerance;
		logger( fmi2OK, "INFO", message.str() );
	}

	lastStatus_ = fmu_->functions->setupExperiment( instance_,
		static_cast<fmi2Boolean>( toleranceDefined ), static_cast<fmi2Real>( tolerance ),
		time_, stopTimeDefined, stopTime );

	lastStatus_ = fmu_->functions->enterInitializationMode( instance_ );

	// exit initialization mode and enter discrete time mode
	lastStatus_ = fmu_->functions->exitInitializationMode( instance_ );

	// call newDiscreteStates to get the eventinfo
	fmu_->functions->newDiscreteStates( instance_, eventinfo_ );

	saveEventIndicators();

	if ( fmi2True == eventinfo_->nextEventTimeDefined ) {
		tnextevent_ = eventinfo_->nextEventTime;
	}

	// go into the "default mode": continuousTimeMode
	lastStatus_ = enterContinuousTimeMode();

	return (fmippStatus) lastStatus_;
}

fmippTime FMUModelExchange::getTime() const
{
	return time_;
}

fmippStatus FMUModelExchange::setTime( fmippTime time )
{
	time_ = time;
	// NB: If instance_ != 0 then also fmu_ != 0.
	if ( 0 != instance_ ) return (fmippStatus) fmu_->functions->setTime( instance_, time_ );

	return fmippFatal;
}

void FMUModelExchange::rewindTime( fmippTime deltaRewindTime )
{
	time_ -= deltaRewindTime;
	fmu_->functions->setTime( instance_, time_ );
	/**
	 * \todo test. Maybe it is necessary to do evnthandling afterwards
	 *       \code{.cpp}
	 *             fmu_->enterEventMode();
	 *             fmu_->newDiscreteStates();
	 *             fmu_->enterContinuousTimeMode();
	 *       \endcode
	 */
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
	fmi2Boolean val2 = (fmi2Boolean) val;
	lastStatus_ = fmu_->functions->setBoolean( instance_, &valref, 1, &val2 );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue( fmippValueReference valref, const fmippString& val )
{
	fmi2String cString = val.c_str();
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
	fmi2Boolean val2 = (fmi2Boolean) *val;
	lastStatus_ = fmu_->functions->setBoolean(instance_, valref, ival, &val2 );
	// no need for backcasting since setter function is write-only
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::setValue(fmippValueReference* valref, const fmippString* val, fmippSize ival)
{
	fmi2String* cStrings = new fmi2String[ival];

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
		return (fmippStatus) lastStatus_;

	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
		return (fmippStatus) lastStatus_;
	}
}

fmippStatus FMUModelExchange::setValue( const fmippString& name, const fmippInteger& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setInteger( instance_, &it->second, 1, &val );
		return (fmippStatus) lastStatus_;
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
		return (fmippStatus) lastStatus_;
	}
}

fmippStatus FMUModelExchange::setValue( const fmippString& name, const fmippBoolean& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		fmi2Boolean val2 = (fmi2Boolean) val;
		lastStatus_ = fmu_->functions->setBoolean( instance_, &it->second, 1, &val2 );
		// no need for backcasting since setter function is write-only
		return (fmippStatus) lastStatus_;
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
		return (fmippStatus) lastStatus_;
	}
}

fmippStatus FMUModelExchange::setValue( const fmippString& name, const fmippString& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	const char* cString = val.c_str();

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setString( instance_, &it->second, 1, &cString );
		return (fmippStatus) lastStatus_;
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
		return (fmippStatus) lastStatus_;
	}
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
	fmi2Boolean val2 = (fmi2Boolean)val;
	lastStatus_ = fmu_->functions->getBoolean( instance_, &valref, 1, &val2 );
	val = (fmippBoolean) val2;
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
	fmi2Boolean val2 = (fmi2Boolean) *val;
	lastStatus_ = fmu_->functions->getBoolean( instance_, valref, ival, &val2 );
	*val = (fmippBoolean)val2;
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
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
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
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUModelExchange::getValue( const fmippString& name, fmippBoolean& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		fmi2Boolean val2 = (fmi2Boolean) val;
		lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, &val2 );
		val = (fmippBoolean) val2;
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
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
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}
	return (fmippStatus) lastStatus_;
}

fmippReal FMUModelExchange::getRealValue( const fmippString& name )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmi2Real val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getReal( instance_, &it->second, 1, val );
	} else {
		val[0] = numeric_limits<fmi2Real>::quiet_NaN();
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
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
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}

	return val[0];
}

fmippBoolean FMUModelExchange::getBooleanValue( const fmippString& name )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmi2Boolean val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, val );
	} else {
		val[0] = fmippFalse;
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}

	return (fmippBoolean) val[0];
}

fmippString FMUModelExchange::getStringValue( const fmippString& name )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmi2String val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getString( instance_, &it->second, 1, val );
	} else {
		val[0] = 0;
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}

	return fmippString( val[0] );
}

fmippStatus FMUModelExchange::getLastStatus() const
{
	if (!fmu_) {
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

vector<fmippValueReference> FMUModelExchange::getDerivativesRefs() const
{
	vector<fmippValueReference> derivatives_refs( derivatives_refs_, derivatives_refs_ + nStateVars_ );
	return derivatives_refs;
}

vector<fmippString> FMUModelExchange::getDerivativesNames() const
{
	vector<fmippString> derivatives_names;

	for ( unsigned int i = 0; i < nStateVars_; ++i ) {
		fmippValueReference der_ref = derivatives_refs_[i];

		for_each(
			varMap_.begin(), varMap_.end(),
			[ &der_ref, &derivatives_names ] ( const pair<fmippString, fmippValueReference> &p ) {
				if ( p.second == der_ref ) derivatives_names.push_back( p.first );
			}
		);
	}

	return derivatives_names;
}

fmippStatus FMUModelExchange::getJac( fmippReal* J ){
	fmippReal direction = 1.0;
	/*
	 * use the default behaviour defined in DynamicalSystem if getDirectionalDerivative is
	 * not supported by the FMU
	 *
	 * currently the default is a numerical 6th order approximation of the Jacobian.
	 */
	if ( !providesJacobian_ ){
		return DynamicalSystem::getJac( J );
	}

	// else use getDirectionalDerivative to read the Jacobian
	for ( unsigned int i = 0; i < nStateVars_; i++ ){
		// get the i-th column of the jacobian
		lastStatus_ = fmu_->functions->getDirectionalDerivative( instance_,
									 derivatives_refs_, nStateVars_,
									 &states_refs_[i], 1,
									 &direction, J );

		// stop calling the getDD function once it returns an exception
		if ( lastStatus_ != fmi2OK )
			break;

		// fill bigger indices of J in the next iteration of the for loop
		J += nStateVars_;
	}

#ifdef DYMOLA2015_WORKAROUND
	/*
	 * bugfix for FMUs exported from dymola.
	 *
	 * Switch the place of the inputs states_refs_ and derivatives_refs_. This bugfix is scripted in a
	 * way, so non-Dymola FMUs also recieve a correct jacobian.
	 */
	if ( lastStatus_ > fmi2OK )
		for ( unsigned int i = 0; i < nStateVars_; i++ ){
			lastStatus_ = fmu_->functions->getDirectionalDerivative( instance_,
									 &states_refs_[i], 1,
									 derivatives_refs_, nStateVars_,
									 &direction, J );
			if ( lastStatus_ != fmi2OK )
				break;
			J += nStateVars_;
		}
#endif
	return (fmippStatus) lastStatus_;
}

fmippValueReference FMUModelExchange::getValueRef( const fmippString& name ) const {
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find(name);

	if ( it != varMap_.end() ) {
		return it->second;
	} else {
		return fmippUndefinedValueReference;
	}
}

fmippStatus FMUModelExchange::getEventIndicators( fmippReal* eventsind )
{
	lastStatus_ = fmu_->functions->getEventIndicators(instance_, eventsind, nEventInds());
	return (fmippStatus) lastStatus_;
}

fmippTime FMUModelExchange::integrate( fmippTime tstop, unsigned int nsteps )
{
	assert( nsteps > 0 );
	fmippTime deltaT = ( tstop - getTime() ) / nsteps;
	return integrate( tstop, deltaT );
}

fmippTime FMUModelExchange::integrate( fmippTime tend, fmippTime deltaT )
{
	// if there are no continuous states, skip integration
	if ( nStateVars_ == 0 ){
		if ( stopBeforeEvent_ ){
			// in the case of stopBeforeEvent, completedIntegratorStep is called at the
			// beginning of the integration rather than the end
			// also event handling is done before the actual integration
			completedIntegratorStep();
			if ( upcomingEvent_ ){
				handleEvents();
				saveEventIndicators();
				upcomingEvent_ = fmippFalse;
			}
		}

		// determine whether a time event will happen in the time horizon we want to integrate
		timeEvent_ = eventinfo_->nextEventTimeDefined && eventinfo_->nextEventTime <= tend;
		if ( timeEvent_ )
			tend = eventinfo_->nextEventTime;
		setTime( tend );

		stateEvent_ = checkStateEvent();
		if ( !stopBeforeEvent_ ){
			completedIntegratorStep();
			if ( timeEvent_ || enterEventMode_ || stateEvent_ ){
				handleEvents();
				saveEventIndicators();
			}
		} else{
			// set a flag so the eventhandling will be done at the beginning of the next step
			if (  timeEvent_ || enterEventMode_ || stateEvent_ )
				upcomingEvent_ = fmippTrue;
		}
		return( tend );
	}

	// if we stopped because of an event, we have to trigger and handle
	// it before we start integrating again
	if ( stopBeforeEvent_ &&  upcomingEvent_ )
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
	stateEvent_ = eventInfo.stateEvent;

	/// \todo respond to terminateSimulation = true

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
			upcomingEvent_ = fmippTrue;
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
			upcomingEvent_ = fmippTrue;
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
	// make one step ftom t = time_ to t = tend_ with explicit euler
	for ( unsigned int i = 0; i < nStateVars_; i++ ){
		intStates_[i] += ( tend_ - time_ )*intDerivatives_[ i ];
	}
	// write the result of the eulerstep into the FMU
	setTime( tend_ );
	setContinuousStates( intStates_ );

	// handle events and set flags
	completedIntegratorStep();
	handleEvents();

	upcomingEvent_ = false;

	// update the event indicators ( this changes the behaviour of checkStateEvent() )
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

	intEventFlag_ |= (fmi2Boolean) stateEvent;
	eventFlag_    |= (fmi2Boolean) stateEvent;

	return stateEvent;
}

fmippBoolean FMUModelExchange::checkTimeEvent()
{
	if ( fmiTrue == eventinfo_->nextEventTimeDefined ) {
		tnextevent_ = eventinfo_->nextEventTime;
	} else {
		tnextevent_ = numeric_limits<fmippTime>::infinity();
	}

	return eventinfo_->nextEventTimeDefined;
}

fmippBoolean FMUModelExchange::checkStepEvent()
{
	completedIntegratorStep();
	return ( enterEventMode_ == fmi2True );
}

void FMUModelExchange::handleEvents()
{
	// change mode to eventmode: otherwise there will be an error when calling newDiscreteStates
	lastStatus_ = enterEventMode();

	// adapt the eventInfo so newDiscreteStates gets galled at least once
	eventinfo_->newDiscreteStatesNeeded = fmi2True;
	eventinfo_->terminateSimulation = fmi2False;

	// call newDiscreteStates several times if necessary
	for ( fmippSize i = 0;
	      eventinfo_->newDiscreteStatesNeeded &&
		      !eventinfo_->terminateSimulation &&
		      i < maxEventIterations_ ;
	      i++ )
		fmu_->functions->newDiscreteStates( instance_, eventinfo_ );

	/// \todo respond to eventInfo_->terminateSimulation = true

	// go back to the "default mode": continuousTimeMode
	lastStatus_ = enterContinuousTimeMode();
}

fmippStatus FMUModelExchange::completedIntegratorStep()
{
	fmi2Boolean noSetFMUStatePriorToCurrentPoint = fmi2False; // will setFmuState be called for a
	                                                          // t < currentTime ? The false flag
	                                                          // allows to clear buffers
	// Inform the model about an accepted step.
	lastStatus_ = fmu_->functions->completedIntegratorStep( instance_,
								noSetFMUStatePriorToCurrentPoint,
								&enterEventMode_,
								&terminateSimulation_ );
	return (fmippStatus) lastStatus_;
}

fmippBoolean FMUModelExchange::getIntEvent()
{
	return intEventFlag_;
}

fmippReal FMUModelExchange::getTimeEvent()
{
	return tnextevent_;
}

fmippBoolean FMUModelExchange::getEventFlag()
{
	return eventFlag_;
}

void FMUModelExchange::setEventFlag( fmippBoolean flag )
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

fmippSize FMUModelExchange::nStates() const
{
	return nStateVars_;
}

vector<fmippValueReference> FMUModelExchange::getStatesRefs() const
{
	vector<fmippValueReference> states_refs( states_refs_, states_refs_ + nStateVars_ );
	return states_refs;
}

vector<fmippString> FMUModelExchange::getStatesNames() const
{
	vector<fmippString> states_names;

	for ( unsigned int i = 0; i < nStateVars_; ++i ) {
		fmippValueReference state_ref = states_refs_[i];

		for_each(
			varMap_.begin(), varMap_.end(),
			[ &state_ref, &states_names ] ( const pair<fmippString, fmippValueReference> &p ) {
				if ( p.second == state_ref ) states_names.push_back( p.first );
			}
		);
	}

	return states_names;
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
	assert( ( lastStatus_ != fmi2OK ) || fmu_);
	if ( fmu_ ) {
		assert( fmu_->description != NULL );
		return fmu_->description;
	} else {
		return NULL;
	}
}

void FMUModelExchange::logger( fmi2Status status, const fmippString& category, const fmippString& msg ) const
{
	callbacks_.logger( callbacks_.componentEnvironment, instanceName_.c_str(),
		status, category.c_str(), msg.c_str() );
}

void FMUModelExchange::logger( fmi2Status status, const char* category, const char* msg ) const
{
	callbacks_.logger( callbacks_.componentEnvironment, instanceName_.c_str(),
		status, category, msg );
}

fmi2Status FMUModelExchange::enterContinuousTimeMode()
{
	return fmu_->functions->enterContinuousTimeMode( instance_ );
}

fmi2Status FMUModelExchange::enterEventMode()
{
	return fmu_->functions->enterEventMode( instance_ );
}

fmippStatus FMUModelExchange::setCallbacks( fmi2::fmi2CallbackLogger logger,
		fmi2::fmi2CallbackAllocateMemory allocateMemory,
		fmi2::fmi2CallbackFreeMemory freeMemory )
{
	if ( ( 0 == logger ) || ( 0 == allocateMemory ) || ( 0 == freeMemory ) ) {
		this->logger( fmi2Error, "ERROR", "callback function pointer(s) invalid" );
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
	logger( fmi2OK, "DEBUG", msg );
}

} // namespace fmi_2_0
