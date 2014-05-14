/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


/**
 * \file ModelManager.h
 * \class ModelManager provides the basic routines with which FMU models are managed 
 *                       acc. to a singlton pattern.  
 * 
 * An instance of ModelManager preserves basic info about the used FMUs, their paths, model names, provided 
 * dll libraries and the XML description file. The underlying DLLs are dynamically loaded and the XML files
 * get parsed. It provides a portable implementation regardless of the platform that has been used for generating 
 * the employed FMUs. An instance of model manager:
 * 1. is privately constructed and cannot be externally instantiated 
 * 2. However, an address of the instance can be accessed 
 * 3. provides fmi functions of any FMU given its fmuPath/modelName or xmlPath/dllPath/modelName,
 *    (in both cases the fmu has to be already unzipped and the unzipped materials should follow 
 *    the standard naming conventions?) 
 * 4. The basic information of any FMU is extracted only once. This is very adequate and time-saving in case 
 *    several instances of an FMU are used. 
 * 
 */ 

#ifndef _FMIPP_MODELMANAGER_H
#define _FMIPP_MODELMANAGER_H


#include <string>
#include <map>

#include "common/fmi_v1.0/fmi_me.h"
#include "common/fmi_v1.0/fmi_cs.h"

#include "import/base/include/ModelDescription.h"


/// Struct for "bare" FMU ME, i.e., pointers to loaded shared library functions and parsed xml model description.
struct BareFMUModelExchange {
	me::FMUModelExchange_functions* functions;
	ModelDescription* description;
};


/// Struct for "bare" FMU CS, i.e., pointers to loaded shared library functions and parsed xml model description.
struct BareFMUCoSimulation {
	cs::FMUCoSimulation_functions* functions;
	ModelDescription* description;
};


class __FMI_DLL ModelManager
{

public:

	/// Descrtructor.        
	~ModelManager();

	/// Get singleton instance of model manager. 
	static ModelManager& getModelManager();

	/// Get model (from standard unzipped FMU). 
	static BareFMUModelExchange* getModel( const std::string& fmuPath,
					       const std::string& modelName );

	/// Get model (from non-standard 'modelName.xml' and 'modelName.dll').  
	static BareFMUModelExchange* getModel( const std::string& xmlPath,
					       const std::string& dllPath,
					       const std::string& modelName );

	/// Get slave (from standard unzipped FMU). 
	static BareFMUCoSimulation* getSlave( const std::string& fmuPath,
					      const std::string& modelName );

	/// Get slave (from non-standard 'modelName.xml' and 'modelName.dll').  
	static BareFMUCoSimulation* getSlave( const std::string& xmlPath,
					      const std::string& dllPath,
					      const std::string& modelName );

private:

	/// Private constructor (singleton). 
	ModelManager() {}

	/// Helper function for loading ME FMU shared library. 
	static int loadDll( std::string dllPath, BareFMUModelExchange* bareFMU );

	/// Helper function for loading CS FMU shared library. 
	static int loadDll( std::string dllPath, BareFMUCoSimulation* bareFMU );

	/// Helper function for loading FMU shared library 
	static void* getAdr( int* s, BareFMUModelExchange* bareFMU, const char* functionName );

	/// Helper function for loading FMU shared library 
	static void* getAdr( int* s, BareFMUCoSimulation* bareFMU, const char* functionName );

	/// Helper function for transforming URIs to a system path.
	static std::string getPathFromUrl( const std::string& inputFileUrl );


	/// Pointer to singleton instance. 
	static ModelManager* modelManager_;


	/// Define container for bare FMU ME collection. 
	typedef std::map<std::string, BareFMUModelExchange*> BareModelCollection;

	/// Collection of bare ME FMUs.
	BareModelCollection modelCollection_;

	/// Define container for bare FMU CS collection. 
	typedef std::map<std::string, BareFMUCoSimulation*> BareSlaveCollection;

	/// Collection of bare CS FMUs.
	BareSlaveCollection slaveCollection_;

};


#endif // _FMIPP_MODELMANAGER_H
