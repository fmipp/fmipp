/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file PowerFactory.cpp
/// \authors Roman Schwalbe, Matthias Stifter, Edmund Widl

#ifndef _WIN32_WINDOWS                      // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS _WIN32_WINNT_WIN7    // Target Windows 7 or later.
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#endif

// Check for compilation with Visual Studio 2010 (required).
#if ( _MSC_VER == 1800 )
#include "windows.h"
#else
#error This project requires Visual Studio 2013.
#endif

// PowerFactory API includes.
#include "v1/Api.hpp"

// Project includes.
#include "PowerFactory.h"
#include "PowerFactoryRMS.h"
#include "SimpleLogger.h"
#include "Utils.h"
#include "SmartValue.h"


extern "C" {
	typedef Api* (__cdecl *CREATEAPI)(const char* username, const char* password, const char* commandLineArguments);
	typedef void (__cdecl *DESTROYAPI)(Api*&);
}


using namespace pf_api;


// Initialize pointer to PF API for class SmartValue.
Api* SmartValue::api = 0;

// Initialize pointer to DLL handle for class PowerFactory.
HINSTANCE PowerFactory::dllHandle_ = 0;


PowerFactory* PowerFactory::create()
{	
	if ( 0 != SmartValue::api ) {
		std::string err( "[PowerFactory::create] instance of PowerFactory already exists, creation of multiple instances not allowed" );
		throw( std::exception( err.c_str() ) );
	}

	dllHandle_ = LoadLibrary( TEXT( "digapi.dll" ) ); // Load digapi.dll
	if ( 0 == dllHandle_ ) {
		std::string err( "[PowerFactory::create] Unable to load digapi.dll");
		throw( std::exception( err.c_str() ) );
	}

	CREATEAPI createApi = (CREATEAPI) GetProcAddress( (struct HINSTANCE__*) dllHandle_, "CreateApiInstanceV1" );

	LOG_DEBUG << "[PowerFactory::create] creating API instance..." << std::endl;

	Api* api = createApi( 0, 0, 0 );
	if ( 0 == api ) {
		std::string err( "[PowerFactory::create] was not able to create API instance" );
		throw( std::exception( err.c_str() ) );
	} else {
		SmartValue::api = api;
	}
	
	LOG_DEBUG << "[PowerFactory::create] API instance successfully created." << std::endl;
	
	const char* installDir = api->GetApplication()->GetInstallationDirectory()->GetString();
	return new PowerFactory( api, installDir );
}


PowerFactory::~PowerFactory()
{
	rms_->rmsStop( false );
	deactivateProject();

	SmartValue::api = 0;
	api_ = 0;
	app_ = 0;

	LOG_DEBUG << "[PowerFactory] Releasing API instance..." << std::endl;

	FreeLibrary( dllHandle_ );
	dllHandle_ = 0;
}


PowerFactory::PowerFactory( Api* api, const std::string& pfRootDir ) :
	api_( api ),
	app_( api_->GetApplication() ),
	ldf_( 0 ),
	pfRoot_( pfRootDir ),
	//rcomCall_( "\"" + pfRoot_ + "ENGINE\\digrcom.exe\" -d -p ncacn_ip_tcp -n 127.0.0.1 -e 2001 -c=" ),
	rms_( new PowerFactoryRMS( this ) )
{}


int
PowerFactory::showUI( bool show )
{	
	return execute( show ? "rcom/show" : "rcom/hide" );
}


std::string
PowerFactory::getCurrentUser()
{
	return SmartObject( app_->GetCurrentUser() )->GetName()->GetString();
}


