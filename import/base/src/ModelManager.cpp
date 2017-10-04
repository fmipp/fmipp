/* ---------------------------------------------------------------------------
 * Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 *
 * Except functions 'loadDll' and 'getAdr', adapted from FmuSdk:
 *
 * Copyright 2010 QTronic GmbH. All rights reserved.
 *
 * The FmuSdk is licensed by the copyright holder under the BSD License
 * (http://www.opensource.org/licenses/bsd-license.html):
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY QTRONIC GMBH "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL QTRONIC GMBH BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ---------------------------------------------------------------------------*/

/**
 * \file ModelManager.cpp 
 * 
 */ 

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#define BUFSIZE 4096

#if defined( WIN32 ) // Windows
#define _WIN32_WINNT 0x0502 // necessary for function SetDllDirectory in Windows
#include <windows.h>
#include "shlwapi.h" // necessary for function PathRemoveFileSpec
#endif

#include <algorithm>
#include <cassert>
#include <utility>

#include "import/base/include/ModelManager.h"
#include "import/base/include/ModelDescription.h"
#include "import/base/include/PathFromUrl.h"

using namespace std;


ModelManager* ModelManager::modelManager_ = 0;


ModelManager::~ModelManager()
{
	// No clean-up required:
	//  - bare FMUs have their own destructors.
	//  - destructors of bare FMUs will be called (from shared_ptr) when
	//    the destructors of the maps they are contained in are called
}


// Retrieve a reference to the unique ModelManager instance.
ModelManager& ModelManager::getModelManager()
{
	// Singleton instance
	static ModelManager modelManagerInstance;
	if ( 0 == modelManager_ ) {
		modelManager_ = &modelManagerInstance;
	}
	return *modelManager_;
}

// Load an unzipped FMU into the model manager. It is assumed that the FMU has been unzipped into
// a single directory and that the unzipped content follows the standard naming conventions.
ModelManager::LoadFMUStatus
ModelManager::loadFMU( const std::string& modelIdentifier,
	const std::string& fmuDirUrl,
	const fmiBoolean loggingOn,
	FMUType& type )
{
	if ( 0 == modelManager_ ) getModelManager();

	type = invalid;

	//	
	// Check if FMU has already been loaded.
	//

	LoadFMUStatus status = getTypeOfLoadedFMU( modelIdentifier, &type );
	if ( success == status ) return duplicate;

	//
	// Load new FMU.
	// 

	// Parse XML model description.
	std::unique_ptr<ModelDescription> description;
	status = loadModelDescription( fmuDirUrl, description );
	if ( success != status ) return status;

	// Sanity check for model identifier.
	if ( !description->hasModelIdentifier( modelIdentifier ) ) {
		return identifier_invalid;
	}

	// The type of the FMU is determined by the model description.
	type = description->getFMUType();

	// Load DLLs and BareFMU
	status = loadBareFMU(std::move(description), fmuDirUrl, modelIdentifier);
	return status;
}	

ModelManager::LoadFMUStatus
ModelManager::loadFMU(const std::string& fmuDirUrl,
	const fmiBoolean loggingOn, FMUType& type, std::string& modelIdentifier)
{
	if ( 0 == modelManager_ ) getModelManager();

	// Parse XML model description.
	std::unique_ptr<ModelDescription> description;
	LoadFMUStatus status = loadModelDescription( fmuDirUrl, description );
	if ( success != status ) return status;

	// Always take the first model identifier
	assert(description->getModelIdentifier().size() > 0);
	modelIdentifier = description->getModelIdentifier()[0];
	type = description->getFMUType();

	// Check whether the model was previously loaded
	FMUType refType = type;
	status = getTypeOfLoadedFMU( modelIdentifier, &refType );
	assert(type == refType); // Assume consistency with description
	if ( status == success ) return duplicate;

	// Load DLLs and BareFMU
	status = loadBareFMU(std::move(description), fmuDirUrl, modelIdentifier);
	return status;
}

// Unload an FMU from the model manager. It must not be in use. 
ModelManager::UnloadFMUStatus
ModelManager::unloadFMU( const std::string& modelIdentifier )
{
	if ( 0 == modelManager_ ) getModelManager();

	ModelManager::UnloadFMUStatus status;

	status = unloadFMU( modelIdentifier, modelManager_->modelCollection_ );
	if ( ModelManager::not_found != status ) return status;

	status = unloadFMU( modelIdentifier, modelManager_->slaveCollection_ );
	if ( ModelManager::not_found != status ) return status;

	status = unloadFMU( modelIdentifier, modelManager_->instanceCollection_ );
	return status;
}

