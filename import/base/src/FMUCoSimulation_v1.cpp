// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
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
Type FMUCoSimulation::getCoSimToolCapabilities( const string& attributeName ) const
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
				val = coSimToolCapabilities.get<bool>( attributeName );
			} else {
				string err = string( "XML attribute not found in model description: " ) + attributeName;
				throw runtime_error( err );
			}
		} else if ( true == hasChild( implementation, "CoSimulation_StandAlone.Capabilities" ) ) {

			const Properties& coSimToolCapabilities = getChildAttributes( implementation, "CoSimulation_StandAlone.Capabilities" );
			if ( true == hasChild( coSimToolCapabilities, attributeName ) )
			{
				val = coSimToolCapabilities.get<bool>( attributeName );
			} else {
				string err = string( "XML attribute not found in model description: " ) + attributeName;
				throw runtime_error( err );
			}

		} else {
			string err( "XML node not found in model description: 'CoSimulation_Tool.Capabilities' or 'CoSimulation_StandAlone.Capabilities'" );
			throw runtime_error( err );
		}
	} else {
		string err( "XML node not found in model description: Implementation" );
		throw runtime_error( err );
	}
	
	return val;
}


// Constructor. Loads the FMU via the model manager (if needed).
FMUCoSimulation::FMUCoSimulation( const string& fmuDirUri,
		const string& modelIdentifier,
		const fmiBoolean loggingOn,
		const fmiReal timeDiffResolution ) :
	FMUCoSimulationBase( loggingOn ),
	instance_( NULL ),
	time_( numeric_limits<fmiReal>::quiet_NaN() ),
	timeDiffResolution_( timeDiffResolution ),
	lastStatus_( fmiOK )
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

	} else if ( fmi_1_0_cs != fmuType ) { // Wrong type of FMU.
		cerr << "wrong type of FMU" << endl;
		return;
	}

	// Retrieve bare FMU from model manager.
	fmu_ = manager.getSlave( modelIdentifier );

	// Set default callback functions.
	using namespace callback;
	callbacks_.logger = loggingOn ? verboseLogger : succinctLogger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;
	callbacks_.stepFinished = stepFinished;

	if ( 0 != fmu_ ) readModelDescription();
}


// Constructor. Requires the FMU to be already loaded (via the model manager).
FMUCoSimulation::FMUCoSimulation( const string& modelIdentifier,
		const fmiBoolean loggingOn,
		const fmiReal timeDiffResolution ) :
	FMUCoSimulationBase( loggingOn ),
	instance_( NULL ),
	time_( numeric_limits<fmiReal>::quiet_NaN() ),
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

	if ( 0 != fmu_ ) readModelDescription();
}


FMUCoSimulation::FMUCoSimulation( const FMUCoSimulation& fmu ) :
	FMUCoSimulationBase( fmu.loggingOn_ ),
	instance_( NULL ),
	fmu_( fmu.fmu_ ),
	callbacks_( fmu.callbacks_ ),
	varMap_( fmu.varMap_ ),
	varTypeMap_( fmu.varTypeMap_ ),
	time_( numeric_limits<fmiReal>::quiet_NaN() ),
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


void FMUCoSimulation::readModelDescription() {

	using namespace ModelDescriptionUtilities;
	typedef ModelDescription::Properties Properties;

	const ModelDescription* description = fmu_->description;
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

	//nValueRefs_ = varMap_.size();
}


fmiStatus FMUCoSimulation::instantiate( const string& instanceName,
					const fmiReal timeout,
					const fmiBoolean visible,
					const fmiBoolean interactive )
{
	instanceName_ = instanceName;

	if ( fmu_ == 0 ) { return lastStatus_ = fmiError; }

	time_ = 0.;

	const string& guid = fmu_->description->getGUID();
	const string& type = fmu_->description->getMIMEType();


	instance_ = fmu_->functions->instantiateSlave( instanceName_.c_str(), guid.c_str(),
		fmu_->fmuLocation.c_str(), type.c_str(), timeout, visible, interactive, callbacks_, loggingOn_ );

	if ( 0 == instance_ ) return lastStatus_ = fmiError;

	lastStatus_ = fmu_->functions->setDebugLogging( instance_, loggingOn_ );

	return lastStatus_;
}