int
PowerFactory::activateProject( const std::string& projectName )
{
	api_->ReleaseObject( ldf_ );
	ldf_ = 0;
	
	// Find the project
	const char* userName = SmartObject( app_->GetCurrentUser() )->GetName()->GetString();
	std::string projFullName = std::string( ".\\" ) + std::string( userName )
		+ std::string( ".intUser\\" ) + projectName + std::string( ".intPrj" );
	Value projVec( Value::VECTOR );
	projVec.VecInsertString( projFullName.c_str() );

	int error = 0;
	SmartValue res( app_->Execute( "SearchFull", &projVec, &error ) );
	if ( 0 != error ) {
		LOG_ERROR << "[PowerFactory::activateProject] error while finding project " << projectName << std::endl;
		return PowerFactory::UndefinedError;
	}

	SmartValue( app_->Execute( "Activate", res, &error ) );
	if ( 0 != error ) {
		LOG_ERROR << "[PowerFactory::activateProject] error while loading project " << projectName << std::endl;
		return PowerFactory::UndefinedError;
	}
	
	SmartObject activProj( app_->GetActiveProject() );
	if ( 0 == *activProj ) {
		LOG_ERROR << "[PowerFactory::activateProject] error getting active project " << projectName << std::endl;
		return PowerFactory::UndefinedError;
	}

	LOG_DEBUG << "[PowerFactory::activateProject] active project: " << activProj->GetName()->GetString() << std::endl;

	SmartObject studyCase( app_->GetActiveStudyCase() );
	LOG_DEBUG << "[PowerFactory::activateProject] active study case: "<< ( *studyCase ? studyCase->GetName()->GetString() : "NONE" ) << std::endl;
	
	return PowerFactory::Ok;
}


int
PowerFactory::deactivateProject()
{	
	clearCachedObjects();
	return execute( "ac/de all" );
}


int
PowerFactory::calculatePowerFlow()
{	
	SmartObject prj( app_->GetActiveProject() );
	if ( !*prj ) return PowerFactory::ProjectNotLoaded;

	if( 0 == ldf_ ) {
		const Value obj( "ComLdf" );
		SmartValue comLdf( app_->Execute( "GetCaseObject", &obj ) );
		ldf_ = static_cast<DataObject*>( comLdf->GetDataObject() );
		if( 0 == ldf_ ) {
			LOG_ERROR << "[PowerFactory::calculatePowerFlow] unable to find load flow object (ComLdf)" << std::endl;
			return PowerFactory::UndefinedError;
		}
	}

	// Perform a load flow calculation.
	int error = 0;
	SmartValue( ldf_->Execute( "Execute", NULL, &error ) );
	if ( error != 0 || isPowerFlowValid() == PowerFactory::LDFnotValid ) {
		LOG_ERROR << "[PowerFactory::calculatePowerFlow] load flow not valid" << std::endl;
		return PowerFactory::LDFnotValid;
	}
	return PowerFactory::Ok;
}


int
PowerFactory::isPowerFlowValid()
{
	int error = 0;
	SmartValue valid( app_->Execute( "IsLdfValid", 0, &error ) );
	if ( 0 == error && 0 != valid.val && 1 == valid->GetInteger() )
		return PowerFactory::Ok;
	else
		return PowerFactory::LDFnotValid;
}