// Removes all FMU instances
ModelManager::UnloadFMUStatus
ModelManager::unloadAllFMUs()
{
	if ( 0 == modelManager_ ) getModelManager();

	UnloadFMUStatus status = unloadAllFMUs(modelManager_->modelCollection_);
	if ( ok != status ) return status;

	status = unloadAllFMUs(modelManager_->slaveCollection_);
	if ( ok != status ) return status;

	status = unloadAllFMUs(modelManager_->instanceCollection_);
	return status;
}

// Get model (FMI ME 1.0).
BareFMUModelExchangePtr
ModelManager::getModel( const std::string& modelIdentifier )
{
	if ( 0 == modelManager_ ) getModelManager();

	BareModelCollection::iterator itFind = modelManager_->modelCollection_.find( modelIdentifier );
	if ( itFind != modelManager_->modelCollection_.end() ) { // Model identifier found in list.
		return itFind->second;
	}
	
	return BareFMUModelExchangePtr();
}


// Get slave (FMI CS 1.0).
BareFMUCoSimulationPtr
ModelManager::getSlave( const std::string& modelIdentifier )
{
	if ( 0 == modelManager_ ) getModelManager();

	BareSlaveCollection::iterator itFind = modelManager_->slaveCollection_.find( modelIdentifier );
	if ( itFind != modelManager_->slaveCollection_.end() ) { // Model identifier found in list.
		return itFind->second;
	}
	
	return BareFMUCoSimulationPtr();
}


// Get instance (FMI ME/CS 2.0).
BareFMU2Ptr
ModelManager::getInstance( const std::string& modelIdentifier )
{
	if ( 0 == modelManager_ ) getModelManager();

	BareInstanceCollection::iterator itFind = modelManager_->instanceCollection_.find( modelIdentifier );
	if ( itFind != modelManager_->instanceCollection_.end() ) { // Model identifier found in list.
		return itFind->second;
	}

	return BareFMU2Ptr();
}

ModelManager::LoadFMUStatus
ModelManager::getTypeOfLoadedFMU( const std::string& modelIdentifier, 
	FMUType* dest )
{
	if ( 0 == modelManager_ ) getModelManager();

	// Write the result locally, in case it is not needed
	FMUType dummyDest;
	if (!dest) dest = &dummyDest;
	
	BareModelCollection::iterator itFindModel = modelManager_->modelCollection_.find( modelIdentifier );
	if ( itFindModel != modelManager_->modelCollection_.end() ) { // Model identifier found in list of descriptions.
		*dest = itFindModel->second->description->getFMUType();
		return success;
	}

	BareSlaveCollection::iterator itFindSlave = modelManager_->slaveCollection_.find( modelIdentifier );
	if ( itFindSlave != modelManager_->slaveCollection_.end() ) { // Model identifier found in list of descriptions.
		*dest = itFindSlave->second->description->getFMUType();
		return success;
	}

	BareInstanceCollection::iterator itFindInstance = modelManager_->instanceCollection_.find( modelIdentifier );
	if ( itFindInstance != modelManager_->instanceCollection_.end() ) { // Model identifier found in list of descriptions.	
		*dest = itFindInstance->second->description->getFMUType();
		return success;
	}
	return failed;
}

ModelManager::LoadFMUStatus 
ModelManager::loadBareFMU(
	std::unique_ptr<ModelDescription> description,
	const std::string& fmuDirUrl, const std::string& modelIdentifier)
{
	assert( (bool) description );
	assert( description->hasModelIdentifier(modelIdentifier) );
	assert(modelManager_);
	assert( getTypeOfLoadedFMU( modelIdentifier, NULL ) != success );

	// Path to shared library (OS specific).
	string dllPath;
	string dllUrl = fmuDirUrl + "/binaries/" + FMU_BIN_DIR + "/" + modelIdentifier + FMU_BIN_EXT;
	if ( false == PathFromUrl::getPathFromUrl( dllUrl, dllPath ) ) return shared_lib_invalid_uri;

	// The type of the FMU is determined by the model description.
	FMUType type = description->getFMUType();
	if ( fmi_1_0_me == type ) // FMI ME 1.0
	{
		BareFMUModelExchangePtr bareFMU = make_shared<BareFMUModelExchange>();
		bareFMU->description = description.release();

		// Loading the DLL may fail. In this case do not add it to list of models.
		if ( 0 == loadDll( dllPath, bareFMU ) ) return shared_lib_load_failed;
		
		// Add bare FMU to list.
		modelManager_->modelCollection_[modelIdentifier] = bareFMU;

		return success;
	}
	else if ( fmi_1_0_cs == type ) // FMI CS 1.0
	{
		BareFMUCoSimulationPtr bareFMU = make_shared<BareFMUCoSimulation>();
		bareFMU->description = description.release();

		bareFMU->fmuLocation = fmuDirUrl;

		//Loading the DLL may fail. In this case do not add it to list of slaves.
		if ( 0 == loadDll( dllPath, bareFMU ) ) return shared_lib_load_failed;

		// Add bare FMU to list.
		modelManager_->slaveCollection_[modelIdentifier] = bareFMU;

		return success;
	}
	else if ( ( fmi_2_0_me == type ) || ( fmi_2_0_cs == type ) || ( fmi_2_0_me_and_cs == type ) )
	{
		BareFMU2Ptr bareFMU = make_shared<BareFMU2>();
		bareFMU->description = description.release();

		bareFMU->fmuResourceLocation = fmuDirUrl + "/resources";

		// Loading the DLL may Fail. In this case do not add it to list of instances.
		// Bare FMU desctructor should take care of freeing memory.
		if ( 0 == loadDll( dllPath, bareFMU ) ) return shared_lib_load_failed;

		// Add bare FMU to list.
		modelManager_->instanceCollection_[modelIdentifier] = bareFMU;

		return success;
	}
	return failed;
}


