/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file FMUCoSimulation.cpp
 */

#ifdef FMI_DEBUG
#include <iostream>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <cassert>
#include <cmath>
#include <limits>

#include "common/FMIPPConfig.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/fmi_v1.0/fmi_cs.h"

#include "import/base/include/FMUCoSimulation.h"
#include "import/base/include/ModelManager.h"


/// \FIXME: Need mechanism to provide custom "stepFinished" function.
static cs::fmiCallbackFunctions functions = { FMUCoSimulation::logger, calloc, free, 0 };


using namespace std;


FMUCoSimulation::FMUCoSimulation( const string& fmuPath,
				  const string& modelName ) :
	instance_( NULL )
{
#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif

	ModelManager& manager = ModelManager::getModelManager();
	fmuPath_ = fmuPath;
	fmu_ = manager.getSlave( fmuPath_, modelName );
	readModelDescription();

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::ctor] DONE." << endl;
#endif
}


FMUCoSimulation::FMUCoSimulation( const FMUCoSimulation& fmu ) :
	instance_( NULL )
{
#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::ctor]" << endl; fflush( stdout );
#endif

	fmuPath_ = fmu.fmuPath_;
	fmu_ = fmu.fmu_;
	varMap_ = fmu.varMap_;

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::ctor] DONE." << endl; fflush( stdout );
#endif
}


FMUCoSimulation::~FMUCoSimulation()
{
#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::dtor] instance_ = " << instance_ << endl; fflush( stdout );
#endif

	if ( instance_ ) {
		fmu_->functions->terminateSlave( instance_ );
		fmu_->functions->freeSlaveInstance( instance_ );
	}

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::dtor] DONE." << endl; fflush( stdout );
#endif
}


void FMUCoSimulation::readModelDescription() {

	using namespace ModelDescriptionUtilities;
	typedef ModelDescription::Properties Properties;

	const ModelDescription* description = fmu_->description;
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

	//nValueRefs_ = varMap_.size();
}


fmiStatus FMUCoSimulation::instantiate( const string& instanceName,
					const fmiReal& timeout,
					const fmiBoolean& visible,
					const fmiBoolean& interactive,
					const fmiBoolean& loggingOn )
{
	instanceName_ = instanceName;

	if ( fmu_ == 0 ) {
		return lastStatus_ = fmiError;
	}

#ifdef FMI_DEBUG
	// General information ...
	cout << "[FMUCoSimulation::instantiate] Types Platform: " << fmu_->functions->getTypesPlatform()
	     << ", FMI Version:  " << fmu_->functions->getVersion() << endl; fflush( stdout );
#endif

	// Basic settings: @todo from a menu.
	time_ = 0.;

	// Memory allocation.
	// Instantiation of the model: @todo from menu.
	// get this right ;) !!!
	const string& guid = fmu_->description->getGUID();
	const string& type = fmu_->description->getMIMEType();

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::instantiate] GUID = " << guid << endl; fflush( stdout );
	cout << "[FMUCoSimulation::instantiate] type = " << type << endl; fflush( stdout );
	cout << "[FMUCoSimulation::instantiate] instanceName = " << instanceName_ << endl; fflush( stdout );
	cout << "[FMUCoSimulation::instantiate] fmuPath_ = " << fmuPath_ << endl; fflush( stdout );
#endif

	instance_ = fmu_->functions->instantiateSlave( instanceName_.c_str(), guid.c_str(),
						       fmuPath_.c_str(), type.c_str(),
						       timeout, visible, interactive,
						       functions, fmiTrue );

	if ( 0 == instance_ ) {
#ifdef FMI_DEBUG
		cout << "[FMUCoSimulation::instantiate] instantiateSlave failed. " << endl; fflush( stdout );
#endif
		return lastStatus_ = fmiError;
	}

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::instantiate] instance_ = " << instance_ << endl; fflush( stdout );
#endif

	lastStatus_ = fmu_->functions->setDebugLogging( instance_, loggingOn );

	if ( loggingOn ) {
		functions.logger( instance_, instanceName_.c_str(), lastStatus_, "?", "Model instance initialized"); fflush( stdout );
	}

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::instantiate] DONE. lastStatus_ = " << lastStatus_ << endl; fflush( stdout );
#endif

	return lastStatus_;
}