int
PowerFactory::getCalcRelevantObject( const std::string& className, const std::string & objectName, DataObject* &dataObj, bool cache )
{	
	dataObj = 0;
	MapStrDataObj* objMap = 0;
	if ( 1 != dataObjByClassMap_.count( className ) ) {
		if ( true == cache ) {
			int error = getCalcRelevantObjectMap( className, objMap );
			if ( PowerFactory::Ok != error ) {
				LOG_ERROR << "[PowerFactory::getCalcRelevantObject] error while getting "<< objectName << "." << className << std::endl;

				if ( PowerFactory::ObjectNameNotUnique != error ) return error;
			}
		} else {
			int error = 0;
			int classId = app_->GetClassId( className.c_str() );
			if( 0 == classId ) {
				LOG_DEBUG << "[PowerFactory::getCalcRelevantObject] no such class name: " << className << std::endl;
				return PowerFactory::NoSuchClassName;
			}

			SmartValue relevantObj( api_->GetApplication()->GetCalcRelevantObjects() );
			if ( Value::VECTOR != relevantObj->GetType() ) {
				LOG_ERROR << "[PowerFactory::getCalcRelevantObject] error while evaluating CalcrelevantObjects" << std::endl;
				return PowerFactory::UndefinedError;
			}
			
			unsigned int count = relevantObj->VecGetSize( &error );
			if ( 0 != error ) {
				LOG_ERROR << "[PowerFactory::getCalcRelevantObject] error while evaluating CalcrelevantObjects" << std::endl;
				return PowerFactory::UndefinedError;
			}

			for ( unsigned int i = 0; i < count; ++i ) {
				DataObject* obj = static_cast<DataObject*>( relevantObj->VecGetDataObject( i, &error ) );
				if ( 0 != error ) {
					LOG_ERROR << "[PowerFactory::getCalcRelevantObject] error while evaluating object #" << i << " from class " << className << std::endl;
					delete objMap;
					objMap = 0;
					return error;
				}

				std::string name = obj->GetName()->GetString();
				if ( classId == obj->GetClassId() && 0 == name.compare( objectName ) )  {	
					dataObj = obj;
					return Ok;
				}
			}

			return PowerFactory::NoSuchObject;
		}
	}
	else
	{
		objMap = dataObjByClassMap_[className];
	}

	if ( 0 == objMap->count( objectName ) ) {
		LOG_DEBUG << "[PowerFactory::getCalcRelevantObject] no such object " << objectName << "." << className << std::endl;
		return PowerFactory::NoSuchObject;
	}

	dataObj = (*objMap)[objectName];
	return PowerFactory::Ok;
}


int
PowerFactory::getCalcRelevantObjectMap( const std::string &className, MapStrDataObj* &objMap )
{	
	objMap = 0;
	int returnState = PowerFactory::Ok;
	
	if ( 1 != dataObjByClassMap_.count( className ) )
	{	
		int error = 0;
		int classId = app_->GetClassId( className.c_str() );
		if ( 0 == classId ) {
			LOG_DEBUG<<"[PowerFactory::getCalcRelevantObjectMap] no such class name: " <<className << std::endl;
			return PowerFactory::NoSuchClassName;
		}

		SmartValue relevantObj( api_->GetApplication()->GetCalcRelevantObjects() );
		if ( Value::VECTOR != relevantObj->GetType() ) {
			LOG_ERROR << "[PowerFactory::getCalcRelevantObjectMap] error while evaluating CalcrelevantObjects" << std::endl;
			return PowerFactory::UndefinedError;
		}

		unsigned int count = relevantObj->VecGetSize( &error );
		if ( 0 != error ) {
			LOG_ERROR << "[PowerFactory::getCalcRelevantObjectMap] error while evaluating CalcrelevantObjects" << std::endl;
			return PowerFactory::UndefinedError;
		}

		objMap = new MapStrDataObj();
		for ( unsigned int i = 0; i < count; ++i ) {
			DataObject* obj = static_cast<DataObject*>( relevantObj->VecGetDataObject( i, &error ) );

			if ( 0 != error ) {
				LOG_ERROR << "[PowerFactory::getCalcRelevantObjectMap] error while evaluating object #" << i <<" from class " << className << std::endl;
				delete objMap;
				objMap = 0;
				return error;
			}

			if ( classId == obj->GetClassId() ) 
			{	
				std::string name = obj->GetName()->GetString();
				if ( false == addObjToMap( *objMap, name, obj ) )
					returnState = PowerFactory::ObjectNameNotUnique;
				
				MapStrDataObj* nameMap = dataObjByNameMap_[name];
				if ( 0 == nameMap )	{
					nameMap = new MapStrDataObj();
					dataObjByNameMap_[name] = nameMap;
				}

				addObjToMap( *nameMap, obj->GetFullName()->GetString(), obj );
			}
		}

		dataObjByClassMap_[className] = objMap;
	} else {
		objMap = dataObjByClassMap_[className];
	}

	return returnState;
}


