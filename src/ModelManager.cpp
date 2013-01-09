#include "ModelManager.h"
#include <stdio.h>

ModelManager* ModelManager::modelManager_ = 0;

ModelManager::~ModelManager()
{
  Descriptions::iterator begin= modelDescriptions_.begin();
  Descriptions::iterator end = modelDescriptions_.end();
  for(Descriptions::iterator it = begin; it != end; ++it) {
#if defined(MINGW) or defined(_MSC_VER)
    FreeLibrary( static_cast<HMODULE>(it->second->dllHandle) );
#else
    dlclose( it->second->dllHandle );
#endif
    
    freeElement( it->second->modelDescription );

    delete it->second;
  }
}


ModelManager& ModelManager::getModelManager()
{
  // Singleton instance
  static ModelManager modelManagerInstance;
  if( 0 == modelManager_ ) {
    modelManager_ = &modelManagerInstance;
  }
  return *modelManager_;
}


FMU_functions* ModelManager::getModelDescription( const std::string& modelPath, const std::string& modelName )
{
  // Description already available?
  Descriptions::iterator itFind = modelManager_->modelDescriptions_.find( modelName );
  if( itFind != modelManager_->modelDescriptions_.end() ) { // Model name found in list of descriptions.
    return itFind->second;
  }

  // fix this for other OSs and 32bit !!!
#if defined(_MSC_VER)
  std::string dllPath = modelPath + "/" + modelName + "/binaries/win64/" + modelName + ".dll";
#elif defined(MINGW)
  std::string dllPath = modelPath + "/" + modelName + "/binaries/win32/" + modelName + ".dll";
#else
  std::string dllPath = modelPath + "/" + modelName + "/binaries/linux64/" + modelName + ".so";
#endif

  FMU_functions* description = new FMU_functions;

  std::string descriptionPath = modelPath + "/" + modelName + "/modelDescription.xml";
  description->modelDescription = parse( descriptionPath.c_str() );

  loadDll( dllPath, description );

  modelManager_->modelDescriptions_[modelName] = description;
  return description;
}



/** stuff below here stolen from qtronic fmu sdk
 */

