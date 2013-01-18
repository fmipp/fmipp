#ifdef FMI_DEBUG
#include <iostream>
#endif

#include <cassert>

#include "FMU.h"
#include "ModelManager.h"
#include "FMUIntegrator.h"

static  fmiCallbackFunctions functions = { FMU::logger, calloc, free };


using namespace std;


FMU::FMU( const string& modelName )
{
#ifdef FMI_DEBUG
  cout << "[FMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif 

  ModelManager& manager = ModelManager::getModelManager();
  fmuFun_ = manager.getModel("./", modelName);
  readModelDescription();

  integrator_ = new FMUIntegrator( this, FMUIntegrator::dp );

#ifdef FMI_DEBUG
  cout << "[FMU::ctor] DONE." << endl; fflush( stdout );
#endif 
}


FMU::FMU( const string& fmuPath,
	  const string& modelName )
{
#ifdef FMI_DEBUG
  cout << "[FMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif 

  ModelManager& manager = ModelManager::getModelManager();
  fmuFun_ = manager.getModel(fmuPath, modelName);
  readModelDescription();

  integrator_ = new FMUIntegrator( this, FMUIntegrator::dp );

#ifdef FMI_DEBUG
  cout << "[FMU::ctor] DONE." << endl;
#endif 
}

FMU::FMU( const string& xmlPath,
	  const std::string& dllPath,
	  const string& modelName )
{
#ifdef FMI_DEBUG
  cout << "[FMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif 

  ModelManager& manager = ModelManager::getModelManager();
  fmuFun_ = manager.getModel( xmlPath, dllPath, modelName );
  readModelDescription();

  integrator_ = new FMUIntegrator( this, FMUIntegrator::dp );

#ifdef FMI_DEBUG
  cout << "[FMU::ctor] done." << endl;
#endif 
}


FMU::FMU( const FMU& aFMU )
{
#ifdef FMI_DEBUG
  cout << "[FMU::ctor]" << endl; fflush( stdout );
#endif 

  fmuFun_ = aFMU.fmuFun_;

  integrator_ = new FMUIntegrator( this, aFMU.integrator_->type() );

  nStateVars_ = aFMU.nStateVars_;
  nEventInds_ = aFMU.nEventInds_;

  valueRefs_ = aFMU.valueRefs_;
  varMap_ = aFMU.varMap_;

  nValueRefs_ = aFMU.nValueRefs_;
  valueRefsPtr_ = &valueRefs_.front();

#ifdef FMI_DEBUG
  cout << "[FMU::ctor] DONE." << endl; fflush( stdout );
#endif 
}


FMU::~FMU()
{
  //delete ncstates_;
  //delete storedv_;

  delete integrator_;

  if(instance_) {
    delete[] cstates_;
    delete[] derivatives_;
    delete[] eventsind_;
    delete[] preeventsind_;
    delete eventinfo_;

    fmuFun_->terminate( instance_ );
    fmuFun_->freeModelInstance( instance_ );
  }
}


void FMU::readModelDescription() {
  nStateVars_ = getNumberOfStates(fmuFun_->modelDescription);
  nEventInds_ = getNumberOfEventIndicators(fmuFun_->modelDescription);

  for(size_t i = 0; fmuFun_->modelDescription->modelVariables[i]; ++i) {
    ScalarVariable* var = (ScalarVariable*) fmuFun_->modelDescription->modelVariables[i];
    valueRefs_.push_back(getValueReference(var));
    varMap_.insert(pair<string,fmiValueReference>(getString(var,att_name),valueRefs_[i]));
  }

  nValueRefs_ = valueRefs_.size();
  valueRefsPtr_ = &valueRefs_.front();
}