int
PowerFactory::getCalcRelevantObjectByName( const std::string& name,DataObject* &obj )
{	
	obj = 0;
	if ( 1 != dataObjByNameMap_.count( name ) ) return PowerFactory::NoSuchObject;

	MapStrDataObj *objMap = dataObjByNameMap_[name];
	if ( 1 != objMap->size() ) {
		LOG_ERROR << "[PowerFactory::getCalcRelevantObjectByName] error: there are " << objMap->size() << " elements cached with the same name (" << name << ")" <<std::endl;
		return PowerFactory::ObjectNameNotUnique;
	}

	obj = objMap->begin()->second;
	return PowerFactory::Ok;
}


int
PowerFactory::cacheCalcRelevantObjectNamesOfClass( const std::string& className )
{	
	MapStrDataObj *objMap = 0;
	return getCalcRelevantObjectMap( className, objMap );
}


int
PowerFactory::setWriteCache( bool enable )
{
	app_->SetWriteCacheEnabled( enable );
	if( app_->IsWriteCacheEnabled() != enable ) return PowerFactory::UndefinedError;
	return PowerFactory::Ok;
}


int PowerFactory::clearCachedObjects()
{	
	// Release pointers.
	MapStrMapStrDataObj::iterator it;
	for ( it = dataObjByClassMap_.begin(); it != dataObjByClassMap_.end(); ++it )
		delete it->second;
	dataObjByClassMap_.clear();

	for( it = dataObjByNameMap_.begin(); it != dataObjByNameMap_.end(); ++it )
		delete it->second;
	dataObjByNameMap_.clear();

	ldf_ = 0;
	rms_->clearCachedObjects();
	dplMap_.clear();
	triggerMap_.clear();

	return PowerFactory::Ok;
}


int
PowerFactory::getAttributeDouble( const std::string& name, const char* parameter, double& value)
{	
	DataObject* obj = 0;
	int error = getCalcRelevantObjectByName( name, obj );
	if( PowerFactory::Ok != error ) return error;
	return getAttributeDouble( obj, parameter, value );
}


int
PowerFactory::getAttributeDouble( DataObject* obj, const char* parameter, double& value )
{	
	if ( 0 == obj ) return PowerFactory::NoSuchObject;
	
	int error = 0;
	value = obj->GetAttributeDouble( parameter, &error );
	if ( 0 != error ) {
		LOG_ERROR << "[PowerFactory::getAttributeDouble] unable to read attribute " << parameter << " from " << obj->GetName()->GetString() << std::endl;
		return PowerFactory::UndefinedError;
	}
	return PowerFactory::Ok;
}


int
PowerFactory::getAttributesDouble( DataObject* obj, std::vector<double>& values )
{	
	if ( 0 == obj ) return PowerFactory::NoSuchObject;
	
	int error = 0;
	SmartValue objAttr = obj->GetAttributes( &error );
	if ( 0 != error ) {
		LOG_ERROR << "[PowerFactory::getAttributesDouble] unable to read attributes from " << obj->GetName()->GetString() << std::endl;
		return PowerFactory::UndefinedError;
	}

	unsigned int size = objAttr->VecGetSize( &error );
	if ( 0 != error ) {
		LOG_DEBUG << "[PowerFactory::getAttributesDouble] invalid arguments received from " <<obj->GetName()->GetString() << std::endl;
		return PowerFactory::UndefinedError;
	}
	
	values.clear();
	for ( unsigned int i = 0; i < size; ++i ) {
		values.push_back( objAttr->VecGetDouble( i, &error ) );
		if ( 0 != error ) {
			LOG_DEBUG << "[PowerFactory::getAttributesDouble] invalid argument #" << i << " received from " << obj->GetName()->GetString() << std::endl;
			return PowerFactory::UndefinedError;
		}
	}
	
	return PowerFactory::Ok;
}