// Helper function for loading a bare FMU shared library (FMI ME Version 1.0).
int ModelManager::loadDll( string dllPath, BareFMUModelExchangePtr bareFMU )
{
	using namespace me;

	int s = 1;

#if defined(MINGW) || defined(_MSC_VER)

	//sets search directory for dlls to bin directory of FMU
	//including workaround to get dll directory out of dll path
	char *bufferPath = new char[dllPath.length() + 1];
	strcpy( bufferPath, dllPath.c_str() );
	PathRemoveFileSpec( bufferPath );
	SetDllDirectory( bufferPath );
	HANDLE h = LoadLibrary( dllPath.c_str() );
	delete [] bufferPath;
	if ( !h ) {
		string error = getLastErrorAsString();
		printf( "ERROR: Could not load \"%s\" (%s)\n", dllPath.c_str(), error.c_str() ); fflush(stdout);
		return 0; // failure
	}

#else

	HANDLE h = dlopen( dllPath.c_str(), RTLD_LAZY );

	if ( !h ) {
		printf( "ERROR: Could not load \"%s\":\n%s\n", dllPath.c_str(), dlerror() ); fflush(stdout);
		return 0; // failure
	}
#endif

	FMUModelExchange_functions* fmuFun = new FMUModelExchange_functions;
	bareFMU->functions = fmuFun;

	fmuFun->dllHandle = h;

	// FMI for Model Exchange 1.0
	fmuFun->getModelTypesPlatform =
		reinterpret_cast<fGetModelTypesPlatform>( getAdr( &s, bareFMU, "fmiGetModelTypesPlatform" ) );
	fmuFun->instantiateModel =
		reinterpret_cast<fInstantiateModel>( getAdr( &s, bareFMU, "fmiInstantiateModel" ) );
	fmuFun->freeModelInstance =
		reinterpret_cast<fFreeModelInstance>( getAdr( &s, bareFMU, "fmiFreeModelInstance" ) );
	fmuFun->completedIntegratorStep =
		reinterpret_cast<fCompletedIntegratorStep>( getAdr( &s, bareFMU, "fmiCompletedIntegratorStep" ) );
	fmuFun->initialize =
		reinterpret_cast<fInitialize>( getAdr( &s, bareFMU, "fmiInitialize" ) );
	fmuFun->eventUpdate =
		reinterpret_cast<fEventUpdate>( getAdr( &s, bareFMU, "fmiEventUpdate" ) );
	fmuFun->getStateValueReferences =
		reinterpret_cast<fGetStateValueReferences>( getAdr( &s, bareFMU, "fmiGetStateValueReferences" ) );
	fmuFun->terminate =
		reinterpret_cast<fTerminate>( getAdr( &s, bareFMU, "fmiTerminate" ) );
	fmuFun->getVersion =
		reinterpret_cast<fGetVersion>( getAdr( &s, bareFMU, "fmiGetVersion" ) );
	fmuFun->setDebugLogging =
		reinterpret_cast<fSetDebugLogging>( getAdr( &s, bareFMU, "fmiSetDebugLogging" ) );
	fmuFun->setTime =
		reinterpret_cast<fSetTime>( getAdr( &s, bareFMU, "fmiSetTime" ) );
	fmuFun->setContinuousStates =
		reinterpret_cast<fSetContinuousStates>( getAdr( &s, bareFMU, "fmiSetContinuousStates" ) );
	fmuFun->setReal =
		reinterpret_cast<fSetReal>( getAdr( &s, bareFMU, "fmiSetReal" ) );
	fmuFun->setInteger =
		reinterpret_cast<fSetInteger>( getAdr( &s, bareFMU, "fmiSetInteger" ) );
	fmuFun->setBoolean =
		reinterpret_cast<fSetBoolean>( getAdr( &s, bareFMU, "fmiSetBoolean" ) );
	fmuFun->setString =
		reinterpret_cast<fSetString>( getAdr( &s, bareFMU, "fmiSetString" ) );
	fmuFun->getReal =
		reinterpret_cast<fGetReal>( getAdr( &s, bareFMU, "fmiGetReal" ) );
	fmuFun->getInteger =
		reinterpret_cast<fGetInteger>( getAdr( &s, bareFMU, "fmiGetInteger" ) );
	fmuFun->getBoolean =
		reinterpret_cast<fGetBoolean>( getAdr( &s, bareFMU, "fmiGetBoolean" ) );
	fmuFun->getString =
		reinterpret_cast<fGetString>( getAdr( &s, bareFMU, "fmiGetString" ) );
	fmuFun->getDerivatives =
		reinterpret_cast<fGetDerivatives>( getAdr( &s, bareFMU, "fmiGetDerivatives" ) );
	fmuFun->getEventIndicators =
		reinterpret_cast<fGetEventIndicators>( getAdr( &s, bareFMU, "fmiGetEventIndicators" ) );
	fmuFun->getContinuousStates =
		reinterpret_cast<fGetContinuousStates>( getAdr( &s, bareFMU, "fmiGetContinuousStates" ) );
	fmuFun->getNominalContinuousStates =
		reinterpret_cast<fGetNominalContinuousStates>( getAdr( &s, bareFMU, "fmiGetNominalContinuousStates" ) );

	return s;
}


