/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


/**
 * \file ModelManager.h
 * \class ModelManager provides the basic routines with which FMU models are managed.
 * 
 * An instance of ModelManager preserves basic info about the used FMUs, their paths, model names, provided 
 * shared libraries (DLL) and the XML description file. The underlying DLLs are dynamically loaded and the XML files
 * get parsed. It provides a portable implementation regardless of the platform that has been used for generating 
 * the employed FMUs. An instance of model manager:
 * 1. is privately constructed and cannot be externally instantiated 
 * 2. provides FMI functions of any FMU given (FMUs have to be already unzipped)
 * 3. loads FMUs only once, which is very time-saving in case several instances of the same FMU are used
 */ 

#ifndef _FMIPP_MODELMANAGER_H
#define _FMIPP_MODELMANAGER_H

#include <string>
#include <map>

#include "common/FMUType.h"
#include "import/base/include/BareFMU.h"


class __FMI_DLL ModelManager
{

public:

	enum LoadFMUStatus { 
		success = 0, ///< The FMU been successfully loaded into the model manager.
		duplicate, ///< The FMU has not been loaded, because an FMU with this name has already been loaded. Unload the existing FMU first.
		shared_lib_invalid_uri, ///< The FMU has not been loaded, because URI provided for the shared library is invalid.
		shared_lib_load_failed, ///< The FMU has not been loaded, because the shared library (DLL) could not be successfully loaded.
		description_invalid_uri, ///< The FMU has not been loaded, because URI provided for the XML model description is invalid.
		description_invalid, ///< The FMU has not been loaded, because the XML model description is invalid.
		identifier_invalid, ///< The FMU has not been loaded, because the specified model identifier is not consistent with the information found in the XML model description.
		failed ///< Unknown error.
	};

	enum UnloadFMUStatus { 
		ok = 0, ///< The FMU has been successfully unloaded from the model manager.
		not_found, ///< The specified FMU has not been found.
		in_use, ///< The FMU cannot be unloaded, because it is still in use.
		unknown ///< Unknown error.
	};

	/// Destructor. 
	~ModelManager();

	/// Get singleton instance of model manager. 
	static ModelManager& getModelManager();

	/**
	 * Load an unzipped FMU into the model manager. It is assumed that the FMU has been unzipped into
	 * a single directory and that the unzipped content follows the standard naming conventions.
	 * @param[in] modelIdentifier FMI model identifier (according to XML model description)
	 * @param[in] fmuDirUrl Path to the extracted FMU directory (given as URL).
	 * @param[in] loggingOn Input flag for turning logging on/off.
	 * @param[out] type Output flag with information about the FMU implementation (ME/CS, version 1.0/2.0).
	 * @return status of the load process
	 */
	static LoadFMUStatus loadFMU( const std::string modelIdentifier,
		const std::string& fmuDirUrl,
		const fmiBoolean loggingOn,
		FMUType& type );

	/**
	 * Unload an FMU from the model manager. It must not be in use. 
	 * @param[in] modelIdentifier model identifier associated to the "bare FMU" to be unloaded
	 * @return status of the unload process
	 */
	static UnloadFMUStatus unloadFMU( const std::string& modelIdentifier );

	/**
	 * Get model (FMI ME 1.0). The corresponding FMU has to be loaded before.
	 * @return smart pointer to "bare" FMU
	 */ 
	static BareFMUModelExchangePtr getModel( const std::string& modelIdentifier );

	/**
	 * Get slave (FMI CS 1.0). The corresponding FMU has to be loaded before.
	 * @return smart pointer to "bare" FMU
	 */
	static BareFMUCoSimulationPtr getSlave( const std::string& modelIdentifier );

	/**
	 * Get instance (FMI ME/CS 2.0). The corresponding FMU has to be loaded before.
	 * @return smart pointer to "bare" FMU
	 */
	static BareFMU2Ptr getInstance( const std::string& modelIdentifier );

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
