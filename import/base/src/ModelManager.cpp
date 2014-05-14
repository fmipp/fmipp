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

#include <stdio.h>
#include <stdexcept>

#if defined( WIN32 )
#include "Windows.h"
#include "Shlwapi.h"
#include "TCHAR.h"
#endif


#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/FMIPPConfig.h"
#include "import/base/include/ModelManager.h"


using namespace std;


ModelManager* ModelManager::modelManager_ = 0;


ModelManager::~ModelManager()
{
	BareModelCollection::iterator beginModels = modelCollection_.begin();
	BareModelCollection::iterator endModels = modelCollection_.end();
	for ( BareModelCollection::iterator it = beginModels; it != endModels; ++it ) {

		if ( 0 != it->second->functions->dllHandle ) {
#if defined(MINGW)
			FreeLibrary( static_cast<HMODULE>( it->second->functions->dllHandle ) );
#elif defined(_MSC_VER)
			FreeLibrary( static_cast<HMODULE>( it->second->functions->dllHandle ) );
#else
			dlclose( it->second->functions->dllHandle );
#endif
		}

		delete it->second->description;

		delete it->second;
	}


	BareSlaveCollection::iterator beginSlaves = slaveCollection_.begin();
	BareSlaveCollection::iterator endSlaves = slaveCollection_.end();
	for ( BareSlaveCollection::iterator it = beginSlaves; it != endSlaves; ++it ) {

		if ( 0 != it->second->functions->dllHandle ) {
#if defined(MINGW)
			FreeLibrary( static_cast<HMODULE>( it->second->functions->dllHandle ) );
#elif defined(_MSC_VER)
			FreeLibrary( static_cast<HMODULE>( it->second->functions->dllHandle ) );
#else
			dlclose( it->second->functions->dllHandle );
#endif
		}

		delete it->second->description;

		delete it->second;
	}
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
BareFMUModelExchange* ModelManager::getModel( const string& fmuPath,
					      const string& modelName )
{
	// Description already available?
	BareModelCollection::iterator itFind = modelManager_->modelCollection_.find( modelName );
	if ( itFind != modelManager_->modelCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string dllPath = getPathFromUrl( fmuPath + "/binaries/" + FMU_BIN_DIR + "/" + modelName + FMU_BIN_EXT );
	BareFMUModelExchange* bareFMU = new BareFMUModelExchange;

	string descriptionPath = getPathFromUrl( fmuPath + "/modelDescription.xml" );
	bareFMU->description = new ModelDescription( descriptionPath );

	loadDll( dllPath, bareFMU );

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
BareFMUModelExchange* ModelManager::getModel( const string& xmlPath,
					      const string& dllPath,
					      const string& modelName )
{
	// Description already available?
	BareModelCollection::iterator itFind = modelManager_->modelCollection_.find( modelName );
	if ( itFind != modelManager_->modelCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string fullDllPath = getPathFromUrl( dllPath + "/" + modelName + FMU_BIN_EXT );

	BareFMUModelExchange* bareFMU = new BareFMUModelExchange;

	string descriptionPath = getPathFromUrl( xmlPath + "/" + modelName + ".xml" );
	bareFMU->description = new ModelDescription( descriptionPath );

	loadDll( fullDllPath, bareFMU );

	modelManager_->modelCollection_[modelName] = bareFMU;
	return bareFMU;
}


/**
 * consider and get the fmi-functions for a specified FMU
 * @param[in] fmuPath a path to an fmu 
 * @param[in] modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */ 
BareFMUCoSimulation* ModelManager::getSlave( const string& fmuPath,
					     const string& modelName )
{
	// Description already available?
	BareSlaveCollection::iterator itFind = modelManager_->slaveCollection_.find( modelName );
	if ( itFind != modelManager_->slaveCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string dllPath = getPathFromUrl( fmuPath + "/binaries/" + FMU_BIN_DIR + "/" + modelName + FMU_BIN_EXT );

	BareFMUCoSimulation* bareFMU = new BareFMUCoSimulation;

	string descriptionPath = getPathFromUrl( fmuPath + "/modelDescription.xml" );
	bareFMU->description = new ModelDescription( descriptionPath );

	loadDll( dllPath, bareFMU );

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
BareFMUCoSimulation* ModelManager::getSlave( const string& xmlPath,
					     const string& dllPath,
					     const string& modelName )
{
	// Description already available?
	BareSlaveCollection::iterator itFind = modelManager_->slaveCollection_.find( modelName );
	if ( itFind != modelManager_->slaveCollection_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string fullDllPath = getPathFromUrl( dllPath + "/" + modelName + FMU_BIN_EXT );

	BareFMUCoSimulation* bareFMU = new BareFMUCoSimulation;

	string descriptionPath = getPathFromUrl( xmlPath + "/" + modelName + ".xml" );
	bareFMU->description = new ModelDescription( descriptionPath );

	loadDll( fullDllPath, bareFMU );

	modelManager_->slaveCollection_[modelName] = bareFMU;
	return bareFMU;
}


/**
 * Load the given dll and set function pointers in fmu
 * 
 * @param[in] dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library) 
 * @param[out] fmuFun  a ptr to fmi functions dictated to the given FMU 
 * @return 0 if failure otherwise 1
 */ 
int ModelManager::loadDll( string dllPath, BareFMUModelExchange* bareFMU )
{
	using namespace me;

	int s = 1;

#if defined(MINGW)
	HANDLE h = LoadLibrary( dllPath.c_str() );
#elif defined(_MSC_VER)
	HANDLE h = LoadLibrary( dllPath.c_str() );
#else
	HANDLE h = dlopen( dllPath.c_str(), RTLD_LAZY );
#endif

	if ( !h ) {
		printf( "ERROR: Could not load %s\n", dllPath.c_str() ); fflush(stdout);
		return 0; // failure
	}

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
int ModelManager::loadDll( string dllPath, BareFMUCoSimulation* bareFMU )
{
	using namespace cs;

	int s = 1;

#if defined(MINGW)
	HANDLE h = LoadLibrary( dllPath.c_str() );
#elif defined(_MSC_VER)
	HANDLE h = LoadLibrary( dllPath.c_str() );
#else
	HANDLE h = dlopen( dllPath.c_str(), RTLD_LAZY );
#endif

	if ( !h ) {
		printf( "ERROR: Could not load %s\n", dllPath.c_str() ); fflush(stdout);
		return 0; // failure
	}

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


void* ModelManager::getAdr( int* s, BareFMUModelExchange *bareFMU, const char* functionName )
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


void* ModelManager::getAdr( int* s, BareFMUCoSimulation *bareFMU, const char* functionName )
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


string ModelManager::getPathFromUrl( const string& inputFileUrl )
{
#if defined( WIN32 )
	LPCTSTR fileUrl = inputFileUrl.c_str();
	LPTSTR filePath = new TCHAR[MAX_PATH];
	DWORD filePathSize = inputFileUrl.size() + 1;
	DWORD tmp = 0;
	PathCreateFromUrl( fileUrl, filePath, &filePathSize, tmp );
	return string( filePath );
#else
	/// \FIXME Replace with proper Linux implementation.
	if ( inputFileUrl.substr( 0, 7 ) != "file://" )
		throw invalid_argument( string( "Cannot handle URI: " ) + inputFileUrl );

	return inputFileUrl.substr( 7, inputFileUrl.size() );
#endif
}
