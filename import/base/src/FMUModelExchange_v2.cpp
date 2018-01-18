// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
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
FMUModelExchange::FMUModelExchange( const string& fmuDirUri,
		const string& modelIdentifier,
		const fmi2Boolean loggingOn,
		const bool stopBeforeEvent,
		const fmi2Time eventSearchPrecision,
		const IntegratorType type ) :
	FMUModelExchangeBase( loggingOn ),
	instance_( 0 ),
	nStateVars_( numeric_limits<size_t>::quiet_NaN() ),
	nEventInds_( numeric_limits<size_t>::quiet_NaN() ),
	nValueRefs_( numeric_limits<size_t>::quiet_NaN() ),
	derivatives_refs_( 0 ),
	states_refs_( 0 ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision ),
	intStates_( 0 ),
	intDerivatives_( 0 ),
	time_( numeric_limits<fmi2Time>::quiet_NaN() ),
	tnextevent_( numeric_limits<fmi2Time>::quiet_NaN() ),
	lastEventTime_( numeric_limits<fmi2Time>::quiet_NaN() ),
	eventinfo_( 0 ),
	eventsind_( 0 ),
	preeventsind_( 0 ),
	callEventUpdate_( fmi2False ),
	stateEvent_( fmi2False ),
	timeEvent_( fmi2False ),
	enterEventMode_( fmi2False ),
	terminateSimulation_( fmi2False ),
	upcomingEvent_( fmi2False ),
	raisedEvent_( fmi2False ),
	eventFlag_( fmi2False ),
	intEventFlag_( fmi2False ),
	lastStatus_( fmi2OK )
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
	} else if ( ( fmi_2_0_me != fmuType ) && ( fmi_2_0_me_and_cs != fmuType ) ) { // Wrong type of FMU.
		cerr << "wrong type of FMU" << endl;
		return;
	}

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
		integrator_->initialize();
		integrator_->setType( type );
	}
}