fmiStatus FMUCoSimulation::initialize( const fmiReal& tStart,
				       const fmiBoolean& stopTimeDefined,
				       const fmiReal& tStop )
{
	if ( 0 == instance_ ) {
		return lastStatus_ = fmiError;
	}

	// Basic settings.
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


fmiStatus FMUCoSimulation::setValue( fmiValueReference valref, std::string& val )
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


fmiStatus FMUCoSimulation::setValue(fmiValueReference* valref, std::string* val, size_t ival)
{
	const char** cStrings = new const char*[ival];

	for ( std::size_t i = 0; i < ival; i++ ) {
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
		logger( fmiDiscard, ret );
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
		logger( fmiDiscard, ret );
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
		logger( fmiDiscard, ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiStatus FMUCoSimulation::setValue( const string& name, std::string val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	const char* cString = val.c_str();

	if ( it != varMap_.end() ) {
		return lastStatus_ = fmu_->functions->setString( instance_, &it->second, 1, &cString );
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
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


fmiStatus FMUCoSimulation::getValue( fmiValueReference valref, std::string& val )
{
	const char* cString;
	lastStatus_ = fmu_->functions->getString( instance_, &valref, 1, &cString );
	val = std::string( cString );
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


fmiStatus FMUCoSimulation::getValue( fmiValueReference* valref, std::string* val, size_t ival )
{
	const char** cStrings = 0;
	
	lastStatus_ = fmu_->functions->getString( instance_, valref, ival, cStrings );

	if ( 0 != cStrings ) {
		for ( std::size_t i = 0; i < ival; i++ ) {
			val[i] = std::string( cStrings[i] );
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
		logger( fmiDiscard, ret );
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
		logger( fmiDiscard, ret );
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
		logger( fmiDiscard, ret );
		return lastStatus_ = fmiDiscard;
	}
}


fmiStatus FMUCoSimulation::getValue( const string& name, std::string& val )
{
	map<string,fmiValueReference>::const_iterator it = varMap_.find( name );
	const char* cString;

	if ( it != varMap_.end() ) {
		lastStatus_ = fmu_->functions->getString( instance_, &it->second, 1, &cString );
		val = std::string( cString );
		return lastStatus_;
	} else {
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
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
		val[0] = std::numeric_limits<fmiReal>::quiet_NaN();
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
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
		val[0] = std::numeric_limits<fmiInteger>::quiet_NaN();
		string ret = name + string( " does not exist" );
		logger( fmiDiscard, ret );
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
		logger( fmiDiscard, ret );
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
		logger( fmiDiscard, ret );
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
	/// \FIXME Replace hard-coded value below with something more sensible.
	if ( abs( time_ - currentCommunicationPoint ) > 1e-9 )
	{
		string ret( "requested current communication point does not match FMU-internal time" );
		logger( fmiError, ret );
		return fmiError;
	}

	fmiStatus status = fmu_->functions->doStep( instance_, currentCommunicationPoint,
						    communicationStepSize, newStep );

	if ( fmiOK == status ) time_ += communicationStepSize;

	return status;
}


void FMUCoSimulation::logger( fmiStatus status, const string& msg ) const
{
	functions.logger( instance_, instanceName_.c_str(), status, "?", msg.c_str() );
}


void FMUCoSimulation::logger( fmiStatus status, const char* msg ) const
{
	functions.logger( instance_, instanceName_.c_str(), status, "?", msg );
}


void FMUCoSimulation::logger( fmiComponent m, fmiString instanceName,
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


FMIType FMUCoSimulation::getType( const string& variableName ) const
{
	map<string,FMIType>::const_iterator it = varTypeMap_.find( variableName );

	if ( it == varTypeMap_.end() ) {
		string ret = variableName + string( " does not exist" );
		logger( fmiDiscard, ret );
		return fmiTypeUnknown;
	}

	return it->second;
}