fmiStatus FMU::instantiate(const string& instanceName, fmiBoolean loggingOn)
{
  instanceName_ = instanceName;

  if(fmuFun_ == 0) {
    return fmiError;
  }

#ifdef FMI_DEBUG
  // General information ... 
  cout << "[FMU::instantiate] Types Platform: " << fmuFun_->getModelTypesPlatform()  << ", FMI Version:  " << fmuFun_->getVersion() << endl; fflush( stdout );
#endif

  // Basic settings: @todo from a menu.
  //nsteps_ = 2;
  time_ = 0.;
  tnextevent_ = 1e50;
  // nStateEvents_ = 0; 
  // nTimeEvents_ = 0; 
  // nCallEventUpdate_ = 0; 
  // maxEventIterations_ = 5;

  // Memory allocation.
#ifdef FMI_DEBUG
  cout << "[FMU::instantiate] nStateVars_ = " << nStateVars_ << " -  nEventInds_ = " << nEventInds_ << endl; fflush( stdout );
#endif

  cstates_      = new fmiReal[nStateVars_];
  derivatives_  = new fmiReal[nStateVars_];
  eventsind_    = new fmiReal[nEventInds_];
  preeventsind_ = new fmiReal[nEventInds_];

  for(size_t i = 0; i < nEventInds_; ++i ) {
    eventsind_[i] = 0;
    preeventsind_[i] = 0;
  }

  eventinfo_ = new fmiEventInfo;

  // Instantiation of the model: @todo from menu.
  // get this right ;) !!!
  const char* guid = getString( fmuFun_->modelDescription, att_guid );

#ifdef FMI_DEBUG
  cout << "[FMU::instantiate] GUID = " << guid << endl; fflush( stdout );
  cout << "[FMU::instantiate] instanceName = " << instanceName_ << endl; fflush( stdout );
#endif

  instance_ = fmuFun_->instantiateModel( instanceName_.c_str(), guid, functions, fmiTrue );

  if(0 == instance_) {
#ifdef FMI_DEBUG
  cout << "[FMU::instantiate] instantiateModel failed. " << endl; fflush( stdout );
#endif
    return fmiError;
  }

#ifdef FMI_DEBUG
  cout << "[FMU::instantiate] instance_ = " << instance_ << endl; fflush( stdout );
#endif

  fmiStatus status = fmuFun_->setDebugLogging( instance_, loggingOn );

  if(loggingOn) {
	  functions.logger(instance_, instanceName_.c_str(), status, "?", "Model instance initialized"); fflush( stdout );
  }

#ifdef FMI_DEBUG
  cout << "[FMU::instantiate] DONE. status = " << status << endl; fflush( stdout );
#endif

  return status;
}


fmiStatus FMU::initialize()
{
  if(0 == instance_) {
    return fmiError;
  }

  // Basic settings.
  fmuFun_->setTime(instance_, time_); 
  fmiStatus status = fmuFun_->initialize(instance_, fmiFalse, 1e-5, eventinfo_);

  //fmiGetReal( instance_, description_->valueRefsPtr_, description_->nValueRefs_, storedv_ );
  fmuFun_->getContinuousStates(instance_, cstates_ , nStateVars_);

  stateEvent_ = fmiFalse;
  timeEvent_ = fmiFalse;
  callEventUpdate_ = fmiFalse;

  return status;
}


fmiStatus FMU::setValue(size_t ivar, fmiReal* val)
{
  return fmuFun_->setReal(instance_, &valueRefsPtr_[ivar], 1, val);
}


fmiStatus FMU::setValue(size_t ivar, fmiReal val)
{
  return fmuFun_->setReal(instance_, &valueRefsPtr_[ivar], 1, &val);
}


fmiStatus FMU::setValue(size_t ivar, fmiBoolean* val)
{
  return fmuFun_->setBoolean(instance_, &valueRefsPtr_[ivar], 1, val);
}


fmiStatus FMU::setValue(const string& name,  fmiReal val)
{
  map<string,fmiValueReference>::const_iterator it = varMap_.find(name);

  if(it != varMap_.end()) { 
    return fmuFun_->setReal(instance_,&it->second,1,&val); 
  } else {
    string ret = name + string(" does not exist"); 
    logger(fmiDiscard,ret);
    return fmiDiscard; 
  }
}


fmiStatus FMU::getContinuousStates(fmiReal* val) const
{
  return fmuFun_->getContinuousStates(instance_, val, nStateVars_);
}


fmiStatus FMU::getValue(size_t ivar, fmiReal* val) const
{
  return fmuFun_->getReal(instance_, &valueRefsPtr_[ivar], 1, val);
}


fmiStatus FMU::getValue(size_t ivar, fmiBoolean* val) const
{
  return fmuFun_->getBoolean(instance_, &valueRefsPtr_[ivar], 1, val);
}


fmiStatus FMU::getValue(const string& name,  fmiReal* val) const
{
  map<string,fmiValueReference>::const_iterator it = varMap_.find(name); 
  //printf("%s : %d\n",it->first,it->second);
  if(it != varMap_.end()) { 
    return fmuFun_->getReal(instance_,&it->second,1,val); 
  } else {
    string ret = name + string(" does not exist"); 
    logger(fmiDiscard,ret);
    return fmiDiscard; 
  }
}


size_t FMU::getValueRef(const string& name) const {
  map<string,fmiValueReference>::const_iterator it = varMap_.find(name); 
  if(it != varMap_.end()) { 
    return it->second; 
  } else {
    return fmiUndefinedValueReference;
  }
}


fmiStatus FMU::getEventIndicators(fmiReal* eventsind) {
  fmiStatus status = fmuFun_->getEventIndicators(instance_, eventsind, nEventInds());
  return status;
} 


fmiStatus FMU::integrate(fmiReal tstop, unsigned int nsteps) {
  assert( nsteps > 0 ); 
  double deltaT = (tstop - time_) / nsteps; 
  return integrate( tstop, deltaT ); 
}


fmiStatus FMU::integrate(fmiReal tstop, double deltaT)
{
  assert( deltaT > 0 ); 
  handleEvents( 0, false );

  fmiStatus status = fmiOK;

  integrator_->integrate( ( tstop - time_ ), ( tstop - time_ )/deltaT );

  setTime( tstop );

  return status;
}
 