// Helper function for loading a bare FMU shared library (FMI CS Version 1.0).
int ModelManager::loadDll( string dllPath, BareFMUCoSimulationPtr bareFMU )
{
	using namespace cs;

	int s = 1;

#if defined(MINGW) || defined(_MSC_VER)

	//sets search directory for dlls to bin directory of FMU
	//including workaround to get dll directory out of dll path
	char *bufferPath = new char[dllPath.length() + 1];
	strcpy( bufferPath, dllPath.c_str() );
	PathRemoveFileSpec( bufferPath );
	SetDllDirectory( bufferPath );
	HANDLE h = LoadLibrary( dllPath.c_str() );
	delete [] bufferPath;
	if ( !h ) {
		string error = getLastErrorAsString();
		printf( "ERROR: Could not load \"%s\" (%s)\n", dllPath.c_str(), error.c_str() ); fflush(stdout);
		return 0; // failure

		}

#else

	HANDLE h = dlopen( dllPath.c_str(), RTLD_LAZY );

	if ( !h ) {
		printf( "ERROR: Could not load \"%s\":\n%s\n", dllPath.c_str(), dlerror() ); fflush(stdout);
		return 0; // failure
	}
#endif

	FMUCoSimulation_functions* fmuFun = new FMUCoSimulation_functions;
	bareFMU->functions = fmuFun;

	fmuFun->dllHandle = h;

	fmuFun->getTypesPlatform        = (fGetTypesPlatform)   getAdr( &s, bareFMU, "fmiGetTypesPlatform" );
	if ( s == 0 ) {
		s = 1; // work around bug for FMUs exported using Dymola 2012 and SimulationX 3.x
		fmuFun->getTypesPlatform    = (fGetTypesPlatform)   getAdr( &s, bareFMU, "fmiGetModelTypesPlatform" );
		if ( s == 1 ) { printf( "  using fmiGetModelTypesPlatform instead\n" ); fflush( stdout ); }
	}


	fmuFun->instantiateSlave =
		reinterpret_cast<fInstantiateSlave>( getAdr( &s, bareFMU, "fmiInstantiateSlave" ) );
	fmuFun->initializeSlave=
		reinterpret_cast<fInitializeSlave>( getAdr( &s, bareFMU, "fmiInitializeSlave" ) );
	fmuFun->terminateSlave =
		reinterpret_cast<fTerminateSlave>( getAdr( &s, bareFMU, "fmiTerminateSlave" ) );
	fmuFun->resetSlave =
		reinterpret_cast<fResetSlave>( getAdr( &s, bareFMU, "fmiResetSlave" ) );
	fmuFun->freeSlaveInstance=
		reinterpret_cast<fFreeSlaveInstance>( getAdr( &s, bareFMU, "fmiFreeSlaveInstance" ) );
	fmuFun->cancelStep =
		reinterpret_cast<fCancelStep>( getAdr( &s, bareFMU, "fmiCancelStep" ) );
	fmuFun->doStep =
		reinterpret_cast<fDoStep>( getAdr( &s, bareFMU, "fmiDoStep" ) );
	fmuFun->getStatus=
		reinterpret_cast<fGetStatus>( getAdr( &s, bareFMU, "fmiGetStatus" ) );
	fmuFun->getRealStatus=
		reinterpret_cast<fGetRealStatus>( getAdr( &s, bareFMU, "fmiGetRealStatus" ) );
	fmuFun->getIntegerStatus =
		reinterpret_cast<fGetIntegerStatus>( getAdr( &s, bareFMU, "fmiGetIntegerStatus" ) );
	fmuFun->getBooleanStatus =
		reinterpret_cast<fGetBooleanStatus>( getAdr( &s, bareFMU, "fmiGetBooleanStatus" ) );
	fmuFun->getStringStatus=
		reinterpret_cast<fGetStringStatus>( getAdr( &s, bareFMU, "fmiGetStringStatus" ) );
	fmuFun->getVersion =
		reinterpret_cast<fGetVersion>( getAdr( &s, bareFMU, "fmiGetVersion" ) );
	fmuFun->setDebugLogging=
		reinterpret_cast<fSetDebugLogging>( getAdr( &s, bareFMU, "fmiSetDebugLogging" ) );
	fmuFun->setReal=
		reinterpret_cast<fSetReal>( getAdr( &s, bareFMU, "fmiSetReal" ) );
	fmuFun->setInteger =
		reinterpret_cast<fSetInteger>( getAdr( &s, bareFMU, "fmiSetInteger" ) );
	fmuFun->setBoolean =
		reinterpret_cast<fSetBoolean>( getAdr( &s, bareFMU, "fmiSetBoolean" ) );
	fmuFun->setString=
		reinterpret_cast<fSetString>( getAdr( &s, bareFMU, "fmiSetString" ) );
	fmuFun->setRealInputDerivatives=
		reinterpret_cast<fSetRealInputDerivatives>( getAdr( &s, bareFMU, "fmiSetRealInputDerivatives" ) );
	fmuFun->getReal=
		reinterpret_cast<fGetReal>( getAdr( &s, bareFMU, "fmiGetReal" ) );
	fmuFun->getInteger =
		reinterpret_cast<fGetInteger>( getAdr( &s, bareFMU, "fmiGetInteger" ) );
	fmuFun->getBoolean =
		reinterpret_cast<fGetBoolean>( getAdr( &s, bareFMU, "fmiGetBoolean" ) );
	fmuFun->getString=
		reinterpret_cast<fGetString>( getAdr( &s, bareFMU, "fmiGetString" ) );
	fmuFun->getRealOutputDerivatives=
		reinterpret_cast<fGetRealOutputDerivatives>( getAdr( &s, bareFMU, "fmiGetRealOutputDerivatives" ) );

	return s;
}

