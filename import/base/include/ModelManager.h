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

#include "import/base/include/BareFMU.h"


class __FMI_DLL ModelManager
{

public:

	enum ModelDeleteStatus { ok, not_found, in_use, unknown };

	/// Destructor.        
	~ModelManager();

	/// Get singleton instance of model manager. 
	static ModelManager& getModelManager();

	/// Get model (from standard unzipped FMU). 
	static BareFMUModelExchangePtr getModel( const std::string& fmuPath,
					       const std::string& modelName,
					       const fmiBoolean loggingOn );

	/// Get model (from non-standard 'modelName.xml' and 'modelName.dll').  
	static BareFMUModelExchangePtr getModel( const std::string& xmlPath,
					       const std::string& dllPath,
					       const std::string& modelName,
					       const fmiBoolean loggingOn );

	/// Get slave (from standard unzipped FMU). 
	static BareFMUCoSimulationPtr getSlave( const std::string& fmuPath,
					      const std::string& modelName,
					      const fmiBoolean loggingOn );

	/// Get slave (from non-standard 'modelName.xml' and 'modelName.dll').  
	static BareFMUCoSimulationPtr getSlave( const std::string& xmlPath,
					      const std::string& dllPath,
					      const std::string& modelName,
					      const fmiBoolean loggingOn );

	/// Get instance (from standard unzipped FMU).
	static BareFMU2Ptr getInstance( const std::string& fmuPath,
				      const std::string& modelName,
				      const fmiBoolean loggingOn );

	/// Get instance (from non-standard 'modelName.xml' and 'modelName.dll').
	static BareFMU2Ptr getInstance( const std::string& xmlPath,
				      const std::string& dllPath,
				      const std::string& modelName,
				      const fmiBoolean loggingOn );

	/// Delete a model from the model manager. The model must not be in use.
	static ModelDeleteStatus deleteModel( const std::string& modelName );

private:

	/// Private constructor (singleton). 
	ModelManager() {}

	/// Helper function for loading a bare FMU shared library (FMI ME Version 1.0).
	static int loadDll( std::string dllPath, BareFMUModelExchangePtr bareFMU );

	/// Helper function for loading a bare FMU shared library (FMI CS Version 1.0).
	static int loadDll( std::string dllPath, BareFMUCoSimulationPtr bareFMU );

	/// Helper function for loading a bare FMU shared library (FMI Version 1.0, ME & CS).
	static int loadDll( std::string dllPath, BareFMU2Ptr bareFMU );
	
	/// Helper function for loading FMU shared library
	static void* getAdr( int* s, BareFMUModelExchangePtr bareFMU, const char* functionName );

	/// Helper function for loading FMU shared library
	static void* getAdr( int* s, BareFMUCoSimulationPtr bareFMU, const char* functionName );

	/// Helper function for loading FMU shared library
	static void* getAdr( int* s, BareFMU2Ptr bareFMU, const char* functionName );

#if defined(MINGW) || defined(_MSC_VER)
	/// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	static std::string getLastErrorAsString();
#endif

	/// Pointer to singleton instance. 
	static ModelManager* modelManager_;


	/// Define container for bare FMU ME collection. 
	typedef std::map<std::string, BareFMUModelExchangePtr > BareModelCollection;

	/// Collection of bare ME FMUs.
	BareModelCollection modelCollection_;

	/// Define container for bare FMU CS collection. 
	typedef std::map<std::string, BareFMUCoSimulationPtr > BareSlaveCollection;

	/// Collection of bare CS FMUs.
	BareSlaveCollection slaveCollection_;

	/// Define container for bare FMU 2 collection.
	typedef std::map<std::string, BareFMU2Ptr > BareInstanceCollection;

	/// Collection of bare 2.0 FMUs.
	BareInstanceCollection instanceCollection_;

};


#endif // _FMIPP_MODELMANAGER_H