int
PowerFactory::getActiveStudyCaseObject( const char* className,const std::string& objectName, bool recursive, DataObject* &child )
{	
	MapStrDataObj objMap;
	SmartObject activeStudyCase( app_->GetActiveStudyCase() );
	int error = getChildObjects( className, *activeStudyCase, objMap, recursive );
	if ( 0 != error ) return error;

	if ( true == objectName.empty() ) { //get first element if no name is specified
		if ( true == objMap.empty() ) return PowerFactory::NoSuchObject;

		child = objMap.begin()->second;

		if( objMap.size() > 1 ) {
			LOG_DEBUG << "[PowerFactory::getActiveStudyCaseObject] warning: multiple objects found in active study case for class " << className << " - returning first object." << std::endl;
			return PowerFactory::ObjectNameNotUnique;
		}
		
		return PowerFactory::Ok;
	}

	if ( 1 != objMap.count(objectName) ) return PowerFactory::NoSuchObject;
	
	child = objMap[objectName];
	return PowerFactory::Ok;
}


int
PowerFactory::getChildObjects( const char* className, DataObject* obj, MapStrDataObj&objMap, bool recursive ) 
{
	if ( 0 == obj ) return PowerFactory::NoSuchObject;

	int error = 0;
	
	int filterID = app_->GetClassId( className );
	SmartValue children( obj->GetChildren( recursive, filterID ) );

	unsigned int count = children->VecGetSize( &error );
	if ( 0 != error ) {
		LOG_ERROR << "[PowerFactory::getChildObjects] error while evaluating child objects from object " << obj->GetName()->GetString() << std::endl;
		return error;
	}

	for ( unsigned int i = 0; i < count; ++i )
	{
		DataObject* child = static_cast<DataObject*>( children->VecGetDataObject( i, &error ) );
		if( 0 != error || 0 == child ) {
			LOG_ERROR << "[PowerFactory::getChildObjects] error while evaluating child object #" << i << " from object " << obj->GetName()->GetString() << std::endl;
			return error;
		}
		if (child->GetClassId() == filterID) {	
			if ( false == addObjToMap( objMap, child->GetName()->GetString(), child ) ) {
				objMap.clear();
				return PowerFactory::ObjectNameNotUnique;
			}
		}
	}
	return PowerFactory::Ok;
}


int
PowerFactory::setAttributeDouble( const std::string& name, const char* parameter, double value )
{
	DataObject* obj = 0;
	int error = getCalcRelevantObjectByName( name, obj );
	if( PowerFactory::Ok != error ) return error;
	return setAttributeDouble( obj, parameter, value );
}


int
PowerFactory::setAttributeDouble( DataObject* obj, const char* parameter, double value )
{	
	if ( 0 == obj ) return PowerFactory::NoSuchObject;
	
	if ( rms_->rmsIsActive() ) {
		LOG_DEBUG << "[PowerFactory::setAttributeDouble] unable to set attribute " << parameter << " to " << obj->GetName()->GetString() << " because last non-blocking-command not finished" << std::endl;
		return PowerFactory::LastCommandNotFinished;
	}
	
	int error = 0;
	obj->SetAttributeDouble( parameter, value, &error );
	if ( 0 != error ) {
		LOG_ERROR << "[PowerFactory::setAttributeDouble] could not set attribute " << parameter << " from " << obj->GetName()->GetString() << std::endl;
		return PowerFactory::UndefinedError;
	}
	return PowerFactory::Ok;
}


