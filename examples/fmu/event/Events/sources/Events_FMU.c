
// define class name and unique id
#define MODEL_IDENTIFIER Events
#define MODEL_GUID "{8c4e810f-3df3-4a00-8276-176fa3c9f9e0}"

// include fmu header files, typedefs and macros
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "openmodelica.h"
#include "openmodelica_func.h"
#include "simulation_data.h"
#include "omc_error.h"
#include "fmiModelTypes.h"
#include "fmiModelFunctions.h"
#include "Events_functions.h"
#include "initialization.h"
#include "events.h"
#include "fmu_model_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

void setStartValues(ModelInstance *comp);
void setDefaultStartValues(ModelInstance *comp);
void eventUpdate(ModelInstance* comp, fmiEventInfo* eventInfo);
fmiReal getReal(ModelInstance* comp, const fmiValueReference vr);
fmiStatus setReal(ModelInstance* comp, const fmiValueReference vr, const fmiReal value);
fmiInteger getInteger(ModelInstance* comp, const fmiValueReference vr);
fmiStatus setInteger(ModelInstance* comp, const fmiValueReference vr, const fmiInteger value);
fmiBoolean getBoolean(ModelInstance* comp, const fmiValueReference vr);
fmiStatus setBoolean(ModelInstance* comp, const fmiValueReference vr, const fmiBoolean value);
fmiString getString(ModelInstance* comp, const fmiValueReference vr);
fmiStatus setExternalFunction(ModelInstance* c, const fmiValueReference vr, const void* value);

// define model size
#define NUMBER_OF_STATES 1
#define NUMBER_OF_EVENT_INDICATORS 1
#define NUMBER_OF_REALS 4
#define NUMBER_OF_INTEGERS 0
#define NUMBER_OF_STRINGS 0
#define NUMBER_OF_BOOLEANS 0
#define NUMBER_OF_EXTERNALFUNCTIONS 0

// define variable data for model
#define $Px_ 0 
#define $P$DER$Px_ 1 
#define $Pz_ 2 
#define $Pk_ 3 


// define initial state vector as vector of value references
#define STATES { $Px_ }
#define STATESDERIVATIVES { $P$DER$Px_ }


// implementation of the Model Exchange functions
#include "fmu_model_interface.c"

// Set values for all variables that define a start value
void setDefaultStartValues(ModelInstance *comp) {

comp->fmuData->modelData.realVarsData[0].attribute.start = 1.0;
comp->fmuData->modelData.realVarsData[1].attribute.start = 0;
comp->fmuData->modelData.realVarsData[2].attribute.start = 0;
comp->fmuData->modelData.realParameterData[0].attribute.start = 10.0;
}
// Set values for all variables that define a start value
void setStartValues(ModelInstance *comp) {

  comp->fmuData->modelData.realVarsData[0].attribute.start =  comp->fmuData->localData[0]->realVars[0];
  comp->fmuData->modelData.realVarsData[1].attribute.start =  comp->fmuData->localData[0]->realVars[1];
  comp->fmuData->modelData.realVarsData[2].attribute.start =  comp->fmuData->localData[0]->realVars[2];
comp->fmuData->modelData.realParameterData[0].attribute.start = comp->fmuData->simulationInfo.realParameter[0];
}
// Used to set the next time event, if any.
void eventUpdate(ModelInstance* comp, fmiEventInfo* eventInfo) {
}

fmiReal getReal(ModelInstance* comp, const fmiValueReference vr) {
  switch (vr) {
      case $Px_ : return comp->fmuData->localData[0]->realVars[0]; break;
      case $P$DER$Px_ : return comp->fmuData->localData[0]->realVars[1]; break;
      case $Pz_ : return comp->fmuData->localData[0]->realVars[2]; break;
      case $Pk_ : return comp->fmuData->simulationInfo.realParameter[0]; break;
      default:
          return fmiError;
  }
}

fmiStatus setReal(ModelInstance* comp, const fmiValueReference vr, const fmiReal value) {
  switch (vr) {
      case $Px_ : comp->fmuData->localData[0]->realVars[0]=value; break;
      case $P$DER$Px_ : comp->fmuData->localData[0]->realVars[1]=value; break;
      case $Pz_ : comp->fmuData->localData[0]->realVars[2]=value; break;
      case $Pk_ : comp->fmuData->simulationInfo.realParameter[0]=value; break;
      default:
          return fmiError;
  }
  return fmiOK;
}

fmiInteger getInteger(ModelInstance* comp, const fmiValueReference vr) {
  switch (vr) {
      default:
          return 0;
  }
}
fmiStatus setInteger(ModelInstance* comp, const fmiValueReference vr, const fmiInteger value) {
  switch (vr) {
      default:
          return fmiError;
  }
  return fmiOK;
}
fmiBoolean getBoolean(ModelInstance* comp, const fmiValueReference vr) {
  switch (vr) {
      default:
          return 0;
  }
}

fmiStatus setBoolean(ModelInstance* comp, const fmiValueReference vr, const fmiBoolean value) {
  switch (vr) {
      default:
          return fmiError;
  }
  return fmiOK;
}

fmiString getString(ModelInstance* comp, const fmiValueReference vr) {
  switch (vr) {
      default:
          return 0;
  }
}

fmiStatus setExternalFunction(ModelInstance* c, const fmiValueReference vr, const void* value){
  switch (vr) {
      default:
          return fmiError;
  }
  return fmiOK;
}


#ifdef __cplusplus
}
#endif

