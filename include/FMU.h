#ifndef _FMU_H
#define _FMU_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string>
#include <vector>
#include <map>

extern "C"
{
#include "fmi_me.h"
}

extern "C" __FMI_DLL void logger(fmiComponent m, fmiString instanceName, fmiStatus status, fmiString category, fmiString message, ...);

static  fmiCallbackFunctions functions = { logger, calloc, free };


class FMUIntegrator;

/** 
 *  The FMI standard requires to define the macro MODEL_IDENTIFIER for each
 *  type of FMU seperately. This is not done here, because this class links
 *  dynamically during run-time.
 */


class __FMI_DLL FMU
{

public:
  FMU(const std::string& modelPath, const std::string& modelName);
  FMU(const std::string& modelName);
  FMU(const FMU& aFMU);
  ~FMU();

  // Instantiate the FMU
  fmiStatus instantiate(const std::string& instanceName, fmiBoolean loggingOn = fmiFalse);

  fmiStatus initialize();

  void raiseEvent() { stateEvent_ = fmiTrue; } 

  const fmiReal& getTime() const { return time_; } 
  void setTime( fmiReal time ) { time_ = time; fmuFun_->setTime( instance_, time_ ); }
  void rewindTime( fmiReal deltaRewindTime ) { time_ -= deltaRewindTime; fmuFun_->setTime( instance_, time_ ); }//fmuFun_->eventUpdate(instance_, fmiFalse, eventinfo_);}

  fmiStatus setValue(std::size_t ivar, fmiReal* val);
  fmiStatus setValue(std::size_t ivar, fmiReal val);
  fmiStatus setValue(std::size_t ivar, fmiBoolean* val);

  /** 
   * Set the value of a variable with
   * Note it is the client responsibility to ensure that this is an allowed operation 
   * @param name the name of a variable 
   * @param val  the desired value 
   * @return the status of the fmi operation 
   */ 
  fmiStatus setValue(const std::string& name,  fmiReal val); 

  fmiStatus getValue(std::size_t ivar, fmiReal* val) const;
  fmiStatus getValue(std::size_t ivar, fmiBoolean* val) const;
  fmiStatus getValue(const std::string& name,  fmiReal* val) const; 
  std::size_t getValueRef(const std::string& name) const;

  fmiStatus getContinuousStates(fmiReal* val) const;
  inline fmiStatus setContinuousStates(fmiReal* val) { fmuFun_->setContinuousStates(instance_, val, nStateVars_); }
  /**
   * get the values of the events indeciator
   * @param eventsind array of size nEventsInd() initialized by the caller
   * @return status of the internal fmi operation 
   */ 
  fmiStatus getEventIndicators(fmiReal* eventsind); 
  
  fmiStatus integrate(fmiReal tend, unsigned int nsteps);
  fmiStatus integrate(fmiReal tend, double deltaT=1E-5);

  void handleEvents(fmiReal tstop, bool completedIntegratorStep);

  inline std::size_t nStates() { return nStateVars_; }
  inline std::size_t nEventInds() { return nEventInds_; }
  inline std::size_t nValueRefs() { return nValueRefs_; }

  inline fmiReal* getCStates() { return cstates_; }

  void logger(fmiStatus status, const std::string& msg) const;
  void logger(fmiStatus status, const char* msg) const;

private:

  friend class FMUIntegrator;

  /**  prevent calling the default constructor */ 
  FMU();

  std::string instanceName_;

  FMU_functions *fmuFun_;

  FMUIntegrator* integrator_;

  int loadFMU();
  int loadDll(std::string dllPath);

  void readModelDescription();

  std::size_t nStateVars_; // Number of state variables.
  std::size_t nEventInds_; // Number of event indivators.
  std::size_t nValueRefs_; // Number of value references.

  std::vector<fmiValueReference> valueRefs_; // Vector of value references.
  std::map<std::string,fmiValueReference> varMap_; 
  fmiValueReference* valueRefsPtr_;          // Pointer to first value reference.

  //fmiReal* storedv_;
  fmiReal* cstates_;
  //fmiReal* ncstates_;
  fmiReal* derivatives_;
  fmiReal  time_;
  fmiReal  tnextevent_;

  fmiEventInfo* eventinfo_; 
  fmiReal*      eventsind_;
  fmiReal*      preeventsind_;

  // unsigned int nStateEvents_;
  // unsigned int nTimeEvents_;
  // unsigned int nCallEventUpdate_;

  fmiComponent instance_;
	
  fmiBoolean callEventUpdate_;
  fmiBoolean stateEvent_;
  fmiBoolean timeEvent_;

  //unsigned int nsteps_; 

  //  const static int maxEventIterations_ = 5;
  static const int maxEventIterations_ = 5;
};

#endif
