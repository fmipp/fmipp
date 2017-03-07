/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _PF_API_POWERFACTORY_H
#define _PF_API_POWERFACTORY_H

/**
 * \file PowerFactory.h 
 *
 * \class PowerFactory PowerFactory.h 
 * High-level interface for PowerFactory simulation.
 *  
 * \authors Roman Schwalbe, Matthias Stifter, Edmund Widl
 */

#include <vector>
#include <map>
#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace api {
	namespace v1 {
		class DataObject;
		class Api;
		class Application;
	}
	class Value;
}

typedef api::v1::DataObject DataObject;
typedef api::v1::Application Application;
typedef api::v1::Api Api;
typedef api::Value Value;


namespace pf_api {


class PowerFactoryRMS;


class PowerFactory
{

public:

	typedef std::map<std::string, DataObject*> MapStrDataObj;
	typedef std::map<std::string,MapStrDataObj*> MapStrMapStrDataObj;
	typedef std::map<std::string,std::string> MapStrStr;

	typedef boost::variant<int,double,std::string> Variant;
	typedef std::vector<Variant> VecVariant;

	
	enum returnState {
		Ok = 0,
		ProjectNotLoaded = -1,
		LDFnotValid = -2,
		NoSuchObject = -3,
		NoSuchClassName = -4,
		ObjectNameNotUnique = -5,
		LastCommandNotFinished = -6,
		UndefinedError = 1
	};


	static PowerFactory* create();
	~PowerFactory();
	
private:

	/// Private constructor.
	PowerFactory( Api* api, const std::string& pfRootDir );

public:
	
	/// Set visibility of UI during simulation.
	int showUI( bool show );

	/// Activate project.
	int activateProject( const std::string& projectName );
	
	/// Deactivate project.
	int deactivateProject();

	/// Performs a power flow calculation for active project.
	int calculatePowerFlow();

	/// Check if power flow calculation is valid.
	int isPowerFlowValid();

	/// Access PF objects by class and name.
	int getCalcRelevantObject( const std::string &className,
		const std::string &objectName, DataObject* &dataObj, bool cache = true );

	/// Get map of PF objects by class type. ATTENTION: objMap must be a pointer, but will be stored internally and therefore must not be released/deleted!
	int getCalcRelevantObjectMap( const std::string &className, MapStrDataObj* &objMap);
	
	/// Access PF objects by name only.
	int getCalcRelevantObjectByName( const std::string& name, DataObject* &obj );

	/// Load all PF objects into cache for later use.
	int cacheCalcRelevantObjectNamesOfClass( const std::string& className );

	/// Enable PF write cache.
	int setWriteCache( bool enable );
	
	/// Clear PF cache.
	int clearCachedObjects();

	/// Retrieve value of object attribute.
	int getAttributeDouble( const std::string& name, const char* parameter, double& value);

	/// Retrieve value of object attribute.
	int getAttributeDouble( DataObject* obj, const char* parameter, double& value );

	/// Retrieve vector of values of object attributes.
	int getAttributesDouble( DataObject* obj, std::vector<double>& values );

	/// Retrie object associated to active case study.
	int getActiveStudyCaseObject( const char* className,
		const std::string& objectName, bool recursive, DataObject* &child );

	/// Map children from given object.
	int getChildObjects( const char* className, DataObject* obj,
		MapStrDataObj &objMap, bool recursive );

	/// Set value of object attribute.
	int	setAttributeDouble( const std::string& name, const char* parameter, double value);

	/// Set value of object attribute.
	int	setAttributeDouble( DataObject* obj, const char* parameter, double value );

	/// Set values of object attributes.
	int setAttributesDouble( DataObject* obj,const std::vector<double>& values );

	/// Set value of object matrix attribute.
	int setMatrixAttributeDouble( DataObject* obj, const char* attribute,
		int row, int col, double value );

	/// Set trigger value.
	int setCharacteristicsTrigger( std::string triggerName, double value );

	/// Define transfer attribute.
	int defineTransferAttributes( const char* className, const char* parameters );

	/// Executes a command via Execute( "ExecuteCmd", &cmd, error ).
	int execute( const char* cmd );

	/// Execute command associated to PF object.
	int execute( DataObject* &obj, const char* cmd );

	// /// DEPRECATED: Execute command via RCOM.
	// int executeRCOMcommand( const char* cmd, bool blocking = true );

	/// Execute a DPL script that takes no arguments and produces no outputs.
	int executeDPL( const std::string& dplScript );

	/// Execute a DPL script with arguments and retrieve outputs.
	int executeDPL( const std::string& dplScript,
		const VecVariant& arguments, VecVariant& results );

private:

	// Pointers to PF API.
	Api* api_;
	Application* app_;
	DataObject* ldf_;

	// Maps of PF objects.
	MapStrDataObj triggerMap_;
	MapStrDataObj dplMap_;
	MapStrMapStrDataObj dataObjByClassMap_;
	MapStrMapStrDataObj dataObjByNameMap_;

	/// PF installation path.
	const std::string pfRoot_;
	
	// /// DEPRECATED: Path to RCOM.
	// const std::string rcomCall_;
	
	/// Handle for PF API DLL.
	static HINSTANCE dllHandle_;

	bool addObjToMap( MapStrDataObj& map, const std::string identifier, DataObject *obj );


public:

	/// For RMS simulation support.
	PowerFactoryRMS* rms_;

};

} // namespace pf_api

#endif // _PF_API_POWERFACTORY_H
