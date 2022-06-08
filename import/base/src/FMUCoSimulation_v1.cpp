// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------
/**
 * \file FMUCoSimulation_v1.cpp
 */
#include <assert.h>
#include <set>
#include <sstream>
#include <iostream>
#include <cmath>
#include <limits>

#include "common/FMIPPConfig.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/fmi_v1.0/fmi_cs.h"

#include "import/base/include/FMUCoSimulation_v1.h"
#include "import/base/include/CallbackFunctions.h"
#include "import/base/include/ModelDescription.h"
#include "import/base/include/ModelManager.h"

using namespace std;

namespace fmi_1_0 {

/// The following template function should not be defined here (the include file
/// would be more appropriate for instance). But putting it there, together with
/// the necessary include statements may cause trouble when using SWIG with MinGW.
template<typename Type>
Type FMUCoSimulation::getCoSimToolCapabilities( const fmippString& attributeName ) const
{
	using namespace ModelDescriptionUtilities;
	Type val;
	
	const ModelDescription* description = getModelDescription();
	
	if ( true == description->hasImplementation() )
	{
		const ModelDescription::Properties& implementation = description->getImplementation();

		if ( true == hasChild( implementation, "CoSimulation_Tool.Capabilities" ) ) {

			const Properties& coSimToolCapabilities = getChildAttributes( implementation, "CoSimulation_Tool.Capabilities" );
			if ( true == hasChild( coSimToolCapabilities, attributeName ) )
			{
				val = coSimToolCapabilities.get<Type>( attributeName );
			} else {
				fmippString err = fmippString( "XML attribute not found in model description: " ) + attributeName;
				throw runtime_error( err );
			}

		} else if ( true == hasChild( implementation, "CoSimulation_StandAlone.Capabilities" ) ) {

			const Properties& coSimToolCapabilities = getChildAttributes( implementation, "CoSimulation_StandAlone.Capabilities" );
			if ( true == hasChild( coSimToolCapabilities, attributeName ) )
			{
				val = coSimToolCapabilities.get<Type>( attributeName );
			} else {
				fmippString err = fmippString( "XML attribute not found in model description: " ) + attributeName;
				throw runtime_error( err );
			}

		} else {

			fmippString err( "XML node not found in model description: 'CoSimulation_Tool.Capabilities' or 'CoSimulation_StandAlone.Capabilities'" );
			throw runtime_error( err );
		}
	} else {
		fmippString err( "XML node not found in model description: Implementation" );
		throw runtime_error( err );
	}
	
	return val;
}

// Constructor. Loads the FMU via the model manager (if needed).
FMUCoSimulation::FMUCoSimulation( const fmippString& fmuDirUri,
	const fmippString& modelIdentifier,
	const fmippBoolean loggingOn,
	const fmippTime timeDiffResolution ) :
		FMUCoSimulationBase( loggingOn ),
		instance_( NULL ),
		time_( numeric_limits<fmippTime>::quiet_NaN() ),
		timeDiffResolution_( timeDiffResolution ),
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
	} else if ( fmi_1_0_cs != loadedFMUType ) { // Wrong type of FMU.
		cerr << "wrong type of FMU (expected FMI CS v1)" << endl;
		lastStatus_ = fmiFatal;
		return;
	}
	
	// Retrieve bare FMU from model manager.
	fmu_ = manager.getSlave( loadedModelIdentifier );

	// Set default callback functions.
	using namespace callback;
	callbacks_.logger = loggingOn ? verboseLogger : succinctLogger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;
	callbacks_.stepFinished = stepFinished;

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
	} else {
		lastStatus_ = fmiFatal;
	}
}

// Constructor. Requires the FMU to be already loaded (via the model manager).
FMUCoSimulation::FMUCoSimulation( const fmippString& modelIdentifier,
	const fmippBoolean loggingOn,
	const fmippTime timeDiffResolution ) :
		FMUCoSimulationBase( loggingOn ),
		instance_( NULL ),
		time_( numeric_limits<fmippTime>::quiet_NaN() ),
		timeDiffResolution_( timeDiffResolution ),
		lastStatus_( fmiOK )
{
	// Get the model manager.
	ModelManager& manager = ModelManager::getModelManager();
	
	// Retrieve bare FMU from model manager.
	fmu_ = manager.getSlave( modelIdentifier );

	// Set default callback functions.
	using namespace callback;
	callbacks_.logger = loggingOn ? verboseLogger : succinctLogger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;
	callbacks_.stepFinished = stepFinished;

	if ( 0 != fmu_ ) {
		readModelDescription();
	} else {
		lastStatus_ = fmiFatal;
	}
}