int
PowerFactory::setAttributesDouble( DataObject* obj,const std::vector<double>& values )
{	
	if ( 0 == obj) return PowerFactory::NoSuchObject;
	
	if ( rms_->rmsIsActive() ) {
		LOG_DEBUG << "[PowerFactory::setAttributesDouble] unable to set attributes to " << obj->GetName()->GetString() << " because last non-blocking-command has not finished" << std::endl;
		return PowerFactory::LastCommandNotFinished;
	}

	Value objAttr( Value::VECTOR );
	std::vector<double>::const_iterator itValues;
	for ( itValues = values.begin(); itValues != values.end(); ++itValues )
		objAttr.VecInsertDouble( *itValues );

	int error=0;
	obj->SetAttributes( &objAttr, &error );

	if ( 0 != error ) {
		LOG_ERROR <<"[PowerFactory::setAttributesDouble] unable to set attributes to " << obj->GetName()->GetString() << std::endl;
		return PowerFactory::UndefinedError;
	}
	
	return PowerFactory::Ok;
}


int
PowerFactory::setMatrixAttributeDouble( DataObject *obj, const char* attribute, int row, int col, double value )
{	
	if ( 0 == obj ) return PowerFactory::NoSuchObject;

	if ( rms_->rmsIsActive() ) {
		LOG_DEBUG<<"[PowerFactory::setMatrixAttributeDouble] unable to set attribute " << attribute << " to " << obj->GetName()->GetString() << " because last non-blocking-command has not finished" << std::endl;
		return PowerFactory::LastCommandNotFinished;
	}
	
	int error = 0;
	obj->SetAttributeDouble( attribute, value, row, col, &error );
	if ( 0 != error ) {
		LOG_ERROR << "[PowerFactory::setMatrixAttributeDouble] unable to set attribute " << attribute << " from " << obj->GetName()->GetString() << std::endl;
		return PowerFactory::UndefinedError;
	}

	return PowerFactory::Ok;
}


int
PowerFactory::setCharacteristicsTrigger( std::string triggerName, double value )
{	
	if ( true == triggerMap_.empty() ) {
		SmartObject activeStudyCase( app_->GetActiveStudyCase() );
		int error = getChildObjects( "SetTrigger", *activeStudyCase, triggerMap_, false );
		if ( 0 != error ) return error;
	}

	if ( 1 != triggerMap_.count( triggerName ) ) {
		LOG_ERROR << "[PowerFactory::setCharacteristicsTrigger] unable to find trigger " << triggerName << std::endl;
		return PowerFactory::NoSuchObject;
	}

	return setAttributeDouble( triggerMap_[triggerName], "ftrigger", value );
}


int
PowerFactory::defineTransferAttributes( const char* className, const char* parameters )
{	
	int error = 0;
	app_->DefineTransferAttributes( className, parameters, &error );
	if ( 0 != error ) {
		LOG_ERROR<<"[PowerFactory::defineTransferAttributes] unable to set transfer attributes " << parameters << " for classname " << className << std::endl;
		return PowerFactory::UndefinedError;
	}
	return PowerFactory::Ok;
}


int
PowerFactory::execute(const char* cmd) 
{
	int error = 0;
	const Value val( cmd );
	app_->Execute( "ExecuteCmd", &val, &error );

	if ( 0 != error ) LOG_ERROR << "[PowerFactory::execute] error: " << cmd <<std::endl;

	return error;
}


int
PowerFactory::execute( DataObject* &obj, const char* cmd )
{	
	if ( 0 == obj ) return PowerFactory::NoSuchObject;

	int error = 0;
	SmartValue( obj->Execute( cmd, 0, &error ) );
	if ( 0 != error ) {
		LOG_ERROR << "[PowerFactory::execute] error while executing " << obj->GetName()->GetString() << "." << cmd << ": " << error << std::endl;
	}
	return error;
}


/*int
 *PowerFactory::executeRCOMcommand( const char* cmd, bool blocking )
 *{	
 *	std::string cmdStr = '\"' + rcomCall + '\"' + cmd + '\"' + '\"';
 *	if ( false == blocking ) cmdStr = "cmd.exe /C " + cmdStr;
 *
 *	int res = system( cmdStr.c_str() );
 *	if ( 0 != res )
 *		LOG_DEBUG << "[PowerFactory::executeRCOMcommand] RCOM commando '" << cmdStr
 *		          << "' returned " << res << std::endl;
 *	return res;
 *}
 */


