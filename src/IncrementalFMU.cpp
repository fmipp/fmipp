#include "IncrementalFMU.h"

IncrementalFMU::IncrementalFMU(const std::string& name, const std::string inputs[], const std::size_t nInputs, const std::string outputs[], const std::size_t nOutputs) {
  fmu_ = new FMU(name);
  setInputs(inputs, nInputs);
  setOutputs(outputs, nOutputs);
  allocevmem();
}



IncrementalFMU::IncrementalFMU(const std::string& name, std::size_t nInputs, std::size_t nOutputs) {
  fmu_ = new FMU(name);
  nInputs_ = nInputs;
  nOutputs_ = nOutputs;
  allocevmem();
}


IncrementalFMU::IncrementalFMU(const IncrementalFMU& aIncrementalFMU) {
  fmu_ = new FMU(*(aIncrementalFMU.fmu_));
  nInputs_ = aIncrementalFMU.nInputs_;
  nOutputs_ = aIncrementalFMU.nOutputs_;
  allocevmem();
}


void IncrementalFMU::setInputs(const std::string inputs[], const std::size_t nInputs) {
  nInputs_ = nInputs;
  inputs_ = new std::size_t[nInputs];
  for(std::size_t i = 0; i < nInputs; i++) {
    inputs_[i] = fmu_->getValueRef(inputs[i]);
  }
}


void IncrementalFMU::setOutputs(const std::string outputs[], const std::size_t nOutputs) {
  nOutputs_ = nOutputs;
  outputs_ = new std::size_t[nOutputs];
  for(std::size_t i = 0; i < nOutputs; i++) {
    outputs_[i] = fmu_->getValueRef(outputs[i]);
  }
}


bool IncrementalFMU::checkForEvent()
{
  for(std::size_t i = 0; i < fmu_->nEventInds(); i++) {
    if(eventindspos_[i] ^ (eventinds_[i] > 0)) {
	return true;
    }
  }
  return false;
}


void IncrementalFMU::initialInputs(const std::string variableNames[], const fmiReal* values, std::size_t nvars)
{
  for(std::size_t i = 0; i < nvars; i++) {
    fmu_->setValue(variableNames[i], values[i]);
  }
}


void IncrementalFMU::initializeIntegration(fmiReal* initialState)
{
  for(std::size_t i = 0; i < fmu_->nStates(); i++) {
    fmu_->setValue(i, initialState[i]);
  }
  // find out why this version works and the version below doesn't !!!
  // fmu_->setContinuousStates(initialState);
}


fmiReal* IncrementalFMU::initialState() const
{
  fmiReal* initState = new fmiReal[fmu_->nStates()];
  fmu_->getContinuousStates(initState);
  /*
  for(std::size_t i = 0; i < fmu_->nStates(); i++) {
    fmu_->getValue(i, &initState[i]);
  }
  */
  return initState;
}


int IncrementalFMU::init(const std::string& instanceName, const std::string variableNames[], const fmiReal* values, std::size_t nvars, const TIMESTAMP startTime, const TIMESTAMP horizon, const TIMESTAMP stepsize )
{
  /*
  // Intstantiate FMU.
#ifdef FMI_DEBUG
  if (fmu_->instantiate(fmiTrue) != fmiOK) return 0;
#elsif
  if (fmu_->instantiate(fmiFalse) != fmiOK) return 0;
#endif
  */
  /* have a look why this works and the above version does not !!! */
  fmiStatus status = fmu_->instantiate(instanceName, fmiFalse);
  if(status != fmiOK) return 0;


  // Set inputs (has to happen before initialization of FMU).
  initialInputs(variableNames, values, nvars);

  // Intialize FMU.
  if ( fmu_->initialize() != fmiOK ) return 0;

  // Define the initial state: The initial state might include guesses. In such
  // cases we have to raise an event (and iterate over fmiEventUpdate) until the
  // FMU has found a solution ...
  fmiReal* v = new fmiReal[nOutputs_];
  fmiReal* s = initialState();
  HistoryEntry init(startTime, s, fmu_->nStates(), v, nOutputs_);
  delete [] v;
  delete [] s;
  initializeIntegration( init.state ); // Set values (but don't integrate afterwards) ...
  fmu_->raiseEvent(); // ... then raise an event ...
  //  fmu_->handleEvents( 0., false ); // ... and finally take proper actions.
  fmu_->handleEvents(startTime, false); // ... and finally take proper actions.
  retrieveIntegrationResults(init.state, init.values, eventinds_); // Then retrieve the result and ...
  predictions_.push_back(init); // ... store as prediction -> will be used by first call to updateState().

  lookAheadHorizon_ = horizon;
  integratorStepSize_ = stepsize;

  return 1;  /* return 1 on success, 0 on failure */
}


