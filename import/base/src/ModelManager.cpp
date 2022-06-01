/* ---------------------------------------------------------------------------
 * Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
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
	const fmippBoolean loggingOn,
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
	const fmippBoolean loggingOn, FMUType& type, std::string& modelIdentifier)
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

	HANDLE h = openDLL( &s, dllPath );
	if ( !s ) return 0;

	FMUModelExchange_functions* fmuFun = new FMUModelExchange_functions;
	bareFMU->functions = fmuFun;
	fmuFun->dllHandle = h;

	// FMI for Model Exchange 1.0
	fmuFun->getModelTypesPlatform =	getAdr10<fGetModelTypesPlatform>( &s, bareFMU, "fmiGetModelTypesPlatform" );
	fmuFun->instantiateModel = getAdr10<fInstantiateModel>( &s, bareFMU, "fmiInstantiateModel" );
	fmuFun->freeModelInstance = getAdr10<fFreeModelInstance>( &s, bareFMU, "fmiFreeModelInstance" );
	fmuFun->completedIntegratorStep = getAdr10<fCompletedIntegratorStep>( &s, bareFMU, "fmiCompletedIntegratorStep" );
	fmuFun->initialize = getAdr10<fInitialize>( &s, bareFMU, "fmiInitialize" );
	fmuFun->eventUpdate = getAdr10<fEventUpdate>( &s, bareFMU, "fmiEventUpdate" );
	fmuFun->getStateValueReferences = getAdr10<fGetStateValueReferences>( &s, bareFMU, "fmiGetStateValueReferences" );
	fmuFun->terminate = getAdr10<fTerminate>( &s, bareFMU, "fmiTerminate" );
	fmuFun->getVersion = getAdr10<fGetVersion>( &s, bareFMU, "fmiGetVersion" );
	fmuFun->setDebugLogging = getAdr10<fSetDebugLogging>( &s, bareFMU, "fmiSetDebugLogging" );
	fmuFun->setTime = getAdr10<fSetTime>( &s, bareFMU, "fmiSetTime" );
	fmuFun->setContinuousStates = getAdr10<fSetContinuousStates>( &s, bareFMU, "fmiSetContinuousStates" );
	fmuFun->setReal = getAdr10<fSetReal>( &s, bareFMU, "fmiSetReal" );
	fmuFun->setInteger = getAdr10<fSetInteger>( &s, bareFMU, "fmiSetInteger" );
	fmuFun->setBoolean = getAdr10<fSetBoolean>( &s, bareFMU, "fmiSetBoolean" );
	fmuFun->setString = getAdr10<fSetString>( &s, bareFMU, "fmiSetString" );
	fmuFun->getReal = getAdr10<fGetReal>( &s, bareFMU, "fmiGetReal" );
	fmuFun->getInteger = getAdr10<fGetInteger>( &s, bareFMU, "fmiGetInteger" );
	fmuFun->getBoolean = getAdr10<fGetBoolean>( &s, bareFMU, "fmiGetBoolean" );
	fmuFun->getString = getAdr10<fGetString>( &s, bareFMU, "fmiGetString" );
	fmuFun->getDerivatives = getAdr10<fGetDerivatives>( &s, bareFMU, "fmiGetDerivatives" );
	fmuFun->getEventIndicators = getAdr10<fGetEventIndicators>( &s, bareFMU, "fmiGetEventIndicators" );
	fmuFun->getContinuousStates = getAdr10<fGetContinuousStates>( &s, bareFMU, "fmiGetContinuousStates" );
	fmuFun->getNominalContinuousStates = getAdr10<fGetNominalContinuousStates>( &s, bareFMU, "fmiGetNominalContinuousStates" );

	return s;
}

// Helper function for loading a bare FMU shared library (FMI CS Version 1.0).
int ModelManager::loadDll( string dllPath, BareFMUCoSimulationPtr bareFMU )
{
	using namespace cs;

	int s = 1;

	HANDLE h = openDLL( &s, dllPath );
	if ( !s ) return 0;

	FMUCoSimulation_functions* fmuFun = new FMUCoSimulation_functions;
	bareFMU->functions = fmuFun;
	fmuFun->dllHandle = h;

	fmuFun->getTypesPlatform = getAdr10<fGetTypesPlatform>( &s, bareFMU, "fmiGetTypesPlatform" );
	if ( s == 0 ) {
		s = 1; // work around bug for FMUs exported using Dymola 2012 and SimulationX 3.x
		fmuFun->getTypesPlatform = getAdr10<fGetTypesPlatform>( &s, bareFMU, "fmiGetModelTypesPlatform" );
		if ( s == 1 ) { printf( "using fmiGetModelTypesPlatform instead\n" ); fflush( stdout ); }
	}

	// FMI for Co-Simulation 1.0
	fmuFun->instantiateSlave = getAdr10<fInstantiateSlave>( &s, bareFMU, "fmiInstantiateSlave" );
	fmuFun->initializeSlave = getAdr10<fInitializeSlave>( &s, bareFMU, "fmiInitializeSlave" );
	fmuFun->terminateSlave = getAdr10<fTerminateSlave>( &s, bareFMU, "fmiTerminateSlave" );
	fmuFun->resetSlave = getAdr10<fResetSlave>( &s, bareFMU, "fmiResetSlave" );
	fmuFun->freeSlaveInstance = getAdr10<fFreeSlaveInstance>( &s, bareFMU, "fmiFreeSlaveInstance" );
	fmuFun->cancelStep = getAdr10<fCancelStep>( &s, bareFMU, "fmiCancelStep" );
	fmuFun->doStep = getAdr10<fDoStep>( &s, bareFMU, "fmiDoStep" );
	fmuFun->getStatus = getAdr10<fGetStatus>( &s, bareFMU, "fmiGetStatus" );
	fmuFun->getRealStatus = getAdr10<fGetRealStatus>( &s, bareFMU, "fmiGetRealStatus" );
	fmuFun->getIntegerStatus = getAdr10<fGetIntegerStatus>( &s, bareFMU, "fmiGetIntegerStatus" );
	fmuFun->getBooleanStatus = getAdr10<fGetBooleanStatus>( &s, bareFMU, "fmiGetBooleanStatus" );
	fmuFun->getStringStatus = getAdr10<fGetStringStatus>( &s, bareFMU, "fmiGetStringStatus" );
	fmuFun->getVersion = getAdr10<fGetVersion>( &s, bareFMU, "fmiGetVersion" );
	fmuFun->setDebugLogging = getAdr10<fSetDebugLogging>( &s, bareFMU, "fmiSetDebugLogging" );
	fmuFun->setReal = getAdr10<fSetReal>( &s, bareFMU, "fmiSetReal" );
	fmuFun->setInteger = getAdr10<fSetInteger>( &s, bareFMU, "fmiSetInteger" );
	fmuFun->setBoolean = getAdr10<fSetBoolean>( &s, bareFMU, "fmiSetBoolean" );
	fmuFun->setString = getAdr10<fSetString>( &s, bareFMU, "fmiSetString" );
	fmuFun->setRealInputDerivatives = getAdr10<fSetRealInputDerivatives>( &s, bareFMU, "fmiSetRealInputDerivatives" );
	fmuFun->getReal = getAdr10<fGetReal>( &s, bareFMU, "fmiGetReal" );
	fmuFun->getInteger = getAdr10<fGetInteger>( &s, bareFMU, "fmiGetInteger" );
	fmuFun->getBoolean = getAdr10<fGetBoolean>( &s, bareFMU, "fmiGetBoolean" );
	fmuFun->getString = getAdr10<fGetString>( &s, bareFMU, "fmiGetString" );
	fmuFun->getRealOutputDerivatives = getAdr10<fGetRealOutputDerivatives>( &s, bareFMU, "fmiGetRealOutputDerivatives" );

	return s;
}

// Helper function for loading a bare FMU shared library (FMI ME/CS Version 2.0).
int ModelManager::loadDll( string dllPath, BareFMU2Ptr bareFMU )
{
	using namespace fmi2;

	assert( bareFMU );
	assert( bareFMU->description );

	int s = 1;

	HANDLE h = openDLL( &s, dllPath );
	if ( !s ) return 0;

	FMU2_functions* fmuFun = new FMU2_functions;
	bareFMU->functions = fmuFun;
	fmuFun->dllHandle = h;

	// Load Common Functions
	s &= loadCommonFMI20Functions( bareFMU );
	deleteFMI20MEandCSSpecificFunctions( bareFMU );

	FMUType type = bareFMU->description->getFMUType();
	if ( ( type == fmi_2_0_me ) || ( type == fmi_2_0_me_and_cs ) )
	{ // FMI 2.0 ME specific functions should be available
		s &= loadFMI20MESpecificFunctions( bareFMU );
	}

	if ( ( type == fmi_2_0_cs ) || ( type == fmi_2_0_me_and_cs ) )
	{ // FMI 2.0 CS specific functions should be available
		s &= loadFMI20CSSpecificFunctions( bareFMU );
	}

	return s;
}

int ModelManager::loadCommonFMI20Functions(BareFMU2Ptr bareFMU)
{
	using namespace fmi2;
	
	assert( bareFMU );
	assert( bareFMU->functions );

	int s = 1; // Status Variable
	FMU2_functions* fmuFun = bareFMU->functions;

	// FMI for Model Exchange 2.0
	fmuFun->getTypesPlatform = getAdr20<fmi2GetTypesPlatformTYPE>( &s, bareFMU, "fmi2GetTypesPlatform" );
	fmuFun->getVersion = getAdr20<fmi2GetVersionTYPE>( &s, bareFMU, "fmi2GetVersion" );
	fmuFun->setDebugLogging = getAdr20<fmi2SetDebugLoggingTYPE>( &s, bareFMU, "fmi2SetDebugLogging" );
	fmuFun->instantiate = getAdr20<fmi2InstantiateTYPE>( &s, bareFMU, "fmi2Instantiate" );
	fmuFun->freeInstance = getAdr20<fmi2FreeInstanceTYPE>( &s, bareFMU, "fmi2FreeInstance" );
	
	fmuFun->setupExperiment = getAdr20<fmi2SetupExperimentTYPE>( &s, bareFMU, "fmi2SetupExperiment" );
	fmuFun->enterInitializationMode = getAdr20<fmi2EnterInitializationModeTYPE>( &s, bareFMU, "fmi2EnterInitializationMode" );
	fmuFun->exitInitializationMode = getAdr20<fmi2ExitInitializationModeTYPE>( &s, bareFMU, "fmi2ExitInitializationMode" );

	fmuFun->terminate = getAdr20<fmi2TerminateTYPE>( &s, bareFMU, "fmi2Terminate" );
	fmuFun->reset = getAdr20<fmi2ResetTYPE>( &s, bareFMU, "fmi2Reset" );

	fmuFun->getReal = getAdr20<fmi2GetRealTYPE>( &s, bareFMU, "fmi2GetReal" );
	fmuFun->getInteger = getAdr20<fmi2GetIntegerTYPE>( &s, bareFMU, "fmi2GetInteger" );
	fmuFun->getBoolean = getAdr20<fmi2GetBooleanTYPE>( &s, bareFMU, "fmi2GetBoolean" );
	fmuFun->getString = getAdr20<fmi2GetStringTYPE>( &s, bareFMU, "fmi2GetString" );

	fmuFun->setReal = getAdr20<fmi2SetRealTYPE>( &s, bareFMU, "fmi2SetReal" );
	fmuFun->setInteger = getAdr20<fmi2SetIntegerTYPE>( &s, bareFMU, "fmi2SetInteger" );
	fmuFun->setBoolean = getAdr20<fmi2SetBooleanTYPE>( &s, bareFMU, "fmi2SetBoolean" );
	fmuFun->setString =	 getAdr20<fmi2SetStringTYPE>( &s, bareFMU, "fmi2SetString" );

	fmuFun->getFMUstate = getAdr20<fmi2GetFMUstateTYPE>( &s, bareFMU, "fmi2GetFMUstate" );
	fmuFun->setFMUstate = getAdr20<fmi2SetFMUstateTYPE>( &s, bareFMU, "fmi2SetFMUstate" );
	fmuFun->freeFMUstate = getAdr20<fmi2FreeFMUstateTYPE>( &s, bareFMU, "fmi2FreeFMUstate" );
	fmuFun->serializedFMUstateSize = getAdr20<fmi2SerializedFMUstateSizeTYPE>( &s, bareFMU, "fmi2SerializedFMUstateSize" );
	fmuFun->serializeFMUstate = getAdr20<fmi2SerializeFMUstateTYPE>( &s, bareFMU, "fmi2SerializeFMUstate" );
	fmuFun->deSerializeFMUstate = getAdr20<fmi2DeSerializeFMUstateTYPE>( &s, bareFMU, "fmi2DeSerializeFMUstate" );
	fmuFun->getDirectionalDerivative = getAdr20<fmi2GetDirectionalDerivativeTYPE>( &s, bareFMU, "fmi2GetDirectionalDerivative" );

	return s;
}

void ModelManager::deleteFMI20MEandCSSpecificFunctions(BareFMU2Ptr bareFMU)
{
	using namespace fmi2;

	assert( bareFMU );
	assert( bareFMU->functions );

	FMU2_functions* fmuFun = bareFMU->functions;

	// me
	fmuFun->enterEventMode = reinterpret_cast<fmi2EnterEventModeTYPE>( fmi2DoNotCall );
	fmuFun->newDiscreteStates = reinterpret_cast<fmi2NewDiscreteStatesTYPE>( fmi2DoNotCall );
	fmuFun->enterContinuousTimeMode = reinterpret_cast<fmi2EnterContinuousTimeModeTYPE>( fmi2DoNotCall );
	fmuFun->completedIntegratorStep = reinterpret_cast<fmi2CompletedIntegratorStepTYPE>( fmi2DoNotCall );

	fmuFun->setTime = reinterpret_cast<fmi2SetTimeTYPE>( fmi2DoNotCall );
	fmuFun->setContinuousStates = reinterpret_cast<fmi2SetContinuousStatesTYPE>( fmi2DoNotCall );
	fmuFun->getDerivatives = reinterpret_cast<fmi2GetDerivativesTYPE>( fmi2DoNotCall );
	fmuFun->getEventIndicators = reinterpret_cast<fmi2GetEventIndicatorsTYPE>( fmi2DoNotCall );
	fmuFun->getContinuousStates = reinterpret_cast<fmi2GetContinuousStatesTYPE>( fmi2DoNotCall );
	fmuFun->getNominalsOfContinuousStates = reinterpret_cast<fmi2GetNominalsOfContinuousStatesTYPE>( fmi2DoNotCall );

	// cs
	fmuFun->setRealInputDerivatives = reinterpret_cast<fmi2SetRealInputDerivativesTYPE>( fmi2DoNotCall );
	fmuFun->getRealOutputDerivatives = reinterpret_cast<fmi2GetRealOutputDerivativesTYPE>( fmi2DoNotCall );
	fmuFun->doStep = reinterpret_cast<fmi2DoStepTYPE>( fmi2DoNotCall );
	fmuFun->cancelStep = reinterpret_cast<fmi2CancelStepTYPE>( fmi2DoNotCall );
	fmuFun->getStatus = reinterpret_cast<fmi2GetStatusTYPE>( fmi2DoNotCall );
	fmuFun->getRealStatus = reinterpret_cast<fmi2GetRealStatusTYPE>( fmi2DoNotCall );
	fmuFun->getIntegerStatus = reinterpret_cast<fmi2GetIntegerStatusTYPE>( fmi2DoNotCall );
	fmuFun->getBooleanStatus = reinterpret_cast<fmi2GetBooleanStatusTYPE>( fmi2DoNotCall );
	fmuFun->getStringStatus = reinterpret_cast<fmi2GetStringStatusTYPE>( fmi2DoNotCall );
}

int ModelManager::loadFMI20MESpecificFunctions(BareFMU2Ptr bareFMU)
{
	using namespace fmi2;
	
	assert( bareFMU );
	assert( bareFMU->functions );

	int s = 1; // Status Variable
	FMU2_functions* fmuFun = bareFMU->functions;

	// me
	fmuFun->enterEventMode = getAdr20<fmi2EnterEventModeTYPE>( &s, bareFMU, "fmi2EnterEventMode" );
	fmuFun->newDiscreteStates = getAdr20<fmi2NewDiscreteStatesTYPE>( &s, bareFMU, "fmi2NewDiscreteStates" );
	fmuFun->enterContinuousTimeMode = getAdr20<fmi2EnterContinuousTimeModeTYPE>( &s, bareFMU, "fmi2EnterContinuousTimeMode" );
	fmuFun->completedIntegratorStep = getAdr20<fmi2CompletedIntegratorStepTYPE>( &s, bareFMU, "fmi2CompletedIntegratorStep" );

	fmuFun->setTime = getAdr20<fmi2SetTimeTYPE>( &s, bareFMU, "fmi2SetTime" );
	fmuFun->setContinuousStates = getAdr20<fmi2SetContinuousStatesTYPE>( &s, bareFMU, "fmi2SetContinuousStates" );
	fmuFun->getDerivatives = getAdr20<fmi2GetDerivativesTYPE>( &s, bareFMU, "fmi2GetDerivatives" );
	fmuFun->getEventIndicators = getAdr20<fmi2GetEventIndicatorsTYPE>( &s, bareFMU, "fmi2GetEventIndicators" );
	fmuFun->getContinuousStates = getAdr20<fmi2GetContinuousStatesTYPE>( &s, bareFMU, "fmi2GetContinuousStates" );
	fmuFun->getNominalsOfContinuousStates = getAdr20<fmi2GetNominalsOfContinuousStatesTYPE>( &s, bareFMU, "fmi2GetNominalsOfContinuousStates" );

	return s;
}

int ModelManager::loadFMI20CSSpecificFunctions(BareFMU2Ptr bareFMU)
{
	using namespace fmi2;
	
	assert( bareFMU );
	assert( bareFMU->functions );

	int s = 1; // Status Variable
	FMU2_functions* fmuFun = bareFMU->functions;

	// cs
	fmuFun->setRealInputDerivatives = getAdr20<fmi2SetRealInputDerivativesTYPE>( &s, bareFMU, "fmi2SetRealInputDerivatives" );
	fmuFun->getRealOutputDerivatives = getAdr20<fmi2GetRealOutputDerivativesTYPE>( &s, bareFMU, "fmi2GetRealOutputDerivatives" );
	fmuFun->doStep = getAdr20<fmi2DoStepTYPE>( &s, bareFMU, "fmi2DoStep" );
	fmuFun->cancelStep = getAdr20<fmi2CancelStepTYPE>( &s, bareFMU, "fmi2CancelStep" );
	fmuFun->getStatus = getAdr20<fmi2GetStatusTYPE>( &s, bareFMU, "fmi2GetStatus" );
	fmuFun->getRealStatus = getAdr20<fmi2GetRealStatusTYPE>( &s, bareFMU, "fmi2GetRealStatus" );
	fmuFun->getIntegerStatus = getAdr20<fmi2GetIntegerStatusTYPE>( &s, bareFMU, "fmi2GetIntegerStatus" );
	fmuFun->getBooleanStatus = getAdr20<fmi2GetBooleanStatusTYPE>( &s, bareFMU, "fmi2GetBooleanStatus" );
	fmuFun->getStringStatus = getAdr20<fmi2GetStringStatusTYPE>( &s, bareFMU, "fmi2GetStringStatus" );

	return s;
}

// Opens the DLL/SO file
HANDLE ModelManager::openDLL(int* status, const string& dllPath)
{
	assert( status );

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
		printf( "ERROR: Could not load \"%s\" (%s)\n", dllPath.c_str(), error.c_str() );
		fflush(stdout);
		*status = 0;
		return 0; // failure
	}

#else

	HANDLE h = dlopen( dllPath.c_str(), RTLD_LAZY );

	if ( !h ) {
		printf( "DLOPEN ERROR: Could not load \"%s\":\n%s\n", dllPath.c_str(), dlerror() );
		fflush(stdout);
		*status = 0;
		return 0; // failure
	}
#endif
	return h;
}

// Helper function for loading FMU shared library.
template<typename FunctionPtrType, typename BareFMUPtrType>
FunctionPtrType ModelManager::getAdr10( int* s, BareFMUPtrType bareFMU, 
		const char* functionName )
{
	assert( bareFMU );
	assert( functionName );

	char name[BUFSIZE];
	sprintf( name, "%s_%s", 
		bareFMU->description->getModelIdentifier()[0].c_str(), functionName );

	return getAdrRaw<FunctionPtrType>( s, bareFMU->functions->dllHandle, name );
}

// Helper function for loading FMU shared library (FMI ME/CS Version 2.0).
template<typename FunctionPtrType>
FunctionPtrType ModelManager::getAdr20( int* s, BareFMU2Ptr bareFMU, 
		const char* functionName )
{
	assert( bareFMU );
	assert( functionName );

	char name[BUFSIZE];
	sprintf( name, "%s", functionName ); // do not prepend the function name for 2.0

	return getAdrRaw<FunctionPtrType>( s, bareFMU->functions->dllHandle, name );
}

// Load function pointer by DLL name
template<typename FunctionPtrType> 
FunctionPtrType ModelManager::getAdrRaw(int* s, HANDLE dllHandle,
	const char* rawFunctionName)
{
	assert( s );
	assert( dllHandle );
	assert( rawFunctionName );

	FunctionPtrType fp = 0;
#if defined(MINGW)
	fp = reinterpret_cast<FunctionPtrType>( 
		GetProcAddress( static_cast<HMODULE>( dllHandle ), rawFunctionName ) );
#elif defined(_MSC_VER)
	fp = reinterpret_cast<FunctionPtrType>( 
		GetProcAddress( static_cast<HMODULE>( dllHandle ), rawFunctionName ) );
#else
	fp = reinterpret_cast<FunctionPtrType>( 
		dlsym( dllHandle, rawFunctionName ) );
#endif

	if ( !fp ) {
		printf ( "WARNING: Function %s not found.\n", rawFunctionName ); 
		fflush( stdout );
		*s = 0; // mark dll load as 'failed'
	}
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

fmi2Status ModelManager::fmi2DoNotCall(...)
{
	printf ( "ERROR: An FMI 2.0 function which is not contained in the DLL/SO "
		"was called.\n" ); 
	fflush( stdout );

	assert( false );
	return fmi2Error;
}