// Helper function for loading a bare FMU shared library (FMI ME/CS Version 2.0).
int ModelManager::loadDll( string dllPath, BareFMU2Ptr bareFMU )
{
	using namespace fmi2;

	int s = 1;

#if defined(MINGW) || defined(_MSC_VER)

	// sets search directory for dlls to bin directory of FMU
	// including workaround to get dll directory out of dll path
	char *bufferPath = new char[dllPath.length() + 1];
	strcpy( bufferPath, dllPath.c_str() );
	PathRemoveFileSpec( bufferPath );
	SetDllDirectory( bufferPath );
	HANDLE h = LoadLibrary( dllPath.c_str() );
	delete [] bufferPath;
	if ( !h ) {
		string error = getLastErrorAsString();
		printf( "ERROR: Could not load \"%s\" (%s)\n", dllPath.c_str(), error.c_str() ); fflush(stdout);
		return 0; // failure
	}

#else

	HANDLE h = dlopen( dllPath.c_str(), RTLD_LAZY );

	if ( !h ) {
		printf( "ERROR: Could not load \"%s\":\n%s\n", dllPath.c_str(), dlerror() ); fflush(stdout);
		return 0; // failure
	}

#endif

	FMU2_functions* fmuFun = new FMU2_functions;
	bareFMU->functions = fmuFun;

	fmuFun->dllHandle = h;

	// FMI for Model Exchange 2.0
	fmuFun->getTypesPlatform=
		reinterpret_cast<fmi2GetTypesPlatformTYPE>( getAdr( &s, bareFMU, "fmi2GetTypesPlatform" ) );
	fmuFun->getVersion=
		reinterpret_cast<fmi2GetVersionTYPE>( getAdr( &s, bareFMU, "fmi2GetVersion" ) );
	fmuFun->setDebugLogging=
		reinterpret_cast<fmi2SetDebugLoggingTYPE>( getAdr( &s, bareFMU, "fmi2SetDebugLogging" ) );
	fmuFun->instantiate=
		reinterpret_cast<fmi2InstantiateTYPE>( getAdr( &s, bareFMU, "fmi2Instantiate" ) );
	fmuFun->freeInstance=
		reinterpret_cast<fmi2FreeInstanceTYPE>( getAdr( &s, bareFMU, "fmi2FreeInstance" ) );

	fmuFun->setupExperiment=
		reinterpret_cast<fmi2SetupExperimentTYPE>( getAdr( &s, bareFMU, "fmi2SetupExperiment" ) );
	fmuFun->enterInitializationMode=
		reinterpret_cast<fmi2EnterInitializationModeTYPE>( getAdr( &s, bareFMU, "fmi2EnterInitializationMode" ) );
	fmuFun->exitInitializationMode=
		reinterpret_cast<fmi2ExitInitializationModeTYPE>( getAdr( &s, bareFMU, "fmi2ExitInitializationMode" ) );

	fmuFun->terminate=
		reinterpret_cast<fmi2TerminateTYPE>( getAdr( &s, bareFMU, "fmi2Terminate" ) );
	fmuFun->reset=
		reinterpret_cast<fmi2ResetTYPE>( getAdr( &s, bareFMU, "fmi2Reset" ) );

	fmuFun->getReal=
		reinterpret_cast<fmi2GetRealTYPE>( getAdr( &s, bareFMU, "fmi2GetReal" ) );
	fmuFun->getInteger=
		reinterpret_cast<fmi2GetIntegerTYPE>( getAdr( &s, bareFMU, "fmi2GetInteger" ) );
	fmuFun->getBoolean=
		reinterpret_cast<fmi2GetBooleanTYPE>( getAdr( &s, bareFMU, "fmi2GetBoolean" ) );
	fmuFun->getString=
		reinterpret_cast<fmi2GetStringTYPE>( getAdr( &s, bareFMU, "fmi2GetString" ) );

	fmuFun->setReal=
		reinterpret_cast<fmi2SetRealTYPE>( getAdr( &s, bareFMU, "fmi2SetReal" ) );
	fmuFun->setInteger=
		reinterpret_cast<fmi2SetIntegerTYPE>( getAdr( &s, bareFMU, "fmi2SetInteger" ) );
	fmuFun->setBoolean=
		reinterpret_cast<fmi2SetBooleanTYPE>( getAdr( &s, bareFMU, "fmi2SetBoolean" ) );
	fmuFun->setString=
		reinterpret_cast<fmi2SetStringTYPE>( getAdr( &s, bareFMU, "fmi2SetString" ) );

	fmuFun->getFMUstate=
		reinterpret_cast<fmi2GetFMUstateTYPE>( getAdr( &s, bareFMU, "fmi2GetFMUstate" ) );
	fmuFun->setFMUstate=
		reinterpret_cast<fmi2SetFMUstateTYPE>( getAdr( &s, bareFMU, "fmi2SetFMUstate" ) );
	fmuFun->freeFMUstate=
		reinterpret_cast<fmi2FreeFMUstateTYPE>( getAdr( &s, bareFMU, "fmi2FreeFMUstate" ) );
	fmuFun->serializedFMUstateSize=
		reinterpret_cast<fmi2SerializedFMUstateSizeTYPE>( getAdr( &s, bareFMU, "fmi2SerializedFMUstateSize" ) );
	fmuFun->serializeFMUstate=
		reinterpret_cast<fmi2SerializeFMUstateTYPE>( getAdr( &s, bareFMU, "fmi2SerializeFMUstate" ) );
	fmuFun->deSerializeFMUstate=
		reinterpret_cast<fmi2DeSerializeFMUstateTYPE>( getAdr( &s, bareFMU, "fmi2DeSerializeFMUstate" ) );
	fmuFun->getDirectionalDerivative=
		reinterpret_cast<fmi2GetDirectionalDerivativeTYPE>( getAdr( &s, bareFMU, "fmi2GetDirectionalDerivative" ) );
	// me
	fmuFun->enterEventMode=
		reinterpret_cast<fmi2EnterEventModeTYPE>( getAdr( &s, bareFMU, "fmi2EnterEventMode" ) );
	fmuFun->newDiscreteStates=
		reinterpret_cast<fmi2NewDiscreteStatesTYPE>( getAdr( &s, bareFMU, "fmi2NewDiscreteStates" ) );
	fmuFun->enterContinuousTimeMode=
		reinterpret_cast<fmi2EnterContinuousTimeModeTYPE>( getAdr( &s, bareFMU, "fmi2EnterContinuousTimeMode" ) );
	fmuFun->completedIntegratorStep=
		reinterpret_cast<fmi2CompletedIntegratorStepTYPE>( getAdr( &s, bareFMU, "fmi2CompletedIntegratorStep" ) );

	fmuFun->setTime=
		reinterpret_cast<fmi2SetTimeTYPE>( getAdr( &s, bareFMU, "fmi2SetTime" ) );
	fmuFun->setContinuousStates=
		reinterpret_cast<fmi2SetContinuousStatesTYPE>( getAdr( &s, bareFMU, "fmi2SetContinuousStates" ) );
	fmuFun->getDerivatives=
		reinterpret_cast<fmi2GetDerivativesTYPE>( getAdr( &s, bareFMU, "fmi2GetDerivatives" ) );
	fmuFun->getEventIndicators=
		reinterpret_cast<fmi2GetEventIndicatorsTYPE>( getAdr( &s, bareFMU, "fmi2GetEventIndicators" ) );
	fmuFun->getContinuousStates=
		reinterpret_cast<fmi2GetContinuousStatesTYPE>( getAdr( &s, bareFMU, "fmi2GetContinuousStates" ) );
	fmuFun->getNominalsOfContinuousStates=
		reinterpret_cast<fmi2GetNominalsOfContinuousStatesTYPE>( getAdr( &s, bareFMU, "fmi2GetNominalsOfContinuousStates" ) );

	// cs
	fmuFun->setRealInputDerivatives=
		reinterpret_cast<fmi2SetRealInputDerivativesTYPE>( getAdr( &s, bareFMU, "fmi2SetRealInputDerivatives" ) );
	fmuFun->getRealOutputDerivatives=
		reinterpret_cast<fmi2GetRealOutputDerivativesTYPE>( getAdr( &s, bareFMU, "fmi2GetRealOutputDerivatives" ) );
	fmuFun->doStep=
		reinterpret_cast<fmi2DoStepTYPE>( getAdr( &s, bareFMU, "fmi2DoStep" ) );
	fmuFun->cancelStep=
		reinterpret_cast<fmi2CancelStepTYPE>( getAdr( &s, bareFMU, "fmi2CancelStep" ) );
	fmuFun->getStatus=
		reinterpret_cast<fmi2GetStatusTYPE>( getAdr( &s, bareFMU, "fmi2GetStatus" ) );
	fmuFun->getRealStatus=
		reinterpret_cast<fmi2GetRealStatusTYPE>( getAdr( &s, bareFMU, "fmi2GetRealStatus" ) );
	fmuFun->getIntegerStatus=
		reinterpret_cast<fmi2GetIntegerStatusTYPE>( getAdr( &s, bareFMU, "fmi2GetIntegerStatus" ) );
	fmuFun->getBooleanStatus=
		reinterpret_cast<fmi2GetBooleanStatusTYPE>( getAdr( &s, bareFMU, "fmi2GetBooleanStatus" ) );
	fmuFun->getStringStatus=
		reinterpret_cast<fmi2GetStringStatusTYPE>( getAdr( &s, bareFMU, "fmi2GetStringStatus" ) );

	return s;
}


