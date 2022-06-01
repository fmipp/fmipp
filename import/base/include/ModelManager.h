// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------


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
	static LoadFMUStatus loadFMU( const std::string& modelIdentifier,
		const std::string& fmuDirUrl,
		const fmippBoolean loggingOn,
		FMUType& type );

	/**
	 * Loads an unzipped FMU into the model manager or reuses existing instances.
	 * 
	 * It is assumed that the FMU has been unzipped into a single directory and 
	 * that the unzipped content follows the standard naming conventions. The 
	 * model identifier will be automatically deduced from the model description.
	 * Hence, the model description always has to be parsed. Use ModelManager::
	 * loadFMU(const std::string&,const std::string&,const fmippBoolean,FMUType&)
	 * in case instantiation performance is an issue. In case the function 
	 * returns successfully, it will set the type and modelIdentifier variables
	 * to the appropriate values. The given model identifier may be used to 
	 * obtain appropriate bare FMUs and to unload the model. In case an FMU 
	 * specifies multiple model identifier, one model identifier will be 
	 * arbitrarily chosen and returned.
	 * @param[in] fmuDirUrl Path to the extracted FMU directory (given as URL).
	 * @param[in] loggingOn Input flag for turning logging on/off.
	 * @param[out] type Output flag with information about the FMU implementation
	 * (ME/CS, version 1.0/2.0).
	 * @param[out] modelIdentifier The output string which will contain one model
	 * identifier of the loaded FMU.
	 * @return status of the load process
	 */
	static LoadFMUStatus loadFMU( const std::string& fmuDirUrl, 
		const fmippBoolean loggingOn, FMUType& type, std::string& modelIdentifier );

	/**
	 * Unload an FMU from the model manager. It must not be in use. 
	 * @param[in] modelIdentifier model identifier associated to the "bare FMU" to be unloaded
	 * @return status of the unload process
	 */
	static UnloadFMUStatus unloadFMU( const std::string& modelIdentifier );

	/**
	 * Unloads all previously loaded FMUs.
	 * It is assumed that no FMU instance is in use anymore. It must be ensured
	 * that all objects which may use an FMU are already deleted. In case the 
	 * function returns successfully, all previously loaded FMUs were removed 
	 * from the ModelManager. In case a failure is returned, an arbitrary set of
	 * FMUs may reside in the ModelManager.
	 * The function may degrade performance as all DLLs and Model description 
	 * instances have to be parsed and loaded again. It is intended for testing 
	 * purpose and to update changed FMUs at runtime.
	 * @return The status of the operation. Ok, in case all cached FMUs could be
	 * removed.
	 */
	static UnloadFMUStatus unloadAllFMUs();

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
	 * Returns the type of the previously loaded model.
	 * In case the model was not loaded, an error will be returned and the type
	 * variable will not be touched. In case the type variable was loaded 
	 * successfully, success will be returned.
	 * @return The status of the operation
	 * @param[in] modelIdentifier The unique ID of the model to query
	 * @param[out] dest The destination to write the queried type or null. In
	 * case null is passed, it is just checked whether the entry already exists.
	 */
	static LoadFMUStatus getTypeOfLoadedFMU(const std::string& modelIdentifier,
		FMUType* dest);

	/**
	 * Get instance (FMI ME/CS 2.0). The corresponding FMU has to be loaded before.
	 * @return smart pointer to "bare" FMU
	 */
	static BareFMU2Ptr getInstance( const std::string& modelIdentifier );