int 
PowerFactory::executeDPL( const std::string& dplScript )
{	
	VecVariant noArgs,noRes;
	return executeDPL( dplScript, noArgs, noRes );
}


int
PowerFactory::executeDPL( const std::string& dplScript, const VecVariant& arguments, VecVariant& results)
{	
	int error = 0;

	if ( true == dplMap_.empty() )
	{
		error = getChildObjects( "ComDpl", app_->GetActiveStudyCase(), dplMap_, true );
		if( 0 != error )
		{
			LOG_ERROR << "[PowerFactory::executeDPL] error while searching for DPL script " << dplScript << std::endl;
			return error;
		}
	}

	if( 1 != dplMap_.count( dplScript ) )
	{
		LOG_ERROR << "[PowerFactory::executeDPL] error: could not find DPL script " << dplScript << std::endl;
		return PowerFactory::NoSuchObject;
	}

	DataObject *script = dplMap_[dplScript];
	int rows = 0, cols = 0;
	script->GetAttributeSize( "IntExpr", rows, cols );

	if( static_cast<int>( arguments.size() ) < rows )
	{
		LOG_DEBUG << "[PowerFactory::executeDPL] warning: only " << arguments.size() 
		          << " arguments specified for " << dplScript << ", which has "
		          << rows << " arguments. Not every argument will be set." << std::endl;
	}

	if( static_cast<int>( arguments.size() ) > rows )
	{
		LOG_ERROR << "[PowerFactory::executeDPL] error: too many arguments ("
		          << arguments.size() << ") specified for " << dplScript
		          << " with " << rows << " arguments. Can't call script." << std::endl;
		return PowerFactory::UndefinedError;
	}

	for( unsigned int i = 0; i < arguments.size(); ++i )
	{
		script->SetAttributeString( "IntExpr", utils::convertVariantToString( arguments[i] ).c_str(), i, 0, &error );
		if( PowerFactory::Ok != error )
		{
			LOG_ERROR << "[PowerFactory::executeDPL] error while generating input parameter for DPL script "
			          << dplScript << " for arg number " << i << std::endl;
			return error;
		}
	}

	SmartValue res = script->Execute( "Execute", NULL, &error );

	results.clear();
	script->GetAttributeSize( "IntResExpr", rows, cols );

	for( int i = 0; i < rows; ++i )
	{
		SmartValue val( script->GetAttributeString( "IntResExpr", i, &error ) );
		if( val->STRING != val->GetType() )
		{
			LOG_DEBUG << "[PowerFactory::executeDPL] unable to read result #" << i << " of "<< dplScript <<". Will be ignored!" << std::endl;
			continue;
		}

		SmartValue type( script->GetAttributeString( "IntResType", i, &error ) );
		if( val->STRING != type->GetType() )
		{
			LOG_DEBUG << "[PowerFactory::executeDPL] unable to read result-type #" << i << " of " << dplScript << ". Will be ignored!" << std::endl;
			continue;
		}
		
		results.push_back( utils::convertStringToVariant( type->GetString(), val->GetString() ) );
	}
	
	if( 0 != error )
	{	LOG_ERROR << "[PowerFactory::executeDPL] error while executing DPL script " << dplScript << std::endl;
		return PowerFactory::UndefinedError;
	}

	return PowerFactory::Ok;
}


bool
PowerFactory::addObjToMap( MapStrDataObj& map, const std::string identifier, DataObject *obj )
{	
	if ( 0 != map.count( identifier ) ) {
		LOG_DEBUG << "[PowerFactory::addObjToMap] warning: object indentifier "
		          << identifier << " not unique for " << obj->GetFullName()->GetString()
		          << " and " << map[identifier]->GetFullName()->GetString() << std::endl;
		return false;
	}

	map[identifier] = obj;
	return true;
}
