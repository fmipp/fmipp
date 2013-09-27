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
#include <limits>

#include "FMUCoSimulation.h"
#include "ModelManager.h"


static  fmiCallbackFunctions functions = { FMUCoSimulation::logger, calloc, free };


using namespace std;


FMUCoSimulation::FMUCoSimulation( const string& fmuPath,
				  const string& modelName )
{
#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif

	ModelManager& manager = ModelManager::getModelManager();
	fmuPath_ = fmuPath;
	fmuFun_ = manager.getSlave( fmuPath_, modelName );
	readModelDescription();

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::ctor] DONE." << endl;
#endif
}


FMUCoSimulation::FMUCoSimulation( const FMUCoSimulation& fmu )
{
#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::ctor]" << endl; fflush( stdout );
#endif

	fmuPath_ = fmu.fmuPath_;
	fmuFun_ = fmu.fmuFun_;
	varMap_ = fmu.varMap_;

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::ctor] DONE." << endl; fflush( stdout );
#endif
}


FMUCoSimulation::~FMUCoSimulation()
{
	if ( instance_ ) {
		fmuFun_->terminateSlave( instance_ );
		fmuFun_->freeSlaveInstance( instance_ );
	}
}


void FMUCoSimulation::readModelDescription() {

	for ( size_t i = 0; fmuFun_->modelDescription->modelVariables[i]; ++i ) {
		ScalarVariable* var = (ScalarVariable*) fmuFun_->modelDescription->modelVariables[i];
		varMap_.insert( make_pair( getString( var,att_name ), getValueReference( var ) ) );
	}

	// nValueRefs_ = varMap_.size();
}


fmiStatus FMUCoSimulation::instantiate( const string& instanceName,
					const fmiReal& timeout,
					const fmiBoolean& visible,
					const fmiBoolean& interactive,
					const fmiBoolean& loggingOn )
{
	instanceName_ = instanceName;

	if ( fmuFun_ == 0 ) {
		return fmiError;
	}

#ifdef FMI_DEBUG
	// General information ...
	cout << "[FMUCoSimulation::instantiate] Types Platform: " << fmuFun_->getTypesPlatform()
	     << ", FMI Version:  " << fmuFun_->getVersion() << endl; fflush( stdout );
#endif

	// Basic settings: @todo from a menu.
	time_ = 0.;

	// Memory allocation.
	// Instantiation of the model: @todo from menu.
	// get this right ;) !!!
	const char* guid = getString( fmuFun_->modelDescription, att_guid );
	const char* type = getString( fmuFun_->modelDescription->cosimulation->model, att_type );

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::instantiate] GUID = " << guid << endl; fflush( stdout );
	cout << "[FMUCoSimulation::instantiate] type = " << type << endl; fflush( stdout );
	cout << "[FMUCoSimulation::instantiate] instanceName = " << instanceName_ << endl; fflush( stdout );
	cout << "[FMUCoSimulation::instantiate] fmuPath_ = " << fmuPath_ << endl; fflush( stdout );
#endif

	instance_ = fmuFun_->instantiateSlave( instanceName_.c_str(), guid, fmuPath_.c_str(),
					       type, timeout, visible, interactive, functions, fmiTrue );

	if (0 == instance_) {
#ifdef FMI_DEBUG
		cout << "[FMUCoSimulation::instantiate] instantiateSlave failed. " << endl; fflush( stdout );
#endif
		return fmiError;
	}

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::instantiate] instance_ = " << instance_ << endl; fflush( stdout );
#endif

	fmiStatus status = fmuFun_->setDebugLogging( instance_, loggingOn );

	if ( loggingOn ) {
		functions.logger( instance_, instanceName_.c_str(), status, "?", "Model instance initialized"); fflush( stdout );
	}

#ifdef FMI_DEBUG
	cout << "[FMUCoSimulation::instantiate] DONE. status = " << status << endl; fflush( stdout );
#endif

	return status;
}


