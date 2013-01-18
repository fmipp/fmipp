#ifndef __ModelManager_H
#define __ModelManager_H

#include <string>
#include <map>

#include "fmi_me.h"

#define BUFSIZE 4096

extern "C" void* getAdr(int* s, FMU_functions *fmuFun, const char* functionName);

int loadDll(std::string dllPath, FMU_functions* fmuFun);


class ModelManager {

public:

  // Destructor.
  ~ModelManager();

  // Get singleton instance of model manager.
  static ModelManager& getModelManager();

  // Get model (from standard unzipped FMU).
  static FMU_functions* getModel( const std::string& fmuPath,
				  const std::string& modelName );

  // Get model (from non-standard 'modelName.xml' and 'modelName.dll'
  static FMU_functions* getModel( const std::string& xmlPath,
				  const std::string& dllPath,
				  const std::string& modelName );


 private:

  // Private constructor (singleton).
  ModelManager() {}

  // Pointer to singleton instance
  static ModelManager* modelManager_;

  // Define container for description collection.
  typedef std::map<std::string, FMU_functions*> Descriptions;

  // Collection of descriptions.
  Descriptions modelDescriptions_;
};


#endif