/* In case no look-ahead prediction is given for time t, this function is responsible to provide
 * an estimate for the corresponding state. For convenience, a REVERSE iterator pointing to the
 * next prediction available AFTER time t is handed over to the function.
 */
void IncrementalFMU::interpolateState( TIMESTAMP t, History_const_reverse_iterator& historyEntry, IncrementalFMU::HistoryEntry& result)
{
  const HistoryEntry& right = *(historyEntry-1);
  const HistoryEntry& left = *(historyEntry);

  for(std::size_t i = 0; i < fmu_->nStates(); i++) {
    result.state[i] = interpolateValue(t, left.time, left.state[i], right.time, right.state[i]);
  }

  for(std::size_t i = 0; i < nOutputs_; i++) {
    result.values[i] = interpolateValue(t, left.time, left.values[i], right.time, right.values[i]);
  }

  result.time = t;
}


/* Linear value interpolation. */
fmiReal IncrementalFMU::interpolateValue( TIMESTAMP x, TIMESTAMP x0, fmiReal y0, TIMESTAMP x1, fmiReal y1 ) const
{
  return y0 + (x - x0)*(y1 - y0)/(x1 - x0);
}

/* Linear value interpolation. */
/* Don't need this version as long as TIMESTAMP == fmiReal
TIMESTAMP IncrementalBatteryFMU::interpolateValue( fmiReal x, fmiReal x0, TIMESTAMP y0, fmiReal x1, TIMESTAMP y1 ) const
{
  return y0 + (x - x0)*(y1 - y0)/(x1 - x0);
}
*/

TIMESTAMP IncrementalFMU::sync( TIMESTAMP t0, TIMESTAMP t1 )
{
  TIMESTAMP t_update = updateState(t0, t1); // Update state.
  if(t_update != t1) {
    return t_update; // Return t_update in case of failure.
  }
  
  // Predict the future state (but make no update yet!), return time for next update.
  TIMESTAMP t2 = predictState(t1);

  return t2;
}

/* be very careful with this sync function, as the inputs are set for the prediction
   i.e. at the _end_ of the interval [t0, t1], before the lookahead takes place */
TIMESTAMP IncrementalFMU::sync(TIMESTAMP t0, TIMESTAMP t1, fmiReal* inputs)
{
  TIMESTAMP t_update = updateState(t0, t1); // Update state.

  if(t_update != t1) {
    return t_update; // Return t_update in case of failure.
  }
  
  // set the new inputs before makeing a prediction
  setCurrentInputs(inputs);

  // Predict the future state (but make no update yet!), return time for next update.
  TIMESTAMP t2 = predictState(t1);

  return t2;
}


void IncrementalFMU::getState(TIMESTAMP t, IncrementalFMU::HistoryEntry& state)
{
  TIMESTAMP oldestPredictionTime = predictions_.front().time;
  TIMESTAMP newestPredictionTime = predictions_.back().time;

  // Check if time stamp t is within the range of the predictions.
  if((t < oldestPredictionTime) || (t > newestPredictionTime)) {
    state.time = TS_INVALID;
    return;
  }

  // If necessary, rewind the internal FMU time.
  if(t < newestPredictionTime) {
    fmu_->rewindTime( newestPredictionTime - t );
  }

  // Search the previous predictions for the state at time t. The search is
  // performed from back to front, because the last entry is hopefully the
  // correct one ...
  History_const_reverse_iterator itFind = predictions_.rbegin();
  History_const_reverse_iterator itEnd = predictions_.rend();
  for( ; itFind != itEnd; ++itFind) {
    if(t == itFind->time) {
      state = *itFind;
      /* should not be necessary, remove again, but have a look ;) !!!
      if ( t < newestPredictionTime ) {
	fmu_->setContinuousStates(state.state);
	fmu_->rewindTime( newestPredictionTime - t );
      }
      */      
      return;
    }
    if(t > itFind->time) {
      interpolateState(t, itFind, state);
      return;
    }
  }

  state.time = TS_INVALID;
}