private:

	/// Private constructor (singleton). 
	ModelManager() {}

	/**
	 * Instantiates the appropriate bare FMU and adds it to the internal 
	 * collections.
	 * It is assumed that the model description lists the given model identifier.
	 * Since FMI 2.0 allows a specification of one model identifier for CS and 
	 * one for ME, the model identifier cannot be automatically deduced from the 
	 * model description. Hence, it is passed as an argument. It is further 
	 * assumed that no collection already contains the bare FMU and its 
	 * associated model identifier.
	 * @return The status of the operation
	 * @param[in] description A unique pointer to a valid model description. The
	 * object will be consumed and ownership is transferred to the bare FMU.
	 * @param[in] fmuDirUrl The base URL of the FMU directory
	 * @param[in] modelIdentifier Specifies the model to load from the given FMU.
	 */
	static LoadFMUStatus loadBareFMU(
		std::unique_ptr<ModelDescription> description, 
		const std::string& fmuDirUrl, const std::string& modelIdentifier);

	/// Helper function for loading a bare FMU shared library (FMI ME Version 1.0).
	static int loadDll( std::string dllPath, BareFMUModelExchangePtr bareFMU );

	/// Helper function for loading a bare FMU shared library (FMI CS Version 1.0).
	static int loadDll( std::string dllPath, BareFMUCoSimulationPtr bareFMU );

	/// Helper function for loading a bare FMU shared library (FMI Version 1.0, ME & CS).
	static int loadDll( std::string dllPath, BareFMU2Ptr bareFMU );

	/**
	 * @brief Loads all function pointers which are common to ME and CS
	 * @details The helper function assumes that the dll handler as well as the 
	 * model description are valid. All common functions will be stored in the 
	 * given bareFMU.
	 * @param bareFMU The pointer to the partially populated FMu structure
	 * @return The status of the operation. 1 on success.
	 */
	static int loadCommonFMI20Functions( BareFMU2Ptr bareFMU );

	/**
	 * @brief Deletes all function pointer which are either specific to ME 2.0 or 
	 * CS 2.0
	 * @details All function pointers will be set to a trap function which aids
	 * debugging. All other functions will not be altered.
	 * @param bareFMU A valid pointer to the destination structure
	 */
	static void deleteFMI20MEandCSSpecificFunctions( BareFMU2Ptr bareFMU );

	/**
	 * @brief Loads all functions which are specific to ME 2.0
	 * @details The helper function assumes that the dll handler as well as the 
	 * model description are valid. All common functions will be stored in the 
	 * given bareFMU.
	 * @param bareFMU The pointer to the partially populated FMu structure
	 * @return The status of the operation. 1 on success.
	 */
	static int loadFMI20MESpecificFunctions( BareFMU2Ptr bareFMU );

	/**
	 * @brief Loads all functions which are specific to CS 2.0
	 * @details The helper function assumes that the dll handler as well as the 
	 * model description are valid. All common functions will be stored in the 
	 * given bareFMU.
	 * @param bareFMU The pointer to the partially populated FMu structure
	 * @return The status of the operation. 1 on success.
	 */
	static int loadFMI20CSSpecificFunctions( BareFMU2Ptr bareFMU );


	/**
	 * @brief Tries to open the DLL/SO file and returns the handler.
	 * @details In case the file cannot be opened, the status variable is set to
	 * 0 and an arbitrary value is returned.
	 * @param status A valid reference to the status variable
	 * @param dllPath The path to the dll file
	 */
	static HANDLE openDLL( int* status, const std::string& dllPath );

	/** 
	 * @brief Helper function for loading FMU 1.0 shared library function
	 * @details The function will load the address of the given function from the
	 * previously loaded DLL. The DLL as well as the model description need to be
	 * available via the given bare FMU. The function itself does not need to be
	 * present. It is assumed that all arguments point to valid instances.
	 * @param FunctionPtrType The pointer to the function type which should be 
	 * returned.
	 * @param BareFMUPtrType The type of the bare FMU pointer to use. It is 
	 * assumed that the given type follows the pointer access convention (E.g. a 
	 * C++ or shared_ptr) and that the destination follows the BareFMU 
	 * convention.
	 * @param functionName The name of the function without any instance prefix
	 * @param s The destination of the status flag. In case the function fails, 
	 * 0 will be written to the given location.
	 * @return The loaded function address.
	 */
	template<typename FunctionPtrType, typename BareFMUPtrType>
	static FunctionPtrType getAdr10( int* s, BareFMUPtrType bareFMU, 
		const char* functionName );

	/** 
	 * @brief Helper function for loading FMU 2.0 shared library function
	 * @details The function will load the address of the given function from the
	 * previously loaded DLL. The DLL as well as the model description need to be
	 * available via the given bare FMU. The function itself does not need to be
	 * present. It is assumed that all arguments point to valid instances.
	 * @param FunctionPtrType The pointer to the function type which should be 
	 * returned.
	 * @param functionName The name of the function without any instance prefix
	 * @param s The destination of the status flag. In case the function fails, 
	 * 0 will be written to the given location.
	 * @return The loaded function address.
	 */
	template<typename FunctionPtrType>
	static FunctionPtrType getAdr20( int* s, BareFMU2Ptr bareFMU, 
		const char* functionName );

	/**
	 * @brief Loads the function address without any function name resolution
	 * @param FunctionPtrType The destination type of the function.
	 * @param s The status variable which will be set to 0 in case an error 
	 * occurred.
	 * @param dllHandle The valid DLL handle which will be used to load the 
	 * function address
	 * @param rawFunctionName The name of the function in the referenced DLL
	 * @return The casted pointer to the function.
	 */
	template<typename FunctionPtrType>
	static FunctionPtrType getAdrRaw(int* s, HANDLE dllHandle,
		const char* rawFunctionName);

#if defined(MINGW) || defined(_MSC_VER)
	/// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	static std::string getLastErrorAsString();
#endif

	/**
	 * Tries to newly instantiate the model description file.
	 * The destination pointer will be set to the model description pointer. In 
	 * case the description cannot be loaded successfully, dest may contain 
	 * arbitrary results.
	 * @param[in] fmuDirUrl The URL of the FMU directory location. The parameter 
	 * will be used to generate the location of the model description file.
	 * @param[out] The pointer which will be set to the instantiated model 
	 * description instance.
	 * \return The status of the operation.
	 */
	static LoadFMUStatus loadModelDescription(const std::string& fmuDirUrl, 
		std::unique_ptr<ModelDescription>& dest);

	/**
	 * Removes the first instance of modelIdentifer from the given fmuCollection
	 * In case the modelIdentifier is not found or if the model is still in use,
	 * the appropriate status code will be returned.
	 */
	template<typename BareFMUType>
	static UnloadFMUStatus unloadFMU( const std::string& modelIdentifier, 
		std::map<std::string, BareFMUType> &fmuCollection );

	/**
	 * Tries to unload all FMUs in the given collection
	 * In case an error is detected, some FMUs may remain in the given 
	 * collection. After successfully executing the function, the collection will
	 * be empty.
	 */
	template<typename BareFMUType>
	static UnloadFMUStatus unloadAllFMUs( 
		std::map<std::string, BareFMUType> &fmuCollection );

	/** 
	 * @brief Function which should never be called
	 * @details The function will assert that it is never called. In case 
	 * debugging is turned off, an error message may be printed and an error 
	 * status is returned.
	 */
	static fmi2Status fmi2DoNotCall(...);

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