FMUCoSimulation::FMUCoSimulation( const FMUCoSimulation& fmu ) :
		FMUCoSimulationBase( fmu.loggingOn_ ),
		instance_( NULL ),
		fmu_( fmu.fmu_ ),
		callbacks_( fmu.callbacks_ ),
		varMap_( fmu.varMap_ ),
		varTypeMap_( fmu.varTypeMap_ ),
		time_( numeric_limits<fmippReal>::quiet_NaN() ),
		timeDiffResolution_( fmu.timeDiffResolution_ ),
		lastStatus_( fmiOK )
{}

FMUCoSimulation::~FMUCoSimulation()
{
	if ( instance_ ) {
		fmu_->functions->terminateSlave( instance_ );
		fmu_->functions->freeSlaveInstance( instance_ );
	}
}

void
FMUCoSimulation::terminate()
{
	if ( instance_ ) {
		fmu_->functions->terminateSlave( instance_ );
		fmu_->functions->freeSlaveInstance( instance_ );
	}
}

void FMUCoSimulation::readModelDescription()
{
	using namespace ModelDescriptionUtilities;

	typedef ModelDescription::Properties Properties;
	const ModelDescription* description = fmu_->description;

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
		} else if ( hasChild( itVar, "fmippBooleanean" ) ) {
			varTypeMap_.insert( make_pair( varName, fmippTypeBoolean ) );
		} else if ( hasChild( itVar, "String" ) ) {
			varTypeMap_.insert( make_pair( varName, fmippTypeString ) );
		} else {
			varTypeMap_.insert( make_pair( varName, fmippTypeUnknown ) );
		}
	}
	//nValueRefs_ = varMap_.size();
}
fmippStatus FMUCoSimulation::instantiate( const fmippString& instanceName,
	const fmippTime timeout,
	const fmippBoolean visible,
	const fmippBoolean interactive )
{
	instanceName_ = instanceName;
	if ( fmu_ == 0 ) { 
		lastStatus_ = fmiError;
		return (fmippStatus) lastStatus_;
	}

	time_ = 0.;

	const fmippString& guid = fmu_->description->getGUID();
	const fmippString& type = fmu_->description->getMIMEType();

	instance_ = fmu_->functions->instantiateSlave( instanceName_.c_str(), guid.c_str(),
		fmu_->fmuLocation.c_str(), type.c_str(), timeout, visible, interactive, callbacks_, loggingOn_ );

	if ( 0 == instance_ ) {
		lastStatus_ = fmiError;
		return (fmippStatus) lastStatus_;
	}

	lastStatus_ = fmu_->functions->setDebugLogging( instance_, loggingOn_ );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::initialize( const fmippTime tStart,
	const fmippBoolean stopTimeDefined, const fmippTime tStop )
{
	if ( 0 == instance_ ) {
		lastStatus_ = fmiError;
		return (fmippStatus) lastStatus_;
	}
	time_ = tStart;
	
	lastStatus_ = fmu_->functions->initializeSlave( instance_, tStart, stopTimeDefined, tStop );
	return (fmippStatus) lastStatus_;
}

fmippTime FMUCoSimulation::getTime() const
{
	return time_;
}

fmippStatus FMUCoSimulation::setValue( fmippValueReference valref, const fmippReal& val )
{
	lastStatus_ = fmu_->functions->setReal( instance_, &valref, 1, &val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setValue( fmippValueReference valref, const fmippInteger& val )
{
	lastStatus_ = fmu_->functions->setInteger( instance_, &valref, 1, &val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setValue( fmippValueReference valref, const fmippBoolean& val )
{
	fmiBoolean val2 = (fmiBoolean) val;
	lastStatus_ = fmu_->functions->setBoolean( instance_, &valref, 1, &val2 );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setValue( fmippValueReference valref, const fmippString& val )
{
	fmiString cString = val.c_str();
	lastStatus_ = fmu_->functions->setString( instance_, &valref, 1, &cString );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setValue(fmippValueReference* valref, const fmippReal* val, fmippSize ival)
{
	lastStatus_ = fmu_->functions->setReal(instance_, valref, ival, val);
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setValue(fmippValueReference* valref, const fmippInteger* val, fmippSize ival)
{
	lastStatus_ = fmu_->functions->setInteger(instance_, valref, ival, val);
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setValue(fmippValueReference* valref, const fmippBoolean* val, fmippSize ival)
{
	fmiBoolean val2 = (fmiBoolean) *val;
	lastStatus_ = fmu_->functions->setBoolean(instance_, valref, ival, &val2);
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setValue(fmippValueReference* valref, const fmippString* val, fmippSize ival)
{
	fmiString* cStrings = new fmiString[ival];
	for ( fmippSize i = 0; i < ival; i++ ) {
		cStrings[i] = val[i].c_str();
	}
	lastStatus_ = fmu_->functions->setString(instance_, valref, ival, cStrings);
	delete [] cStrings;
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setValue( const fmippString& name, const fmippReal& val )
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

fmippStatus FMUCoSimulation::setValue( const fmippString& name, const fmippInteger& val )
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

fmippStatus FMUCoSimulation::setValue( const fmippString& name, const fmippBoolean& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		fmiBoolean val2 = (fmiBoolean) val;
		lastStatus_ = fmu_->functions->setBoolean( instance_, &it->second, 1, &val2 );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setValue( const fmippString& name, const fmippString& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmiString cString = val.c_str();
	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->setString( instance_, &it->second, 1, &cString );
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( fmippValueReference valref, fmippReal& val )
{
	lastStatus_ = fmu_->functions->getReal( instance_, &valref, 1, &val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( fmippValueReference valref, fmippInteger& val )
{
	lastStatus_ = fmu_->functions->getInteger( instance_, &valref, 1, &val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( fmippValueReference valref, fmippBoolean& val )
{
	fmiBoolean val2;
	lastStatus_ = fmu_->functions->getBoolean( instance_, &valref, 1, &val2 );
	val = (fmippBoolean) val2;
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( fmippValueReference valref, fmippString& val )
{
	fmiString cString;
	lastStatus_ = fmu_->functions->getString( instance_, &valref, 1, &cString );
	val = fmippString( cString );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( fmippValueReference* valref, fmippReal* val, fmippSize ival )
{
	lastStatus_ = fmu_->functions->getReal( instance_, valref, ival, val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( fmippValueReference* valref, fmippInteger* val, fmippSize ival )
{
	lastStatus_ = fmu_->functions->getInteger( instance_, valref, ival, val );
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( fmippValueReference* valref, fmippBoolean* val, fmippSize ival )
{
	fmiBoolean* val2 = new fmiBoolean[ival];
	for ( fmippSize i = 0; i < ival; ++i ) {
		val2[i] = (fmiBoolean) val[i];
	}
	lastStatus_ = fmu_->functions->getBoolean( instance_, valref, ival, val2 );
	delete val2;
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( fmippValueReference* valref, fmippString* val, fmippSize ival )
{
	fmiString* cStrings = 0;
	
	lastStatus_ = fmu_->functions->getString( instance_, valref, ival, cStrings );
	if ( 0 != cStrings ) {
		for ( fmippSize i = 0; i < ival; ++i ) {
			val[i] = fmippString( cStrings[i] );
		}
	}
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( const fmippString& name, fmippReal& val )
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

fmippStatus FMUCoSimulation::getValue( const fmippString& name, fmippInteger& val )
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

fmippStatus FMUCoSimulation::getValue( const fmippString& name, fmippBoolean& val )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	if ( it != varMap_.end() ) {
		fmiBoolean val2;
		lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, &val2 );
		val = (fmippBoolean) val2;
	} else {
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::getValue( const fmippString& name, fmippString& val )
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

fmippReal FMUCoSimulation::getRealValue( const fmippString& name )
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

fmippInteger FMUCoSimulation::getIntegerValue( const fmippString& name )
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

fmippBoolean FMUCoSimulation::getBooleanValue( const fmippString& name )
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find( name );
	fmiBoolean val;
	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getBoolean( instance_, &it->second, 1, &val );
	} else {
		val = fmiFalse;
		fmippString ret = name + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		lastStatus_ = fmiDiscard;
	}
	return (fmippBoolean) val;
}

fmippString FMUCoSimulation::getStringValue( const fmippString& name )
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

fmippStatus FMUCoSimulation::getLastStatus() const
{
	return (fmippStatus) lastStatus_;
}

fmippValueReference FMUCoSimulation::getValueRef( const fmippString& name ) const
{
	map<fmippString,fmippValueReference>::const_iterator it = varMap_.find(name);
	if ( it != varMap_.end() ) {
		return it->second;
	} else {
		return fmippUndefinedValueReference;
	}
}

fmippStatus FMUCoSimulation::doStep( fmippTime currentCommunicationPoint,
	fmippTime communicationStepSize, fmippBoolean newStep )
{
	if ( abs( time_ - currentCommunicationPoint ) > timeDiffResolution_ )
	{
		fmippString ret( "requested current communication point does not match FMU-internal time" );
		logger( fmiError, "ABORT", ret );
		return fmippError;
	}

	lastStatus_ = fmu_->functions->doStep( instance_, time_, communicationStepSize, 
		( ( newStep == fmippTrue ) ? fmiTrue : fmiFalse ) );

	if ( fmiOK == lastStatus_ ) time_ += communicationStepSize;
	return (fmippStatus) lastStatus_;
}

fmippStatus FMUCoSimulation::setCallbacks( cs::fmiCallbackLogger logger,
	cs::fmiCallbackAllocateMemory allocateMemory,
	cs::fmiCallbackFreeMemory freeMemory,
	cs::fmiStepFinished stepFinished )
{
	
	if ( ( 0 == logger ) || ( 0 == allocateMemory ) || ( 0 == freeMemory ) ) {
		this->logger( fmiError, "ERROR", "callback function pointer(s) invalid" );
		return fmippError;
	}

	callbacks_.logger = logger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;
	callbacks_.stepFinished = stepFinished;
	return fmippOK;
}

void FMUCoSimulation::logger( fmiStatus status, const fmippString& category, const fmippString& msg ) const
{
	callbacks_.logger( instance_, instanceName_.c_str(), status, category.c_str(), msg.c_str() );
}

void FMUCoSimulation::logger( fmiStatus status, const fmippChar* category, const fmippChar* msg ) const
{
	callbacks_.logger( instance_, instanceName_.c_str(), status, category, msg );
}

fmippSize FMUCoSimulation::nStates() const
{
	return 0;
}

fmippSize FMUCoSimulation::nEventInds() const
{
	return 0;
}

fmippSize FMUCoSimulation::nValueRefs() const
{
	return varMap_.size();
}

const ModelDescription* FMUCoSimulation::getModelDescription() const
{
	assert( ( lastStatus_ != fmiOK  ) || fmu_);
	if ( fmu_ ) {
		assert( fmu_->description != NULL );
		return fmu_->description;
	} else {
		return NULL;
	}
}

FMIPPVariableType FMUCoSimulation::getType( const fmippString& variableName ) const
{
	map<fmippString,FMIPPVariableType>::const_iterator it = varTypeMap_.find( variableName );
	if ( it == varTypeMap_.end() ) {
		fmippString ret = variableName + fmippString( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return fmippTypeUnknown;
	}
	return it->second;
}

fmippBoolean
FMUCoSimulation::canHandleVariableCommunicationStepSize() const
{
	return getCoSimToolCapabilities<fmippBoolean>( "canHandleVariableCommunicationStepSize" );
}

fmippBoolean
FMUCoSimulation::canHandleEvents() const
{
	return getCoSimToolCapabilities<fmippBoolean>( "canHandleEvents" );
}

fmippBoolean
FMUCoSimulation::canRejectSteps() const
{
	return getCoSimToolCapabilities<fmippBoolean>( "canRejectSteps" );
}

fmippBoolean
FMUCoSimulation::canInterpolateInputs() const
{
	return getCoSimToolCapabilities<fmippBoolean>( "canInterpolateInputs" );
}

fmippSize
FMUCoSimulation::maxOutputDerivativeOrder() const
{
	return getCoSimToolCapabilities<fmippSize>( "maxOutputDerivativeOrder" );
}

fmippBoolean
FMUCoSimulation::canRunAsynchronuously() const
{
	return getCoSimToolCapabilities<fmippBoolean>( "canRunAsynchronuously" );
}

fmippBoolean
FMUCoSimulation::canSignalEvents() const
{
	return getCoSimToolCapabilities<fmippBoolean>( "canSignalEvents" );
}

fmippBoolean 
FMUCoSimulation::canBeInstantiatedOnlyOncePerProcess() const
{
	return getCoSimToolCapabilities<fmippBoolean>( "canBeInstantiatedOnlyOncePerProcess" );
}

fmippBoolean
FMUCoSimulation::canNotUseMemoryManagementFunctions() const
{
	return getCoSimToolCapabilities<fmippBoolean>( "canNotUseMemoryManagementFunctions" );
}

void
FMUCoSimulation::sendDebugMessage( const fmippString& msg ) const
{
	logger( fmiOK, "DEBUG", msg );
}

} // namespace fmi_1_0
