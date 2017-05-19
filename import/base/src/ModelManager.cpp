/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
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
 * --------------------------------------------------------------*/

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
#include "Shlwapi.h" // necessary for function PathRemoveFileSpec
#endif

#include <stdio.h>
#include <stdexcept>

#include "import/base/include/ModelManager.h"
#include "import/base/include/ModelDescription.h"
#include "import/base/include/CallbackFunctions.h"
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


/**
 * @return a reference of the unique ModelManager isntance
 */
ModelManager& ModelManager::getModelManager()
{
	// Singleton instance
	static ModelManager modelManagerInstance;
	if ( 0 == modelManager_ ) {
		modelManager_ = &modelManagerInstance;
	}
	return *modelManager_;
}


/**
 * consider and get the fmi-functions for a specified FMU
 * @param[in] fmuPath a path to an fmu 
 * @param[in] modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */ 
BareFMUModelExchangePtr
ModelManager::getModel(
	const string& fmuPath,
	const string& modelName,
	const fmiBoolean loggingOn )
{
	// Description already available?
	BareModelCollection::iterator itFind = modelManager_->modelCollection_.find( modelName );
	if ( itFind != modelManager_->modelCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string dllPath;
	string dllUrl = fmuPath + "/binaries/" + FMU_BIN_DIR + "/" + modelName + FMU_BIN_EXT;
	if ( false == PathFromUrl::getPathFromUrl( dllUrl, dllPath ) ) return BareFMUModelExchangePtr();

	string descriptionPath;
	if ( false == PathFromUrl::getPathFromUrl( fmuPath + "/modelDescription.xml", descriptionPath ) ) return BareFMUModelExchangePtr();

	ModelDescription* description = new ModelDescription( descriptionPath );
	if ( false == description->isValid() || description->getVersion() == 2 ) {
		delete description;
		return BareFMUModelExchangePtr();
	}

	BareFMUModelExchangePtr bareFMU = make_shared<BareFMUModelExchange>();
	bareFMU->description = description;

	bareFMU->callbacks = new me::fmiCallbackFunctions;
	bareFMU->callbacks->logger = loggingOn ? callback::verboseLogger : callback::succinctLogger;
	bareFMU->callbacks->allocateMemory = callback::allocateMemory;
	bareFMU->callbacks->freeMemory = callback::freeMemory;

	//Loading the DLL may fail. In this case do not add it to modelCollection_
	if ( 0 == loadDll( dllPath, bareFMU ) ) return BareFMUModelExchangePtr();
	
	// Add bare FMU to list.
	modelManager_->modelCollection_[modelName] = bareFMU;

	return bareFMU;
}

/**
 * consider and get the fmi-functions for a specified FMU
 * @param \in xmlPath a path to an XML description file
 * @param dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library) 
 * @param modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */
BareFMUModelExchangePtr
ModelManager::getModel(
	const string& xmlPath,
	const string& dllPath,
	const string& modelName,
	const fmiBoolean loggingOn )
{
	// Description already available?
	BareModelCollection::iterator itFind = modelManager_->modelCollection_.find( modelName );
	if ( itFind != modelManager_->modelCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string fullDllPath;
	string dllUrl = dllPath + "/" + modelName + FMU_BIN_EXT;
	if ( false == PathFromUrl::getPathFromUrl( dllUrl, fullDllPath ) ) return BareFMUModelExchangePtr();

	string descriptionPath;
	if ( false == PathFromUrl::getPathFromUrl( xmlPath + "/modelDescription.xml", descriptionPath ) ) return BareFMUModelExchangePtr();

	ModelDescription* description = new ModelDescription( descriptionPath );
	if ( false == description->isValid() ) {
		delete description;
		return BareFMUModelExchangePtr();
	}

	BareFMUModelExchangePtr bareFMU = make_shared<BareFMUModelExchange>();
	bareFMU->description = description;

	bareFMU->callbacks = new me::fmiCallbackFunctions;
	bareFMU->callbacks->logger = loggingOn ? callback::verboseLogger : callback::succinctLogger;
	bareFMU->callbacks->allocateMemory = callback::allocateMemory;
	bareFMU->callbacks->freeMemory = callback::freeMemory;

	//Loading the DLL may fail. In this case do not add it to modelCollection_
	if ( 0 == loadDll( fullDllPath, bareFMU ) ) BareFMUModelExchangePtr();

	// Add bare FMU to list.
	modelManager_->modelCollection_[modelName] = bareFMU;
	return bareFMU;
}


/**
 * consider and get the fmi-functions for a specified FMU
 * @param[in] fmuPath a path to an fmu 
 * @param[in] modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */ 
BareFMUCoSimulationPtr
ModelManager::getSlave(
	const string& fmuPath,
	const string& modelName,
	const fmiBoolean loggingOn )
{
	// Description already available?
	BareSlaveCollection::iterator itFind = modelManager_->slaveCollection_.find( modelName );
	if ( itFind != modelManager_->slaveCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string dllPath;
	string dllUrl = fmuPath + "/binaries/" + FMU_BIN_DIR + "/" + modelName + FMU_BIN_EXT;
	if ( false == PathFromUrl::getPathFromUrl( dllUrl, dllPath ) ) return BareFMUCoSimulationPtr();

	string descriptionPath;
	if ( false == PathFromUrl::getPathFromUrl( fmuPath + "/modelDescription.xml", descriptionPath ) ) return BareFMUCoSimulationPtr();

	ModelDescription* description = new ModelDescription( descriptionPath );
	if ( false == description->isValid() ) {
		delete description;
		return BareFMUCoSimulationPtr();
	}

	BareFMUCoSimulationPtr bareFMU = make_shared<BareFMUCoSimulation>();
	bareFMU->description = description;

	bareFMU->callbacks = new cs::fmiCallbackFunctions;
	bareFMU->callbacks->logger = loggingOn ? callback::verboseLogger : callback::succinctLogger;
	bareFMU->callbacks->allocateMemory = callback::allocateMemory;
	bareFMU->callbacks->freeMemory = callback::freeMemory;
	bareFMU->callbacks->stepFinished = callback::stepFinished;

	//Loading the DLL may fail. In this case do not add it to slaveCollection_
	if ( 0 == loadDll( dllPath, bareFMU ) ) return BareFMUCoSimulationPtr();

	// Add bare FMU to list.
	modelManager_->slaveCollection_[modelName] = bareFMU;
	return bareFMU;
}

/**
 * consider and get the fmi-functions for a specified FMU
 * @param \in xmlPath a path to an XML description file
 * @param dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library) 
 * @param modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */
BareFMUCoSimulationPtr
ModelManager::getSlave(
	const string& xmlPath,
	const string& dllPath,
	const string& modelName,
	const fmiBoolean loggingOn )
{
	// Description already available?
	BareSlaveCollection::iterator itFind = modelManager_->slaveCollection_.find( modelName );
	if ( itFind != modelManager_->slaveCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string fullDllPath;
	string dllUrl = dllPath + "/" + modelName + FMU_BIN_EXT;
	if ( false == PathFromUrl::getPathFromUrl( dllUrl, fullDllPath ) ) return BareFMUCoSimulationPtr();

	string descriptionPath;
	if ( false == PathFromUrl::getPathFromUrl( xmlPath + "/modelDescription.xml", descriptionPath ) ) return BareFMUCoSimulationPtr();

	ModelDescription* description = new ModelDescription( descriptionPath );
	if ( false == description->isValid() ) {
		delete description;
		return BareFMUCoSimulationPtr();
	}

	BareFMUCoSimulationPtr bareFMU = make_shared<BareFMUCoSimulation>();
	bareFMU->description = description;

	bareFMU->callbacks = new cs::fmiCallbackFunctions;
	bareFMU->callbacks->logger = loggingOn ? callback::verboseLogger : callback::succinctLogger;
	bareFMU->callbacks->allocateMemory = callback::allocateMemory;
	bareFMU->callbacks->freeMemory = callback::freeMemory;
	bareFMU->callbacks->stepFinished = callback::stepFinished;

	//Loading the DLL may fail. In this case do not add it to slaveCollection_
	if ( 0 == loadDll( fullDllPath, bareFMU ) ) return BareFMUCoSimulationPtr();

	// Add bare FMU to list.
	modelManager_->slaveCollection_[modelName] = bareFMU;
	return bareFMU;
}


/**	
 * consider and get the fmi-functions for a specified FMU
 * @param[in] fmuPath a path to an fmu
 * @param[in] modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */
BareFMU2Ptr
ModelManager::getInstance(
	const string& fmuPath,
	const string& modelName,
	const fmiBoolean loggingOn )
{
	// Description already available?
	BareInstanceCollection::iterator itFind = modelManager_->instanceCollection_.find( modelName );
	if ( itFind != modelManager_->instanceCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string dllPath;
	string dllUrl = fmuPath + "/binaries/" + FMU_BIN_DIR + "/" + modelName + FMU_BIN_EXT;
	if ( false == PathFromUrl::getPathFromUrl( dllUrl, dllPath ) ) return BareFMU2Ptr();

	bool foundDescription;
	ModelDescription* description = new ModelDescription( fmuPath + "/modelDescription.xml", foundDescription );
	if ( ( false == foundDescription ) || ( false == description->isValid() ) )
	{
		delete description;
		return BareFMU2Ptr();
	}

	BareFMU2Ptr bareFMU = make_shared<BareFMU2>();
	bareFMU->description = description;

	bareFMU->callbacks = new fmi2::fmi2CallbackFunctions;
	bareFMU->callbacks->logger = loggingOn ? callback2::verboseLogger : callback2::succinctLogger;
	bareFMU->callbacks->allocateMemory = callback2::allocateMemory;
	bareFMU->callbacks->freeMemory = callback2::freeMemory;
	bareFMU->callbacks->stepFinished = callback2::stepFinished;

	//Loading the DLL may Fail. In this case do not add it to instanceCollection_
	if ( 0 == loadDll( dllPath, bareFMU ) ) return BareFMU2Ptr();

	// Add bare FMU to list.
	modelManager_->instanceCollection_[modelName] = bareFMU;
	return bareFMU;
}

/**
 * consider and get the fmi-functions for a specified FMU
 * @param \in xmlPath a path to an XML description file
 * @param dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library)
 * @param modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */
BareFMU2Ptr
ModelManager::getInstance(
	const string& xmlPath,
	const string& dllPath,
	const string& modelName,
	const fmiBoolean loggingOn )
{
	// Description already available?
	BareInstanceCollection::iterator itFind = modelManager_->instanceCollection_.find( modelName );
	if ( itFind != modelManager_->instanceCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string fullDllPath;
	string dllUrl = dllPath + "/" + modelName + FMU_BIN_EXT;
	if ( false == PathFromUrl::getPathFromUrl( dllUrl, fullDllPath ) ) return BareFMU2Ptr();

	bool foundDescription;
	ModelDescription* description = new ModelDescription( xmlPath + "/modelDescription.xml", foundDescription );
	if ( !foundDescription )
		return BareFMU2Ptr();

	if ( false == description->isValid() ) {
		delete description;
		return BareFMU2Ptr();
	}

	BareFMU2Ptr bareFMU = make_shared<BareFMU2>();
	bareFMU->description = description;

	bareFMU->callbacks = new fmi2::fmi2CallbackFunctions;
	bareFMU->callbacks->logger = loggingOn ? callback2::verboseLogger : callback2::succinctLogger;
	bareFMU->callbacks->allocateMemory = callback2::allocateMemory;
	bareFMU->callbacks->freeMemory = callback2::freeMemory;
	bareFMU->callbacks->stepFinished = callback2::stepFinished;

	//Loading the DLL may fail. In this case do not add it to instanceCollection_
	if ( 0 == loadDll( fullDllPath, bareFMU ) ) return BareFMU2Ptr();

	// Add bare FMU to list.
	modelManager_->instanceCollection_[modelName] = bareFMU;
	return bareFMU;
}


/// Delete a model from the model manager. The model must not be in use.
ModelManager::ModelDeleteStatus ModelManager::deleteModel( const string& modelName )
{
	BareModelCollection::iterator itFindModel = modelManager_->modelCollection_.find( modelName );
	if ( itFindModel != modelManager_->modelCollection_.end() ) { // Model name found in list of descriptions.
		if ( 1 == itFindModel->second.use_count() ) {
			// The bare FMU instance found in the list is unique -> save to delete.
			modelManager_->modelCollection_.erase( itFindModel );
			return ModelManager::ok;
		} else {
			// The bare FMU instance found in the list is NOT unique but still in use somewhere.
			return ModelManager::in_use;
		}
	}

	BareSlaveCollection::iterator itFindSlave = modelManager_->slaveCollection_.find( modelName );
	if ( itFindSlave != modelManager_->slaveCollection_.end() ) { // Model name found in list of descriptions.
		if ( 1 == itFindSlave->second.use_count() ) {
			// The bare FMU instance found in the list is unique -> save to delete.
			modelManager_->slaveCollection_.erase( itFindSlave );
			return ModelManager::ok;
		} else {
			// The bare FMU instance found in the list is NOT unique but still in use somewhere.
			return ModelManager::in_use;
		}
	}

	BareInstanceCollection::iterator itFindInstance = modelManager_->instanceCollection_.find( modelName );
	if ( itFindInstance != modelManager_->instanceCollection_.end() ) { // Model name found in list of descriptions.
		if ( 1 == itFindInstance->second.use_count() ) {
			// The bare FMU instance found in the list is unique -> save to delete.
			modelManager_->instanceCollection_.erase( itFindInstance );
			return ModelManager::ok;
		} else {
			// The bare FMU instance found in the list is NOT unique but still in use somewhere.
			return ModelManager::in_use;
		}
	}
	
	return ModelManager::not_found;
}


/**
 * Load the given dll and set function pointers in fmu
 * 
 * @param[in] dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library) 
 * @param[out] fmuFun  a ptr to fmi functions dictated to the given FMU 
 * @return 0 if failure otherwise 1
 */ 
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


/**
 * Load the given dll and set function pointers in fmu
 * 
 * @param[in] dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library) 
 * @param[out] fmuFun  a ptr to fmi functions dictated to the given FMU 
 * @return 0 if failure otherwise 1
 */ 
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


void* ModelManager::getAdr( int* s, BareFMUModelExchangePtr bareFMU, const char* functionName )
{
	char name[BUFSIZE];
	void* fp = 0;
	sprintf( name, "%s_%s", bareFMU->description->getModelIdentifier().c_str(), functionName );

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


void* ModelManager::getAdr( int* s, BareFMUCoSimulationPtr bareFMU, const char* functionName )
{
	char name[BUFSIZE];
	void* fp = 0;
	sprintf( name, "%s_%s", bareFMU->description->getModelIdentifier().c_str(), functionName );

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
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
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
