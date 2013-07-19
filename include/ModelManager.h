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

#include "fmi_me.h"


class ModelManager {

public:

        
	~ModelManager();  ///< Descrtructor.
	static ModelManager& getModelManager();  ///< Get singleton instance of model manager. 
	static FMU_functions* getModel( const std::string& fmuPath,
					const std::string& modelName ); ///< Get model (from standard unzipped FMU). 
	static FMU_functions* getModel( const std::string& xmlPath,
					const std::string& dllPath,
					const std::string& modelName ); ///< Get model (from non-standard 'modelName.xml' and 'modelName.dll').  

private:

	ModelManager() {}  ///< Private constructor (singleton). 
	static int loadDll( std::string dllPath, FMU_functions* fmuFun );               ///< Helper function for loading FMU shared library. 
	static void* getAdr( int* s, FMU_functions *fmuFun, const char* functionName ); ///< Helper function for loading FMU shared library 
	static ModelManager* modelManager_;                         ///< Pointer to singleton instance. 
	typedef std::map<std::string, FMU_functions*> Descriptions; ///< Define container for description collection. 
	Descriptions modelDescriptions_;                            ///< Collection of descriptions of FMUs.
};


#endif // _FMIPP_MODELMANAGER_H