void FMU::handleEvents( fmiTime tStop, bool completedIntegratorStep)
{
  // Get event indicators.
  size_t nx = nStateVars_; // Number of state variables.
  size_t nz = nEventInds_; // Number of event indicators.
  size_t nvals = nValueRefs_; // Number of value references.

  for( size_t i = 0; i < nz; ++i ) preeventsind_[i] = eventsind_[i];

  //fmiGetEventIndicators( instance_, eventsind_, nz ); 
  getEventIndicators(eventsind_);
  
  for( size_t i = 0; i < nz; ++i ) stateEvent_ = stateEvent_ || (preeventsind_[i] * eventsind_[i] < 0);

  timeEvent_ = ( time_ > tnextevent_ ); // abs(time - tnextevent_) <= EPS ; 

  // Inform the model about an accepted step.
  if( true == completedIntegratorStep ) fmuFun_->completedIntegratorStep(instance_, &callEventUpdate_);

  // nTimeEvents_ += timeEvent_ ? 1 : 0;
  // nStateEvents_ += stateEvent_ ? 1 : 0;
  // nCallEventUpdate_ += callEventUpdate_ ? 1 : 0;

#ifdef FMI_DEBUG
  if( callEventUpdate_ || stateEvent_ || timeEvent_ )
    cout << "[FMU::handleEvents] An event occured: "
	      << "  event_update : " << callEventUpdate_
	      << " , stateEvent : "  << stateEvent_
	      << " , timeEvent : "  << timeEvent_ << endl;  fflush( stdout );
#endif 

  if( callEventUpdate_ || stateEvent_ || timeEvent_ ) {
    eventinfo_->iterationConverged = fmiFalse; 
      
    // Event time is identified and stored values get updated. 
    unsigned int cnt = 0; 
    while( (fmiFalse == eventinfo_->iterationConverged) && (cnt < maxEventIterations_) )
    { 
      fmuFun_->eventUpdate( instance_, fmiTrue, eventinfo_ ); 

      // If intermediate results need to be set. 
      // if( fmiFalse == eventinfo_->iterationConverged ) { 
      // 	fmiStatus status = fmiGetReal( instance_, description_->valueRefsPtr_,
      // 				       description_->nValueRefs_, storedv_ );
      // }
      //showArray( "Event Results", storedv_, nvals );  

      cnt++;
    }

    // The values of the continuous states are retained.
    if( (fmiTrue == eventinfo_->stateValuesChanged) || (true == stateEvent_) ) {
      fmuFun_->getContinuousStates( instance_, cstates_ , nStateVars_ );
    }

    // // Nominal values might have changed.
    // if( eventinfo_->stateValueReferencesChanged == fmiTrue )
    //   fmiGetNominalContinuousStates( instance_, ncstates_, description_->??? );

    // Next time event is identified.
    if( eventinfo_->upcomingTimeEvent ) {
      tnextevent_ = (eventinfo_->nextEventTime < tStop) ? eventinfo_->nextEventTime : tStop;
    }
    stateEvent_ = fmiFalse;
  }
}


void FMU::logger( fmiStatus status, const string& msg ) const
{
  functions.logger( instance_, instanceName_.c_str(), status, "?", msg.c_str() );
}


void FMU::logger( fmiStatus status, const char* msg ) const
{
  functions.logger( instance_, instanceName_.c_str(), status, "?", msg );
}


void FMU::logger( fmiComponent m, fmiString instanceName,
		  fmiStatus status, fmiString category,
		  fmiString message, ... ) 
{
	char msg[4096];
	char buf[4096];
	int len;
	int capacity;

	va_list ap;
	va_start(ap, message);
	capacity = sizeof(buf) - 1;
#if defined(_MSC_VER) && _MSC_VER>=1400
	len = _snprintf_s(msg, capacity, _TRUNCATE, "%s: %s", instanceName, message);
	if (len < 0) goto fail;
	len = vsnprintf_s(buf, capacity, _TRUNCATE, msg, ap);
	if (len < 0) goto fail;
#elif defined(WIN32)
	len = _snprintf(msg, capacity, "%s: %s", instanceName, message);
	if (len < 0) goto fail;
	len = vsnprintf(buf, capacity, msg, ap);
	if (len < 0) goto fail;
#else
	len = snprintf(msg, capacity, "%s: %s", instanceName, message);
	if (len < 0) goto fail;
	len = vsnprintf(buf, capacity, msg, ap);
	if (len < 0) goto fail;
#endif
	/* append line break */
	buf[len] = '\n';
	buf[len + 1] = 0;
	va_end(ap);

	switch (status) {
		case fmiFatal:
			printf(buf);
			break;
		default:
			printf(buf);
			break;
	}
	return;

fail:
	printf("logger failed, message too long?");
}