/* Apply the most recent prediction and make a state update. */
TIMESTAMP IncrementalFMU::updateState(TIMESTAMP t0, TIMESTAMP t1)
{
  // Get prediction for time t1.
  getState(t1, currentState_);
  if(TS_INVALID == currentState_.time) {
    return TS_INVALID;
  }

  return t1;
}


/* Predict the future state but make no update yet. */
TIMESTAMP IncrementalFMU::predictState(TIMESTAMP t1)
{
  // Return if initial state is invalid.
  if(TS_INVALID == currentState_.time) {
    return TS_INVALID;
  }

  // Clear previous predictions.
  predictions_.clear();

  // Initialize the first state and the FMU.
  HistoryEntry prediction;
  History_const_iterator newestPrediction;
  prediction = currentState_;
  initializeIntegration(prediction.state);
  fmu_->raiseEvent();

  // Set the initial prediction.
  predictions_.push_back(prediction);

  // Make predictions ...
  TIMESTAMP lastIntegratorStepTime = currentState_.time;
  //TIMESTAMP horizon = ( 0 == lookAheadHorizon_ ) ? globalFMUSyncTime_ : ( t1 + lookAheadHorizon_ );
  TIMESTAMP horizon = t1 + lookAheadHorizon_;
  while(prediction.time < horizon) {

    // if used with other version of FMU.h, remove "prediction.time +"
    // Integration step.
    fmu_->integrate(prediction.time + integratorStepSize_);

    // Retrieve results from FMU integration.
    retrieveIntegrationResults(prediction.state, prediction.values, eventinds_);

    // Add latest prediction.
    //    prediction.time += integratorStepSize_;
    prediction.time += integratorStepSize_;

    predictions_.push_back( prediction );

    //printf( "[IncrementalFMUBase::predictState] temp = %f - consumed = %f - time = %d\n", prediction.state.temperature, prediction.state.consumed, prediction.time ); fflush(stdout);

    lastIntegratorStepTime = prediction.time;

    newestPrediction = predictions_.end();

    // Check if an event has occured.
    // interpolation for the events or something better than just stopping the step after the event has occurred would be nice !!!
    if(checkForEvent()) {
      // not necessary right now, but improve this !!!
      // Rewind internal time of the FMU to match the event.
      //      fmu_->rewindTime( prediction.time - eventTime );
      //      fmu_->rewindTime(integratorStepSize_);
      fmu_->integrate(prediction.time); 
      retrieveIntegrationResults(prediction.state, prediction.values, eventinds_);

      for(std::size_t i = 0; i < fmu_->nEventInds(); i++) {
	eventindspos_[i] = eventinds_[i] > 0;
      }

      // prediction.time = eventTime;
      predictions_.pop_back();
      predictions_.push_back(prediction);
      
      return prediction.time;
    }
  }

  //if((0 == lookAheadHorizon_) && (prediction.time > horizon)) return horizon;

  return prediction.time;
}


void IncrementalFMU::retrieveIntegrationResults(fmiReal* result, fmiReal* values, fmiReal* eventinds) const
{
  fmu_->getContinuousStates(result);
  for(std::size_t i = 0; i < nOutputs_; i++) {
    fmu_->getValue(outputs_[i], &values[i]);
  }
  fmu_->getEventIndicators(eventinds);
}


fmiStatus IncrementalFMU::setCurrentInputs(fmiReal* inputs) const {
  for(std::size_t i = 0; i < nInputs_; i++) {
    fmu_->setValue(inputs_[i], inputs[i]);
  }
}

/** Using macros is in general not the best thing to do, but since FMI
 *  forces us to do use them anyway, it doesn't make things worse ...
 */
//#define VALUE_TO_STRING(ID) TO_STRING(ID)
//#define TO_STRING(VALUE) #VALUE