// Constructor. Requires the FMU to be already loaded (via the model manager).
FMUModelExchange::FMUModelExchange( const string& modelIdentifier,
		const bool loggingOn,
		const bool stopBeforeEvent,
		const fmi2Time eventSearchPrecision,
		const IntegratorType type ) :
	FMUModelExchangeBase( loggingOn ),
	instance_( 0 ),
	nStateVars_( numeric_limits<size_t>::quiet_NaN() ),
	nEventInds_( numeric_limits<size_t>::quiet_NaN() ),
	nValueRefs_( numeric_limits<size_t>::quiet_NaN() ),
	derivatives_refs_( 0 ),
	states_refs_( 0 ),
	stopBeforeEvent_( stopBeforeEvent ),
	eventSearchPrecision_( eventSearchPrecision ),
	intStates_( 0 ),
	intDerivatives_( 0 ),
	time_( numeric_limits<fmi2Time>::quiet_NaN() ),
	tnextevent_( numeric_limits<fmi2Time>::quiet_NaN() ),
	lastEventTime_( numeric_limits<fmi2Time>::quiet_NaN() ),
	eventinfo_( 0 ),
	eventsind_( 0 ),
	preeventsind_( 0 ),
	callEventUpdate_( fmi2False ),
	stateEvent_( fmi2False ),
	timeEvent_( fmi2False ),
	enterEventMode_( fmi2False ),
	terminateSimulation_( fmi2False ),
	upcomingEvent_( fmi2False ),
	raisedEvent_( fmi2False ),
	eventFlag_( fmi2False ),
	intEventFlag_( fmi2False ),
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
	derivatives_refs_( 0 ),
	states_refs_( 0 ),
	varMap_( fmu.varMap_ ),
	varTypeMap_( fmu.varTypeMap_ ),
	stopBeforeEvent_( fmu.stopBeforeEvent_ ),
	eventSearchPrecision_( fmu.eventSearchPrecision_ ),
	intStates_( 0 ),
	intDerivatives_( 0 ),
	time_( numeric_limits<fmi2Time>::quiet_NaN() ),
	tnextevent_( numeric_limits<fmi2Time>::quiet_NaN() ),
	lastEventTime_( numeric_limits<fmi2Time>::quiet_NaN() ),
	eventinfo_( 0 ),
	eventsind_( 0 ),
	preeventsind_( 0 ),
	callEventUpdate_( fmi2False ),
	stateEvent_( fmi2False ),
	timeEvent_( fmi2False ),
	enterEventMode_( fmi2False ),
	terminateSimulation_( fmi2False ),
	upcomingEvent_( fmi2False ),
	raisedEvent_( fmi2False ),
	eventFlag_( fmi2False ),
	intEventFlag_( fmi2False ),
	lastStatus_( fmi2OK )
{
	if ( 0 != fmu_ ){
		// allocate memory for the integrator
		integrator_->initialize();
		// create the stepper
		integrator_->setType( fmu.integrator_->getProperties().type );
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
	set<string> allVariableNames;
	pair< set<string>::iterator, bool > varNamesInsert;

	// List of all variable value references -> check if value references are unique.
	set<fmi2ValueReference> allVariableValRefs; 
	pair< set<fmi2ValueReference>::iterator, bool > varValRefsInsert;

	for ( ; itVar != itEnd; ++itVar )
	{
		const Properties& varAttributes = getAttributes( itVar );

		string varName = varAttributes.get<string>( "name" );
		fmi2ValueReference varValRef = varAttributes.get<fmi2ValueReference>( "valueReference" );

		varNamesInsert = allVariableNames.insert( varName );
		if ( false == varNamesInsert.second ) { // Check if variable name is unique.
			string message = string( "multiple definitions of variable name '" ) +
				varName + string( "' found" );
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
		fmi2Time startTime;
		fmi2Time stopTime;
		fmi2Real tolerance;
		fmi2Time stepSize; // \FIXME: currently unused
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
	derivatives_refs_ = new fmi2ValueReference[nStateVars_];
	states_refs_ = new fmi2ValueReference[nStateVars_];
	if ( nStateVars_> 0 )
		description->getStatesAndDerivativesReferences( states_refs_, derivatives_refs_ );
}


FMIVariableType FMUModelExchange::getType( const string& variableName ) const
{
	map<string,FMIVariableType>::const_iterator it = varTypeMap_.find( variableName );

	if ( it == varTypeMap_.end() ) {
		string ret = variableName + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		return fmiTypeUnknown;
	}

	return it->second;
}


fmiStatus FMUModelExchange::instantiate( const string& instanceName )
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
		return (fmiStatus) lastStatus_;
	}

	time_ = 0.;
	tnextevent_ = numeric_limits<fmi2Time>::infinity();

	// Memory allocation.
	if ( nEventInds_ > 0 ) {
		eventsind_ = new fmi2Real[nEventInds_];
		preeventsind_ = new fmi2Real[nEventInds_];
	}

	if ( nStateVars_ > 0 ) {
		intStates_ = new fmi2Real[nStateVars_];
		intDerivatives_ = new fmi2Real[nStateVars_];
	}

	for ( size_t i = 0; i < nEventInds_; ++i ) {
		eventsind_[i] = 0;
		preeventsind_[i] = 0;
	}

	eventinfo_ = new fmi2EventInfo;

	const string& guid = fmu_->description->getGUID();

	fmi2Boolean visible = fmi2False; // visible = false means that the FMU is executed in batch mode

	// call instantiate
	instance_ = fmu_->functions->instantiate( instanceName_.c_str(), fmi2ModelExchange,
		guid.c_str(), fmu_->fmuResourceLocation.c_str(), &callbacks_, visible, loggingOn_ );

	// check whether instantiate returned a non trivial object
	if ( 0 == instance_ ){
		lastStatus_ = fmi2Error;
		return (fmiStatus) lastStatus_;
	}

	/// \FIXME retrieve options for debug logging as defined in fmiModelDescription.LogCategories
	size_t nCategories = 0;
	char** categories = NULL;

	lastStatus_ = fmu_->functions->setDebugLogging( instance_,
							loggingOn_,
							nCategories,
							categories );

	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::initialize( bool toleranceDefined, double tolerance )
{
	// NB: If instance_ != 0 then also fmu_ != 0.
	if ( 0 == instance_ ) {
		lastStatus_ = fmi2Error;
		return (fmiStatus) lastStatus_;
	}

	fmi2Boolean stopTimeDefined = fmi2False;
	fmi2Time stopTime = 1;

	/* In case the tolerance is not defined, use the default experiment for setupExperiment if available. Open questions:
	 *
	 *   * What happens if the default stop time is available but we want to integrate past that?
	 *   * What if the tolerance is later changed?
	 *
	 */
	if ( fmu_->description->hasDefaultExperiment() ){
		Integrator::Properties properties = integrator_->getProperties();
		fmi2Time startTime;
		fmi2Time defaultStopTime;
		fmi2Real defaultTolerance;
		fmi2Time stepSize;           // \FIXME: currently unused
		fmu_->description->getDefaultExperiment( startTime, defaultStopTime, defaultTolerance,
							 stepSize );
		if ( defaultTolerance == defaultTolerance ){ // Check that value is not NaN.
			// Redefine in case the tolerance has not been defined by the user.
			if ( false == toleranceDefined ) {
				toleranceDefined = true;
				tolerance = static_cast<double>( defaultTolerance );
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

	// go into the "default mode": continuousTimeMode
	enterContinuousTimeMode();

	return (fmiStatus) lastStatus_;
}


fmiTime FMUModelExchange::getTime() const
{
	return time_;
}


void FMUModelExchange::setTime( fmiTime time )
{
	time_ = time;
	// NB: If instance_ != 0 then also fmu_ != 0.
	if ( 0 != instance_ ) fmu_->functions->setTime( instance_, time_ );
}


void FMUModelExchange::rewindTime( fmiTime deltaRewindTime )
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


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, fmiReal& val )
{
	lastStatus_ = fmu_->functions->setReal( instance_, &valref, 1, &val );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, fmiInteger& val )
{
	lastStatus_ = fmu_->functions->setInteger( instance_, &valref, 1, &val );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, fmiBoolean& val )
{
	fmi2Boolean val2 = (fmi2Boolean) val;
	lastStatus_ = fmu_->functions->setBoolean( instance_, &valref, 1, &val2 );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::setValue( fmiValueReference valref, string& val )
{
	const char* cString = val.c_str();
	lastStatus_ = fmu_->functions->setString( instance_, &valref, 1, &cString );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, fmiReal* val, size_t ival)
{
	lastStatus_ = fmu_->functions->setReal(instance_, valref, ival, val);
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, fmiInteger* val, size_t ival)
{
	lastStatus_ = fmu_->functions->setInteger(instance_, valref, ival, val);
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, fmiBoolean* val, size_t ival)
{
	fmi2Boolean val2 = (fmi2Boolean) *val;
	lastStatus_ = fmu_->functions->setBoolean(instance_, valref, ival, &val2 );
	// no need for backcasting since setter function is write-only
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::setValue(fmiValueReference* valref, string* val, size_t ival)
{
	const char** cStrings = new const char*[ival];

	for ( size_t i = 0; i < ival; i++ ) {
		cStrings[i] = val[i].c_str();
	}
	lastStatus_ = fmu_->functions->setString(instance_, valref, ival, cStrings);
	delete [] cStrings;
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::setValue( const string& name, fmiReal val )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setReal( instance_, &it->second, 1, &val );
		return (fmiStatus) lastStatus_;

	} else {
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
		return (fmiStatus) lastStatus_;
	}
}


fmiStatus FMUModelExchange::setValue( const string& name, fmiInteger val )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setInteger( instance_, &it->second, 1, &val );
		return (fmiStatus) lastStatus_;
	} else {
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
		return (fmiStatus) lastStatus_;
	}
}


fmiStatus FMUModelExchange::setValue( const string& name, fmiBoolean val )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		fmi2Boolean val2 = (fmi2Boolean) val;
		lastStatus_ = fmu_->functions->setBoolean( instance_, &it->second, 1, &val2 );
		// no need for backcasting since setter function is write-only
		return (fmiStatus) lastStatus_;
	} else {
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
		return (fmiStatus) lastStatus_;
	}
}


fmiStatus FMUModelExchange::setValue( const string& name, string val )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );
	const char* cString = val.c_str();

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setString( instance_, &it->second, 1, &cString );
		return (fmiStatus) lastStatus_;
	} else {
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
		return (fmiStatus) lastStatus_;
	}
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, fmiReal& val )
{
	lastStatus_ = fmu_->functions->getReal( instance_,
						(fmi2ValueReference*) &valref,
						1,
						(fmi2Real*) &val );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, fmiInteger& val )
{
	lastStatus_ = fmu_->functions->getInteger( instance_,
						   &valref,
						   1,
						   &val );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, fmiBoolean& val )
{
	fmi2Boolean val2 = (fmi2Boolean)val;
	lastStatus_ = fmu_->functions->getBoolean( instance_,
						   (fmi2ValueReference*) &valref,
						   1,
						   &val2 );
	val = (fmiBoolean) val2;
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( fmiValueReference valref, string& val )
{
	const char* cString;
	lastStatus_ = fmu_->functions->getString( instance_, &valref, 1, &cString );
	val = string( cString );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, fmiReal* val, size_t ival )
{
	lastStatus_ = fmu_->functions->getReal( instance_, valref, ival, val );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, fmiInteger* val, size_t ival )
{
	lastStatus_ = fmu_->functions->getInteger( instance_, valref, ival, val );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( fmiValueReference* valref, fmiBoolean* val, size_t ival )
{
	fmi2Boolean val2 = (fmi2Boolean) *val;
	lastStatus_ = fmu_->functions->getBoolean( instance_, valref, ival, &val2 );
	*val = (fmiBoolean)val2;
	return (fmiStatus) lastStatus_;
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

	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( const string& name, fmiReal& val )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getReal( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( const string& name, fmiInteger& val )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getInteger( instance_, &it->second, 1, &val );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( const string& name, fmiBoolean& val )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		fmi2Boolean val2 = (fmi2Boolean) val;
		lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, &val2 );
		val = (fmiBoolean) val2;
	} else {
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getValue( const string& name, string& val )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );
	const char* cString;

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getString( instance_, &it->second, 1, &cString );
		val = string( cString );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}
	return (fmiStatus) lastStatus_;
}


