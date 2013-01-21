#ifndef _FMU_H
#define _FMU_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string>
#include <vector>
#include <map>

#include "FMIPPConfig.h"
#include "fmi_me.h"


class FMUIntegrator;

/** 
 *  The FMI standard requires to define the macro MODEL_IDENTIFIER for each
 *  type of FMU seperately. This is not done here, because this class links
 *  dynamically during run-time.
 */


class __FMI_DLL FMU
{

public:

  FMU( const std::string& modelName );

  FMU( const std::string& fmuPath,
       const std::string& modelName );

  FMU( const std::string& xmlPath,
       const std::string& dllPath,
       const std::string& modelName );

  FMU(const FMU& aFMU);

  ~FMU();

  // Instantiate the FMU
  fmiStatus instantiate(const std::string& instanceName, fmiBoolean loggingOn = fmiFalse);

  fmiStatus initialize();

  const fmiReal& getTime() const;
  void setTime( fmiReal time );
  void rewindTime( fmiReal deltaRewindTime );

  fmiStatus setValue(fmiValueReference valref, fmiReal& val);
  fmiStatus setValue(fmiValueReference* valref, fmiReal* val, std::size_t ival);

  //fmiStatus setValue(fmiValueReference valref, fmiBoolean& val);

  fmiStatus setValue(const std::string& name,  fmiReal val); 

  fmiStatus getValue(fmiValueReference valref, fmiReal& val) const;
  fmiStatus getValue(fmiValueReference* valref, fmiReal* val, std::size_t ival) const;

  //fmiStatus getValue(fmiValueReference valref, fmiBoolean& val) const;

  fmiStatus getValue(const std::string& name,  fmiReal& val) const; 

  fmiValueReference getValueRef(const std::string& name) const;

  fmiStatus getContinuousStates( fmiReal* val ) const;
  fmiStatus setContinuousStates( const fmiReal* val );

  fmiStatus getDerivatives( fmiReal* val ) const;

  fmiStatus getEventIndicators(fmiReal* eventsind); 
  
  fmiStatus integrate(fmiReal tend, unsigned int nsteps);
  fmiStatus integrate(fmiReal tend, double deltaT=1E-5);

  void raiseEvent() { stateEvent_ = fmiTrue; } 
  void handleEvents( fmiTime tstop, bool completedIntegratorStep );

  inline std::size_t nStates() { return nStateVars_; }
  inline std::size_t nEventInds() { return nEventInds_; }
  inline std::size_t nValueRefs() { return nValueRefs_; }

  void logger(fmiStatus status, const std::string& msg) const;
  void logger(fmiStatus status, const char* msg) const;


  static void logger( fmiComponent m, fmiString instanceName,
		      fmiStatus status, fmiString category,
		      fmiString message, ... );

private:

  friend class FMUIntegrator;

  /**  prevent calling the default constructor */ 
  FMU();

  std::string instanceName_;

  fmiComponent instance_;

  FMU_functions *fmuFun_;

  FMUIntegrator* integrator_;

  std::size_t nStateVars_; // Number of state variables.
  std::size_t nEventInds_; // Number of event indivators.
  std::size_t nValueRefs_; // Number of value references.

  //std::vector<fmiValueReference> valueRefs_; // Vector of value references.
  std::map<std::string,fmiValueReference> varMap_; 
  //fmiValueReference* valueRefsPtr_;          // Pointer to first value reference.

  //fmiReal* storedv_;
  //fmiReal* cstates_;
  //fmiReal* ncstates_;
  //fmiReal* derivatives_;
  fmiReal  time_;
  fmiReal  tnextevent_;

  fmiEventInfo* eventinfo_; 
  fmiReal*      eventsind_;
  fmiReal*      preeventsind_;

  // unsigned int nStateEvents_;
  // unsigned int nTimeEvents_;
  // unsigned int nCallEventUpdate_;
	
  fmiBoolean callEventUpdate_;
  fmiBoolean stateEvent_;
  fmiBoolean timeEvent_;

  void readModelDescription();

  static const int maxEventIterations_ = 5;

};

#endif