// Helper function for loading FMU shared library (FMI ME Version 1.0).
void* ModelManager::getAdr( int* s, BareFMUModelExchangePtr bareFMU, const char* functionName )
{
	char name[BUFSIZE];
	void* fp = 0;
	sprintf( name, "%s_%s", bareFMU->description->getModelIdentifier()[0].c_str(), functionName );

#if defined(MINGW)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( bareFMU->functions->dllHandle ), name ) );
#elif defined(_MSC_VER)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( bareFMU->functions->dllHandle ), name ) );
#else
	fp = dlsym( bareFMU->functions->dllHandle, name );
#endif

	if ( !fp ) {
		printf ( "WARNING: Function %s not found.\n", name ); fflush( stdout );
		*s = 0; // mark dll load as 'failed'
	}

	return fp;
}


// Helper function for loading FMU shared library (FMI CS Version 1.0).
void* ModelManager::getAdr( int* s, BareFMUCoSimulationPtr bareFMU, const char* functionName )
{
	char name[BUFSIZE];
	void* fp = 0;
	sprintf( name, "%s_%s", bareFMU->description->getModelIdentifier()[0].c_str(), functionName );

#if defined(MINGW)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( bareFMU->functions->dllHandle ), name ) );
#elif defined(_MSC_VER)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( bareFMU->functions->dllHandle ), name ) );
#else
	fp = dlsym( bareFMU->functions->dllHandle, name );
#endif

	if ( !fp ) {
		printf ( "WARNING: Function %s not found.\n", name ); fflush( stdout );
		*s = 0; // mark dll load as 'failed'
	}

	return fp;
}