fmiReal FMUModelExchange::getRealValue( const string& name )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );
	fmi2Real val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getReal( instance_, &it->second, 1, val );
	} else {
		val[0] = numeric_limits<fmi2Real>::quiet_NaN();
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}

	return val[0];
}


fmiInteger FMUModelExchange::getIntegerValue( const string& name )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );
	fmi2Integer val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getInteger( instance_, &it->second, 1, val );
	} else {
		val[0] = numeric_limits<fmi2Integer>::quiet_NaN();
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}

	return val[0];
}


fmiBoolean FMUModelExchange::getBooleanValue( const string& name )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );
	fmi2Boolean val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, val );
	} else {
		val[0] = fmi2False;
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}

	return (fmiBoolean) val[0];
}


fmiString FMUModelExchange::getStringValue( const string& name )
{
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find( name );
	fmi2String val[1];

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getString( instance_, &it->second, 1, val );
	} else {
		val[0] = 0;
		string ret = name + string( " does not exist" );
		logger( fmi2Discard, "WARNING", ret );
		lastStatus_ = fmi2Discard;
	}

	return val[0];
}


fmiStatus FMUModelExchange::getLastStatus() const
{
	if (!fmu_) {
		return fmiFatal;
	} else {
		return (fmiStatus) lastStatus_;
	}
}


