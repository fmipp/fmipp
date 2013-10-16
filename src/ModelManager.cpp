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

#if defined(_MSC_VER) or defined(MINGW)
#include "Windows.h"
#include "Shlwapi.h"
#include "TCHAR.h"
#endif

#include "FMIPPConfig.h"
#include "ModelManager.h"


using namespace std;


ModelManager* ModelManager::modelManager_ = 0;


ModelManager::~ModelManager()
{
	ModelDescriptions::iterator beginModels = modelDescriptions_.begin();
	ModelDescriptions::iterator endModels = modelDescriptions_.end();
	for ( ModelDescriptions::iterator it = beginModels; it != endModels; ++it ) {
#if defined(MINGW)
		FreeLibrary( static_cast<HMODULE>( it->second->dllHandle ) );
#elif defined(_MSC_VER)
		FreeLibrary( static_cast<HMODULE>( it->second->dllHandle ) );
#else
		dlclose( it->second->dllHandle );
#endif

		freeElement( it->second->modelDescription );

		delete it->second;
	}


	SlaveDescriptions::iterator beginSlaves = slaveDescriptions_.begin();
	SlaveDescriptions::iterator endSlaves = slaveDescriptions_.end();
	for ( SlaveDescriptions::iterator it = beginSlaves; it != endSlaves; ++it ) {
#if defined(MINGW)
		FreeLibrary( static_cast<HMODULE>( it->second->dllHandle ) );
#elif defined(_MSC_VER)
		FreeLibrary( static_cast<HMODULE>( it->second->dllHandle ) );
#else
		dlclose( it->second->dllHandle );
#endif

		freeElement( it->second->modelDescription );

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
FMU_functions* ModelManager::getModel( const string& fmuPath,
				       const string& modelName )
{
	// Description already available?
	ModelDescriptions::iterator itFind = modelManager_->modelDescriptions_.find( modelName );
	if ( itFind != modelManager_->modelDescriptions_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	// fix this for other OSs and 32bit !!!
#if defined(_MSC_VER)
	string dllPath = getPathFromUrl( fmuPath + "/binaries/win32/" + modelName + ".dll" );
#elif defined(MINGW)
	string dllPath = getPathFromUrl( fmuPath + "/binaries/win32/" + modelName + ".dll" );
#elif defined(__APPLE__)
	string dllPath = getPathFromUrl( fmuPath + "/binaries/darwin64/" + modelName + ".dylib" );
#else
	string dllPath = getPathFromUrl( fmuPath + "/binaries/linux64/" + modelName + ".so" );
#endif

	FMU_functions* description = new FMU_functions;

	string descriptionPath = getPathFromUrl( fmuPath + "/modelDescription.xml" );
	description->modelDescription = parse( descriptionPath.c_str() );

	loadDll( dllPath, description );

	modelManager_->modelDescriptions_[modelName] = description;
	return description;
}

/**
 * consider and get the fmi-functions for a specified FMU
 * @param \in xmlPath a path to an XML description file
 * @param dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library) 
 * @param modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */
FMU_functions* ModelManager::getModel( const string& xmlPath,
				       const string& dllPath,
				       const string& modelName )
{
	// Description already available?
	ModelDescriptions::iterator itFind = modelManager_->modelDescriptions_.find( modelName );
	if ( itFind != modelManager_->modelDescriptions_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string fullDllPath = getPathFromUrl( dllPath + "/" + modelName + ".dll" );

	FMU_functions* description = new FMU_functions;

	string descriptionPath = getPathFromUrl( xmlPath + "/" + modelName + ".xml" );
	description->modelDescription = parse( descriptionPath.c_str() );

	loadDll( fullDllPath, description );

	modelManager_->modelDescriptions_[modelName] = description;
	return description;
}


/**
 * consider and get the fmi-functions for a specified FMU
 * @param[in] fmuPath a path to an fmu 
 * @param[in] modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */ 
FMUCoSimulation_functions* ModelManager::getSlave( const string& fmuPath,
						   const string& modelName )
{
	// Description already available?
	SlaveDescriptions::iterator itFind = modelManager_->slaveDescriptions_.find( modelName );
	if ( itFind != modelManager_->slaveDescriptions_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	// fix this for other OSs and 32bit !!!
#if defined(_MSC_VER)
	string dllPath = getPathFromUrl( fmuPath + "/binaries/win32/" + modelName + ".dll" );
#elif defined(MINGW)
	string dllPath = getPathFromUrl( fmuPath + "/binaries/win32/" + modelName + ".dll" );
#elif defined(__APPLE__)
	string dllPath = getPathFromUrl( fmuPath + "/binaries/darwin64/" + modelName + ".dylib" );
#else
	string dllPath = getPathFromUrl( fmuPath + "/binaries/linux64/" + modelName + ".so" );
#endif

	FMUCoSimulation_functions* description = new FMUCoSimulation_functions;

	string descriptionPath = getPathFromUrl( fmuPath + "/modelDescription.xml" );
	description->modelDescription = parse( descriptionPath.c_str() );

	loadDll( dllPath, description );

	modelManager_->slaveDescriptions_[modelName] = description;
	return description;
}

/**
 * consider and get the fmi-functions for a specified FMU
 * @param \in xmlPath a path to an XML description file
 * @param dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library) 
 * @param modelName the name of a model
 * @return a pointer of fmi-functions dictated to specified FMU
 */
FMUCoSimulation_functions* ModelManager::getSlave( const string& xmlPath,
						   const string& dllPath,
						   const string& modelName )
{
	// Description already available?
	SlaveDescriptions::iterator itFind = modelManager_->slaveDescriptions_.find( modelName );
	if ( itFind != modelManager_->slaveDescriptions_.end() ) { // Model name found in list of descriptions.
		return itFind->second;
	}

	string fullDllPath = getPathFromUrl( dllPath + "/" + modelName + ".dll" );

	FMUCoSimulation_functions* description = new FMUCoSimulation_functions;

	string descriptionPath = getPathFromUrl( xmlPath + "/" + modelName + ".xml" );
	description->modelDescription = parse( descriptionPath.c_str() );

	loadDll( fullDllPath, description );

	modelManager_->slaveDescriptions_[modelName] = description;
	return description;
}


/**
 * Load the given dll and set function pointers in fmu
 * 
 * @param[in] dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library) 
 * @param[out] fmuFun  a ptr to fmi functions dictated to the given FMU 
 * @return 0 if failure otherwise 1
 */ 
int ModelManager::loadDll( string dllPath, FMU_functions* fmuFun )
{
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

	fmuFun->dllHandle = h;

	// FMI for Model Exchange 1.0
	fmuFun->getModelTypesPlatform   = (fGetModelTypesPlatform) getAdr( &s, fmuFun, "fmiGetModelTypesPlatform" );
	fmuFun->instantiateModel        = (fInstantiateModel)   getAdr( &s, fmuFun, "fmiInstantiateModel" );
	fmuFun->freeModelInstance       = (fFreeModelInstance)  getAdr( &s, fmuFun, "fmiFreeModelInstance" );
	fmuFun->setTime                 = (fSetTime)            getAdr( &s, fmuFun, "fmiSetTime" );
	fmuFun->setContinuousStates     = (fSetContinuousStates)getAdr( &s, fmuFun, "fmiSetContinuousStates" );
	fmuFun->completedIntegratorStep = (fCompletedIntegratorStep)getAdr( &s, fmuFun, "fmiCompletedIntegratorStep" );
	fmuFun->initialize              = (fInitialize)         getAdr( &s, fmuFun, "fmiInitialize" );
	fmuFun->getDerivatives          = (fGetDerivatives)     getAdr( &s, fmuFun, "fmiGetDerivatives" );
	fmuFun->getEventIndicators      = (fGetEventIndicators) getAdr( &s, fmuFun, "fmiGetEventIndicators" );
	fmuFun->eventUpdate             = (fEventUpdate)        getAdr( &s, fmuFun, "fmiEventUpdate" );
	fmuFun->getContinuousStates     = (fGetContinuousStates)getAdr( &s, fmuFun, "fmiGetContinuousStates" );
	fmuFun->getNominalContinuousStates = (fGetNominalContinuousStates)getAdr( &s, fmuFun, "fmiGetNominalContinuousStates" );
	fmuFun->getStateValueReferences = (fGetStateValueReferences)getAdr( &s, fmuFun, "fmiGetStateValueReferences" );
	fmuFun->terminate               = (fTerminate)          getAdr( &s, fmuFun, "fmiTerminate" );
	fmuFun->getVersion              = (fGetVersion)         getAdr( &s, fmuFun, "fmiGetVersion" );
	fmuFun->setDebugLogging         = (fSetDebugLogging)    getAdr( &s, fmuFun, "fmiSetDebugLogging" );
	fmuFun->setReal                 = (fSetReal)            getAdr( &s, fmuFun, "fmiSetReal" );
	fmuFun->setInteger              = (fSetInteger)         getAdr( &s, fmuFun, "fmiSetInteger" );
	fmuFun->setBoolean              = (fSetBoolean)         getAdr( &s, fmuFun, "fmiSetBoolean" );
	fmuFun->setString               = (fSetString)          getAdr( &s, fmuFun, "fmiSetString" );
	fmuFun->getReal                 = (fGetReal)            getAdr( &s, fmuFun, "fmiGetReal" );
	fmuFun->getInteger              = (fGetInteger)         getAdr( &s, fmuFun, "fmiGetInteger" );
	fmuFun->getBoolean              = (fGetBoolean)         getAdr( &s, fmuFun, "fmiGetBoolean" );
	fmuFun->getString               = (fGetString)          getAdr( &s, fmuFun, "fmiGetString" );

	return s;
}


/**
 * Load the given dll and set function pointers in fmu
 * 
 * @param[in] dllPath a path to the DLL library of the unzipped FMU (It can be also a *.so library) 
 * @param[out] fmuFun  a ptr to fmi functions dictated to the given FMU 
 * @return 0 if failure otherwise 1
 */ 
int ModelManager::loadDll( string dllPath, FMUCoSimulation_functions* fmuFun )
{
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

	fmuFun->dllHandle = h;

	fmuFun->getTypesPlatform        = (fGetTypesPlatform)   getAdr( &s, fmuFun, "fmiGetTypesPlatform" );
	if ( s == 0 ) {
		s = 1; // work around bug for FMUs exported using Dymola 2012 and SimulationX 3.x
		fmuFun->getTypesPlatform    = (fGetTypesPlatform)   getAdr( &s, fmuFun, "fmiGetModelTypesPlatform" );
		if ( s == 1 ) { printf( "  using fmiGetModelTypesPlatform instead\n" ); fflush( stdout ); }
	}
	fmuFun->instantiateSlave        = (fInstantiateSlave)   getAdr( &s, fmuFun, "fmiInstantiateSlave" );
	fmuFun->initializeSlave         = (fInitializeSlave)    getAdr( &s, fmuFun, "fmiInitializeSlave" );
	fmuFun->terminateSlave          = (fTerminateSlave)     getAdr( &s, fmuFun, "fmiTerminateSlave" );
	fmuFun->resetSlave              = (fResetSlave)         getAdr( &s, fmuFun, "fmiResetSlave" );
	fmuFun->freeSlaveInstance       = (fFreeSlaveInstance)  getAdr( &s, fmuFun, "fmiFreeSlaveInstance" );
	fmuFun->setRealInputDerivatives = (fSetRealInputDerivatives) getAdr( &s, fmuFun, "fmiSetRealInputDerivatives" );
	fmuFun->getRealOutputDerivatives = (fGetRealOutputDerivatives) getAdr( &s, fmuFun, "fmiGetRealOutputDerivatives" );
	fmuFun->cancelStep              = (fCancelStep)         getAdr( &s, fmuFun, "fmiCancelStep" );
	fmuFun->doStep                  = (fDoStep)             getAdr( &s, fmuFun, "fmiDoStep" );
	fmuFun->getStatus               = (fGetStatus)          getAdr( &s, fmuFun, "fmiGetStatus" );
	fmuFun->getRealStatus           = (fGetRealStatus)      getAdr( &s, fmuFun, "fmiGetRealStatus" );
	fmuFun->getIntegerStatus        = (fGetIntegerStatus)   getAdr( &s, fmuFun, "fmiGetIntegerStatus" );
	fmuFun->getBooleanStatus        = (fGetBooleanStatus)   getAdr( &s, fmuFun, "fmiGetBooleanStatus" );
	fmuFun->getStringStatus         = (fGetStringStatus)    getAdr( &s, fmuFun, "fmiGetStringStatus" );

	fmuFun->getVersion              = (fGetVersion)         getAdr( &s, fmuFun, "fmiGetVersion" );
	fmuFun->setDebugLogging         = (fSetDebugLogging)    getAdr( &s, fmuFun, "fmiSetDebugLogging" );
	fmuFun->setReal                 = (fSetReal)            getAdr( &s, fmuFun, "fmiSetReal" );
	fmuFun->setInteger              = (fSetInteger)         getAdr( &s, fmuFun, "fmiSetInteger" );
	fmuFun->setBoolean              = (fSetBoolean)         getAdr( &s, fmuFun, "fmiSetBoolean" );
	fmuFun->setString               = (fSetString)          getAdr( &s, fmuFun, "fmiSetString" );
	fmuFun->getReal                 = (fGetReal)            getAdr( &s, fmuFun, "fmiGetReal" );
	fmuFun->getInteger              = (fGetInteger)         getAdr( &s, fmuFun, "fmiGetInteger" );
	fmuFun->getBoolean              = (fGetBoolean)         getAdr( &s, fmuFun, "fmiGetBoolean" );
	fmuFun->getString               = (fGetString)          getAdr( &s, fmuFun, "fmiGetString" );

	return s;
}


void* ModelManager::getAdr( int* s, FMU_functions *fmuFun, const char* functionName )
{
	char name[BUFSIZE];
	void* fp;
	sprintf( name, "%s_%s", getModelIdentifier( fmuFun->modelDescription ), functionName );

#if defined(MINGW)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( fmuFun->dllHandle ), name ) );
#elif defined(_MSC_VER)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( fmuFun->dllHandle ), name ) );
#else
	fp = dlsym( fmuFun->dllHandle, name );
#endif

	if ( !fp ) {
		printf ( "WARNING: Function %s not found.\n", name ); fflush( stdout );
		*s = 0; // mark dll load as 'failed'
	}

	return fp;
}


void* ModelManager::getAdr( int* s, FMUCoSimulation_functions *fmuFun, const char* functionName )
{
	char name[BUFSIZE];
	void* fp;
	sprintf( name, "%s_%s", getModelIdentifier( fmuFun->modelDescription ), functionName );

#if defined(MINGW)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( fmuFun->dllHandle ), name ) );
#elif defined(_MSC_VER)
	fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>( fmuFun->dllHandle ), name ) );
#else
	fp = dlsym( fmuFun->dllHandle, name );
#endif

	if ( !fp ) {
		printf ( "WARNING: Function %s not found.\n", name ); fflush( stdout );
		*s = 0; // mark dll load as 'failed'
	}

	return fp;
}


string ModelManager::getPathFromUrl( const string& inputFileUrl )
{
#if defined(_MSC_VER) or defined(MINGW)
	// FIXME: The following line of code may cause troubles. If so, remove it
	// and uncomment the following lines (including "delete fileUrl;" below).
	LPCTSTR fileUrl = inputFileUrl.c_str();
	// LPTSTR fileUrl = new TCHAR[inputFileUrl.size() + 1];
	// fileUrl[inputFileUrl.size()] = 0;
	// copy( inputFileUrl.begin(), inputFileUrl.end(), fileUrl );
	LPTSTR filePath = new TCHAR[MAX_PATH];
	DWORD filePathSize = inputFileUrl.size() + 1;
	DWORD tmp;
	PathCreateFromUrl( fileUrl, filePath, &filePathSize, tmp );
	// delete fileUrl;
	return string( filePath );
#else
	// FIXME: Replace with proper Linux implementation.
	if ( inputFileUrl.substr( 0, 7 ) != "file://" )
		throw invalid_argument( string( "Cannot handle URI: " ) + inputFileUrl );

	return inputFileUrl.substr( 7, inputFileUrl.size() );
#endif
}