// Helper function for loading FMU shared library (FMI ME/CS Version 1.0).
void* ModelManager::getAdr( int* s, BareFMU2Ptr bareFMU, const char* functionName )
{
	char name[BUFSIZE];
	void* fp = 0;
	sprintf( name, "%s", functionName ); // do not prepend the function name for 2.0

#if defined(MINGW)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( bareFMU->functions->dllHandle ), name ) );
#elif defined(_MSC_VER)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( bareFMU->functions->dllHandle ), name ) );
#else
	fp = dlsym( bareFMU->functions->dllHandle, name );
#endif

	if ( !fp ){
		printf ( "WARNING: Function %s not found.\n", name ); fflush( stdout );
		*s = 0; // mark dll load as 'failed'
	}

	/* // Workaround for Dymola bug (quite old, maybe Dymola 2015?): functions are called fmiXYZ instead of fim2XYZ
	if ( !fp ) {
		for ( int i = 3; i < BUFSIZE - 1; i++ ) { name[i] = name[i+1]; }
#if defined(MINGW)
		fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( bareFMU->functions->dllHandle ), name ) );
#elif defined(_MSC_VER)
		fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( bareFMU->functions->dllHandle ), name ) );
#else
		fp = dlsym( bareFMU->functions->dllHandle, name );
#endif
		if ( !fp ){
			printf ( "WARNING: Function %s not found.\n", name ); fflush( stdout );
			*s = 0; // mark dll load as 'failed'
		}
	}
	*/

	return fp;
}