fmiStatus FMUModelExchange::getContinuousStates( fmiReal* val )
{
	lastStatus_ = fmu_->functions->getContinuousStates( instance_,
							    (fmi2Real*) val,
							    nStateVars_ );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::setContinuousStates( const fmiReal* val )
{
	lastStatus_ = fmu_->functions->setContinuousStates( instance_,
							    (fmiReal*) val,
							    nStateVars_ );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getDerivatives( fmiReal* val )
{
	lastStatus_ = fmu_->functions->getDerivatives( instance_, val, nStateVars_ );
	return (fmiStatus) lastStatus_;
}


fmiStatus FMUModelExchange::getJac( fmiReal* J ){
	fmi2Real direction = 1.0;
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
	return (fmiStatus) lastStatus_;
}


fmiValueReference FMUModelExchange::getValueRef( const string& name ) const {
	map<string,fmi2ValueReference>::const_iterator it = varMap_.find(name);

	if ( it != varMap_.end() ) {
		return it->second;
	} else {
		return fmi2UndefinedValueReference;
	}
}


fmiStatus FMUModelExchange::getEventIndicators( fmiReal* eventsind )
{
	lastStatus_ = fmu_->functions->getEventIndicators(instance_, eventsind, nEventInds());
	return (fmiStatus) lastStatus_;
}


fmiTime FMUModelExchange::integrate( fmiTime tstop, unsigned int nsteps )
{
	assert( nsteps > 0 );
	fmi2Time deltaT = ( tstop - getTime() ) / nsteps;
	return integrate( tstop, deltaT );
}

fmiTime FMUModelExchange::integrate( fmiTime tend, fmiTime deltaT )
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
				upcomingEvent_ = fmi2False;
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
				upcomingEvent_ = fmi2True;
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
			upcomingEvent_ = fmi2True;
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
			upcomingEvent_ = fmi2True;
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
	raisedEvent_ = fmi2True;
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
	if ( fmiTrue == eventinfo_->nextEventTimeDefined ) {
		tnextevent_ = eventinfo_->nextEventTime;
	} else {
		tnextevent_ = numeric_limits<fmiTime>::infinity();
	}

	return eventinfo_->nextEventTimeDefined;
}


bool FMUModelExchange::checkStepEvent()
{
	completedIntegratorStep();
	return ( enterEventMode_ == fmi2True );
}


void FMUModelExchange::handleEvents()
{
	// change mode to eventmode: otherwise there will be an error when calling newDiscreteStates
	fmu_->functions->enterEventMode( instance_ );

	// adapt the eventInfo so newDiscreteStates gets galled at least once
	eventinfo_->newDiscreteStatesNeeded = fmi2True;
	eventinfo_->terminateSimulation = fmi2False;

	// call newDiscreteStates several times if necessary
	for ( size_t i = 0;
	      eventinfo_->newDiscreteStatesNeeded &&
		      !eventinfo_->terminateSimulation &&
		      i < maxEventIterations_ ;
	      i++ )
		fmu_->functions->newDiscreteStates( instance_, eventinfo_ );

	/// \todo respond to eventInfo_->terminateSimulation = true

	// go back to the "default mode": continuousTimeMode
	fmu_->functions->enterContinuousTimeMode( instance_ );
}


fmiStatus FMUModelExchange::completedIntegratorStep()
{
	fmi2Boolean noSetFMUStatePriorToCurrentPoint = fmi2False; // will setFmuState be called for a
	                                                          // t < currentTime ? The false flag
	                                                          // allows to clear buffers
	// Inform the model about an accepted step.
	lastStatus_ = fmu_->functions->completedIntegratorStep( instance_,
								noSetFMUStatePriorToCurrentPoint,
								&enterEventMode_,
								&terminateSimulation_ );
	return (fmiStatus) lastStatus_;
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

void FMUModelExchange::logger( fmi2Status status, const string& category, const string& msg ) const
{
	callbacks_.logger( callbacks_.componentEnvironment, instanceName_.c_str(),
		status, category.c_str(), msg.c_str() );
}


void FMUModelExchange::logger( fmi2Status status, const char* category, const char* msg ) const
{
	callbacks_.logger( callbacks_.componentEnvironment, instanceName_.c_str(),
		status, category, msg );
}


void FMUModelExchange::enterContinuousTimeMode()
{
	fmu_->functions->enterContinuousTimeMode( instance_ );
}


fmiStatus FMUModelExchange::setCallbacks( fmi2::fmi2CallbackLogger logger,
		fmi2::fmi2CallbackAllocateMemory allocateMemory,
		fmi2::fmi2CallbackFreeMemory freeMemory )
{
	if ( ( 0 == logger ) || ( 0 == allocateMemory ) || ( 0 == freeMemory ) ) {
		this->logger( fmi2Error, "ERROR", "callback function pointer(s) invalid" );
		return fmiError;
	}

	callbacks_.logger = logger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;

	return fmiOK;
}


void
FMUModelExchange::sendDebugMessage( const string& msg ) const
{
	logger( fmi2OK, "DEBUG", msg );
}

} // namespace fmi_2_0