// Load the given dll and set function pointers in fmu
// Return 0 to indicate failure
int loadDll( std::string dllPath, FMU_functions* fmuFun )
{
    int s = 1;
#ifdef FMI_COSIMULATION
    int x = 1;
#endif

    // printf("dllPath = %s\n", dllPath.c_str());

#if defined(MINGW) or defined(_MSC_VER)
    HANDLE h = LoadLibrary( dllPath.c_str() );
#else
    HANDLE h = dlopen( dllPath.c_str(), RTLD_LAZY );
#endif

    if ( !h ) {
      printf( "ERROR: Could not load %s\n", dllPath.c_str() );
      return 0; // failure
    }

    fmuFun->dllHandle = h;

#ifdef FMI_COSIMULATION   
    fmuFun->getTypesPlatform        = (fGetTypesPlatform)   getAdr(&s, fmuFun, "fmiGetTypesPlatform");
    if (s==0) { 
        s = 1; // work around bug for FMUs exported using Dymola 2012 and SimulationX 3.x
        fmuFun->getTypesPlatform    = (fGetTypesPlatform)   getAdr(&s, fmuFun, "fmiGetModelTypesPlatform");
        if (s==1) printf("  using fmiGetModelTypesPlatform instead\n");
    }
    fmuFun->instantiateSlave        = (fInstantiateSlave)   getAdr(&s, fmuFun, "fmiInstantiateSlave");
    fmuFun->initializeSlave         = (fInitializeSlave)    getAdr(&s, fmuFun, "fmiInitializeSlave");    
    fmuFun->terminateSlave          = (fTerminateSlave)     getAdr(&s, fmuFun, "fmiTerminateSlave");
    fmuFun->resetSlave              = (fResetSlave)         getAdr(&s, fmuFun, "fmiResetSlave");
    fmuFun->freeSlaveInstance       = (fFreeSlaveInstance)  getAdr(&s, fmuFun, "fmiFreeSlaveInstance");
    fmuFun->setRealInputDerivatives = (fSetRealInputDerivatives) getAdr(&s, fmuFun, "fmiSetRealInputDerivatives");
    fmuFun->getRealOutputDerivatives = (fGetRealOutputDerivatives) getAdr(&s, fmuFun, "fmiGetRealOutputDerivatives");
    fmuFun->cancelStep              = (fCancelStep)         getAdr(&s, fmuFun, "fmiCancelStep");
    fmuFun->doStep                  = (fDoStep)             getAdr(&s, fmuFun, "fmiDoStep");
    // SimulationX 3.4 and 3.5 do not yet export getStatus and getXStatus: do not count this as failure here
    fmuFun->getStatus               = (fGetStatus)          getAdr(&x, fmuFun, "fmiGetStatus");
    fmuFun->getRealStatus           = (fGetRealStatus)      getAdr(&x, fmuFun, "fmiGetRealStatus");
    fmuFun->getIntegerStatus        = (fGetIntegerStatus)   getAdr(&x, fmuFun, "fmiGetIntegerStatus");
    fmuFun->getBooleanStatus        = (fGetBooleanStatus)   getAdr(&x, fmuFun, "fmiGetBooleanStatus");
    fmuFun->getStringStatus         = (fGetStringStatus)    getAdr(&x, fmuFun, "fmiGetStringStatus");    

#else // FMI for Model Exchange 1.0
    fmuFun->getModelTypesPlatform   = (fGetModelTypesPlatform) getAdr(&s, fmuFun, "fmiGetModelTypesPlatform");
    fmuFun->instantiateModel        = (fInstantiateModel)   getAdr(&s, fmuFun, "fmiInstantiateModel");
    fmuFun->freeModelInstance       = (fFreeModelInstance)  getAdr(&s, fmuFun, "fmiFreeModelInstance");
    fmuFun->setTime                 = (fSetTime)            getAdr(&s, fmuFun, "fmiSetTime");
    fmuFun->setContinuousStates     = (fSetContinuousStates)getAdr(&s, fmuFun, "fmiSetContinuousStates");
    fmuFun->completedIntegratorStep = (fCompletedIntegratorStep)getAdr(&s, fmuFun, "fmiCompletedIntegratorStep");
    fmuFun->initialize              = (fInitialize)         getAdr(&s, fmuFun, "fmiInitialize");
    fmuFun->getDerivatives          = (fGetDerivatives)     getAdr(&s, fmuFun, "fmiGetDerivatives");
    fmuFun->getEventIndicators      = (fGetEventIndicators) getAdr(&s, fmuFun, "fmiGetEventIndicators");
    fmuFun->eventUpdate             = (fEventUpdate)        getAdr(&s, fmuFun, "fmiEventUpdate");
    fmuFun->getContinuousStates     = (fGetContinuousStates)getAdr(&s, fmuFun, "fmiGetContinuousStates");
    fmuFun->getNominalContinuousStates = (fGetNominalContinuousStates)getAdr(&s, fmuFun, "fmiGetNominalContinuousStates");
    fmuFun->getStateValueReferences = (fGetStateValueReferences)getAdr(&s, fmuFun, "fmiGetStateValueReferences");
    fmuFun->terminate               = (fTerminate)          getAdr(&s, fmuFun, "fmiTerminate");
#endif 
    fmuFun->getVersion              = (fGetVersion)         getAdr(&s, fmuFun, "fmiGetVersion");
    fmuFun->setDebugLogging         = (fSetDebugLogging)    getAdr(&s, fmuFun, "fmiSetDebugLogging");
    fmuFun->setReal                 = (fSetReal)            getAdr(&s, fmuFun, "fmiSetReal");
    fmuFun->setInteger              = (fSetInteger)         getAdr(&s, fmuFun, "fmiSetInteger");
    fmuFun->setBoolean              = (fSetBoolean)         getAdr(&s, fmuFun, "fmiSetBoolean");
    fmuFun->setString               = (fSetString)          getAdr(&s, fmuFun, "fmiSetString");
    fmuFun->getReal                 = (fGetReal)            getAdr(&s, fmuFun, "fmiGetReal");
    fmuFun->getInteger              = (fGetInteger)         getAdr(&s, fmuFun, "fmiGetInteger");
    fmuFun->getBoolean              = (fGetBoolean)         getAdr(&s, fmuFun, "fmiGetBoolean");
    fmuFun->getString               = (fGetString)          getAdr(&s, fmuFun, "fmiGetString");
    return s; 
}


extern "C" void* getAdr( int* s, FMU_functions *fmuFun, const char* functionName )
{
  char name[BUFSIZE];
  void* fp;
  sprintf( name, "%s_%s", getModelIdentifier( fmuFun->modelDescription ), functionName );

#if defined(MINGW) or defined(_MSC_VER)
  fp = reinterpret_cast<void*>( GetProcAddress( static_cast<HMODULE>(fmuFun->dllHandle), name ) );
#else
  fp = dlsym( fmuFun->dllHandle, name );
#endif

  if ( !fp ) {
    printf ( "WARNING: Function %s not found.\n", name );
    *s = 0; // mark dll load as 'failed'        
  }

  return fp;
}