#if defined(MINGW) || defined(_MSC_VER)
// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
string ModelManager::getLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if( 0 == errorMessageID )
        return string(); //No error message has been recorded

    LPSTR messageBuffer = 0;
    size_t size = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL, errorMessageID, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&messageBuffer, 0, NULL );

    string message( messageBuffer, size );

    //Free the buffer.
    LocalFree( messageBuffer );

    return message;
}
#endif


ModelManager::LoadFMUStatus 
ModelManager::loadModelDescription(const std::string& fmuDirUrl,
	std::unique_ptr<ModelDescription>& dest)
{
	// Path to XML model description (OS specific).
	string xmlFilePath;
	string xmlFileUrl = fmuDirUrl + "/modelDescription.xml";
	if ( false == PathFromUrl::getPathFromUrl( xmlFileUrl, xmlFilePath ) ) return description_invalid_uri;

	// Parse XML model description.
	dest = std::unique_ptr<ModelDescription>( new ModelDescription( xmlFilePath ) );
	if ( !dest->isValid() ) {
		return description_invalid;
	}
	return success;
}

template<typename BareFMUType>
ModelManager::UnloadFMUStatus 
ModelManager::unloadFMU( const std::string& modelIdentifier,
	std::map<std::string, BareFMUType> &fmuCollection )
{
	auto itFindFMU = fmuCollection.find( modelIdentifier );
	if ( itFindFMU != fmuCollection.end() ) { // Model identifier found in list of descriptions.
		if ( 1 == itFindFMU->second.use_count() ) {
			// The bare FMU instance found in the list is unique -> can be deleted without causing problems.
			fmuCollection.erase( itFindFMU );
			return ModelManager::ok;
		} else {
			// The bare FMU instance found in the list is NOT unique but still in use somewhere.
			return ModelManager::in_use;
		}
	} else {
		return ModelManager::not_found;
	}
}

template<typename BareFMUType>
ModelManager::UnloadFMUStatus 
ModelManager::unloadAllFMUs(
	std::map<std::string, BareFMUType> &fmuCollection )
{
	auto it = fmuCollection.cbegin();
	while ( it != fmuCollection.cend() )
	{
		// Copy identifier to avoid memory access violations
		std::string modelIdentifier = it->first;
		UnloadFMUStatus status = unloadFMU( modelIdentifier, fmuCollection );
		if ( ok != status ) return status;

		assert(not_found != status);
		it = fmuCollection.cbegin();
	}
	return ok;
}