fmiStatus FMUCoSimulation::initialize( const fmiReal tStart,
				       const fmiBoolean stopTimeDefined,
				       const fmiReal tStop )
{
	if ( 0 == instance_ ) {
		return lastStatus_ = fmiError;
	}

	time_ = tStart;
	
	return lastStatus_ = fmu_->functions->initializeSlave( instance_, tStart, stopTimeDefined, tStop );
}


fmiReal FMUCoSimulation::getTime() const
{
	return time_;
}


fmiStatus FMUCoSimulation::setValue( fmiValueReference valref, fmiReal& val )
{
	return lastStatus_ = fmu_->functions->setReal( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::setValue( fmiValueReference valref, fmiInteger& val )
{
	return lastStatus_ = fmu_->functions->setInteger( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::setValue( fmiValueReference valref, fmiBoolean& val )
{
	return lastStatus_ = fmu_->functions->setBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::setValue( fmiValueReference valref, string& val )
{
	const char* cString = val.c_str();
	return lastStatus_ = fmu_->functions->setString( instance_, &valref, 1, &cString );
}


fmiStatus FMUCoSimulation::setValue(fmiValueReference* valref, fmiReal* val, size_t ival)
{
	return lastStatus_ = fmu_->functions->setReal(instance_, valref, ival, val);
}


fmiStatus FMUCoSimulation::setValue(fmiValueReference* valref, fmiInteger* val, size_t ival)
{
	return lastStatus_ = fmu_->functions->setInteger(instance_, valref, ival, val);
}


fmiStatus FMUCoSimulation::setValue(fmiValueReference* valref, fmiBoolean* val, size_t ival)
{
	return lastStatus_ = fmu_->functions->setBoolean(instance_, valref, ival, val);
}


fmiStatus FMUCoSimulation::setValue(fmiValueReference* valref, string* val, size_t ival)
{
	const char** cStrings = new const char*[ival];

	for ( size_t i = 0; i < ival; i++ ) {
		cStrings[i] = val[i].c_str();
	}
	lastStatus_ = fmu_->functions->setString(instance_, valref, ival, cStrings);
	delete [] cStrings;
	return lastStatus_;
}


fmiStatus FMUCoSimulation::setValue( const string& name, fmiReal val )
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


fmiStatus FMUCoSimulation::setValue( const string& name, fmiInteger val )
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


fmiStatus FMUCoSimulation::setValue( const string& name, fmiBoolean val )
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


fmiStatus FMUCoSimulation::setValue( const string& name, string val )
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


fmiStatus FMUCoSimulation::getValue( fmiValueReference valref, fmiReal& val )
{
	return lastStatus_ = fmu_->functions->getReal( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference valref, fmiInteger& val )
{
	return lastStatus_ = fmu_->functions->getInteger( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference valref, fmiBoolean& val )
{
	return lastStatus_ = fmu_->functions->getBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference valref, string& val )
{
	const char* cString;
	lastStatus_ = fmu_->functions->getString( instance_, &valref, 1, &cString );
	val = string( cString );
	return lastStatus_;
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference* valref, fmiReal* val, size_t ival )
{
	return lastStatus_ = fmu_->functions->getReal( instance_, valref, ival, val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference* valref, fmiInteger* val, size_t ival )
{
	return lastStatus_ = fmu_->functions->getInteger( instance_, valref, ival, val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference* valref, fmiBoolean* val, size_t ival )
{
	return lastStatus_ = fmu_->functions->getBoolean( instance_, valref, ival, val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference* valref, string* val, size_t ival )
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


fmiStatus FMUCoSimulation::getValue( const string& name, fmiReal& val )
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


fmiStatus FMUCoSimulation::getValue( const string& name, fmiInteger& val )
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


fmiStatus FMUCoSimulation::getValue( const string& name, fmiBoolean& val )
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


fmiStatus FMUCoSimulation::getValue( const string& name, string& val )
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

fmiReal FMUCoSimulation::getRealValue( const string& name )
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


fmiInteger FMUCoSimulation::getIntegerValue( const string& name )
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


fmiBoolean FMUCoSimulation::getBooleanValue( const string& name )
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


fmiString FMUCoSimulation::getStringValue( const string& name )
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


fmiStatus FMUCoSimulation::getLastStatus() const
{
	return lastStatus_;
}


fmiValueReference FMUCoSimulation::getValueRef( const string& name ) const
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find(name);

	if ( it != varMap_.end() ) {
		return it->second;
	} else {
		return fmiUndefinedValueReference;
	}
}


fmiStatus FMUCoSimulation::doStep( fmiReal currentCommunicationPoint,
				   fmiReal communicationStepSize,
				   fmiBoolean newStep )
{
	if ( abs( time_ - currentCommunicationPoint ) > timeDiffResolution_ )
	{
		string ret( "requested current communication point does not match FMU-internal time" );
		logger( fmiError, "ABORT", ret );
		return fmiError;
	}

	lastStatus_ = fmu_->functions->doStep( instance_, time_,
						    communicationStepSize, newStep );

	if ( fmiOK == lastStatus_ ) time_ += communicationStepSize;

	return lastStatus_;
}


fmiStatus FMUCoSimulation::setCallbacks( cs::fmiCallbackLogger logger,
	cs::fmiCallbackAllocateMemory allocateMemory,
	cs::fmiCallbackFreeMemory freeMemory,
	cs::fmiStepFinished stepFinished )
{
	
	if ( ( 0 == logger ) || ( 0 == allocateMemory ) || ( 0 == freeMemory ) ) {
		this->logger( fmiError, "ERROR", "callback function pointer(s) invalid" );
		return fmiError;
	}

	callbacks_.logger = logger;
	callbacks_.allocateMemory = allocateMemory;
	callbacks_.freeMemory = freeMemory;
	callbacks_.stepFinished = stepFinished;

	return fmiOK;
}


void FMUCoSimulation::logger( fmiStatus status, const string& category, const string& msg ) const
{
	callbacks_.logger( instance_, instanceName_.c_str(), status, category.c_str(), msg.c_str() );
}


void FMUCoSimulation::logger( fmiStatus status, const char* category, const char* msg ) const
{
	callbacks_.logger( instance_, instanceName_.c_str(), status, category, msg );
}


size_t FMUCoSimulation::nStates() const
{
	return 0;
}


size_t FMUCoSimulation::nEventInds() const
{
	return 0;
}


size_t FMUCoSimulation::nValueRefs() const
{
	return varMap_.size();
}

const ModelDescription* FMUCoSimulation::getModelDescription() const
{
	assert(getLastStatus() != fmiOK || fmu_);
	if (fmu_) {
		assert(fmu_->description != NULL);
		return fmu_->description;
	} else {
		return NULL;
	}
}

FMIVariableType FMUCoSimulation::getType( const string& variableName ) const
{
	map<string,FMIVariableType>::const_iterator it = varTypeMap_.find( variableName );

	if ( it == varTypeMap_.end() ) {
		string ret = variableName + string( " does not exist" );
		logger( fmiDiscard, "WARNING", ret );
		return fmiTypeUnknown;
	}

	return it->second;
}


bool
FMUCoSimulation::canHandleVariableCommunicationStepSize() const
{
	return getCoSimToolCapabilities<bool>( "canHandleVariableCommunicationStepSize" );
}


bool
FMUCoSimulation::canHandleEvents() const
{
	return getCoSimToolCapabilities<bool>( "canHandleEvents" );
}


bool
FMUCoSimulation::canRejectSteps() const
{
	return getCoSimToolCapabilities<bool>( "canRejectSteps" );
}


bool
FMUCoSimulation::canInterpolateInputs() const
{
	return getCoSimToolCapabilities<bool>( "canInterpolateInputs" );
}


size_t
FMUCoSimulation::maxOutputDerivativeOrder() const
{
	return getCoSimToolCapabilities<size_t>( "maxOutputDerivativeOrder" );
}


bool
FMUCoSimulation::canRunAsynchronuously() const
{
	return getCoSimToolCapabilities<bool>( "canRunAsynchronuously" );
}


bool
FMUCoSimulation::canSignalEvents() const
{
	return getCoSimToolCapabilities<bool>( "canSignalEvents" );
}


bool 
FMUCoSimulation::canBeInstantiatedOnlyOncePerProcess() const
{
	return getCoSimToolCapabilities<bool>( "canBeInstantiatedOnlyOncePerProcess" );
}


bool
FMUCoSimulation::canNotUseMemoryManagementFunctions() const
{
	return getCoSimToolCapabilities<bool>( "canNotUseMemoryManagementFunctions" );
}


void
FMUCoSimulation::sendDebugMessage( const string& msg ) const
{
	logger( fmiOK, "DEBUG", msg );
}

} // namespace fmi_1_0