fmiStatus FMUCoSimulation::initialize( const fmiReal& tStart,
				       const fmiBoolean& stopTimeDefined,
				       const fmiReal& tStop )
{
	if ( 0 == instance_ ) {
		return fmiError;
	}

	// Basic settings.
	fmiStatus status = fmuFun_->initializeSlave( instance_, tStart, stopTimeDefined, tStop );
	return status;
}


fmiReal FMUCoSimulation::getTime() const
{
	return time_;
}


fmiStatus FMUCoSimulation::setValue( fmiValueReference valref, fmiReal& val )
{
	return fmuFun_->setReal( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::setValue( fmiValueReference valref, fmiInteger& val )
{
	return fmuFun_->setInteger( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::setValue( fmiValueReference valref, fmiBoolean& val )
{
	return fmuFun_->setBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::setValue( fmiValueReference valref, std::string& val )
{
	const char* cString = val.c_str();
	return fmuFun_->setString( instance_, &valref, 1, &cString );
}


fmiStatus FMUCoSimulation::setValue(fmiValueReference* valref, fmiReal* val, size_t ival)
{
	return fmuFun_->setReal(instance_, valref, ival, val);
}


fmiStatus FMUCoSimulation::setValue(fmiValueReference* valref, fmiInteger* val, size_t ival)
{
	return fmuFun_->setInteger(instance_, valref, ival, val);
}


fmiStatus FMUCoSimulation::setValue(fmiValueReference* valref, fmiBoolean* val, size_t ival)
{
	return fmuFun_->setBoolean(instance_, valref, ival, val);
}


fmiStatus FMUCoSimulation::setValue(fmiValueReference* valref, std::string* val, size_t ival)
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


fmiStatus FMUCoSimulation::setValue( const string& name, fmiReal val )
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


fmiStatus FMUCoSimulation::setValue( const string& name, fmiInteger val )
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


fmiStatus FMUCoSimulation::setValue( const string& name, fmiBoolean val )
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


fmiStatus FMUCoSimulation::setValue( const string& name, std::string val )
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


fmiStatus FMUCoSimulation::getValue( fmiValueReference valref, fmiReal& val ) const
{
	return fmuFun_->getReal( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference valref, fmiInteger& val ) const
{
	return fmuFun_->getInteger( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference valref, fmiBoolean& val ) const
{
	return fmuFun_->getBoolean( instance_, &valref, 1, &val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference valref, std::string& val ) const
{
	const char* cString;
	return fmuFun_->getString( instance_, &valref, 1, &cString );
	val = std::string( cString );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference* valref, fmiReal* val, size_t ival ) const
{
	return fmuFun_->getReal( instance_, valref, ival, val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference* valref, fmiInteger* val, size_t ival ) const
{
	return fmuFun_->getInteger( instance_, valref, ival, val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference* valref, fmiBoolean* val, size_t ival ) const
{
	return fmuFun_->getBoolean( instance_, valref, ival, val );
}


fmiStatus FMUCoSimulation::getValue( fmiValueReference* valref, std::string* val, size_t ival ) const
{
	const char** cStrings;
	fmiStatus status;

	status = fmuFun_->getString( instance_, valref, ival, cStrings );
	for ( std::size_t i = 0; i < ival; i++ ) {
		val[i] = std::string( cStrings[i] );
	}
	return status;
}


fmiStatus FMUCoSimulation::getValue( const string& name, fmiReal& val ) const
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


fmiStatus FMUCoSimulation::getValue( const string& name, fmiInteger& val ) const
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


fmiStatus FMUCoSimulation::getValue( const string& name, fmiBoolean& val ) const
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


fmiStatus FMUCoSimulation::getValue( const string& name, std::string& val ) const
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



fmiValueReference FMUCoSimulation::getValueRef( const string& name ) const {
	map<string,fmiValueReference>::const_iterator it = varMap_.find(name);

	if ( it != varMap_.end() ) {
		return it->second;
	} else {
		return fmiUndefinedValueReference;
	}
}


fmiStatus FMUCoSimulation::doStep( fmiReal currentCommunicationPoint,
				   fmiReal communicationStepSize,
				   fmiBoolean newStep ) const
{
	fmuFun_->doStep( instance_, currentCommunicationPoint, communicationStepSize, newStep );
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
