/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


/**
 * \file IncrementalFMU.cpp 
 */ 

#include <iostream>
#include <cassert>
#include <cmath>

#include "IncrementalFMU.h"
#include "FMU.h"


using namespace std;


IncrementalFMU::IncrementalFMU( const string& fmuPath,
				const string& modelName ) : nRealInputs_( 0 ), nIntegerInputs_( 0 ), nBooleanInputs_( 0 ), nStringInputs_( 0 ), nRealOutputs_( 0 ), nIntegerOutputs_( 0 ), nBooleanOutputs_( 0 ), nStringOutputs_( 0 )
{
	fmu_ = new FMU( fmuPath, modelName );
}


IncrementalFMU::IncrementalFMU( const string& xmlPath,
				const string& dllPath,
				const string& modelName ) : nRealInputs_( 0 ), nIntegerInputs_( 0 ), nBooleanInputs_( 0 ), nStringInputs_( 0 ), nRealOutputs_( 0 ), nIntegerOutputs_( 0 ), nBooleanOutputs_( 0 ), nStringOutputs_( 0 )
{
	fmu_ = new FMU( xmlPath, dllPath, modelName );
}


IncrementalFMU::IncrementalFMU( const IncrementalFMU& aIncrementalFMU ) {
	fmu_ = new FMU( *(aIncrementalFMU.fmu_) );
	nRealInputs_ = aIncrementalFMU.nRealInputs_;
	nIntegerInputs_ = aIncrementalFMU.nIntegerInputs_;
	nBooleanInputs_ = aIncrementalFMU.nBooleanInputs_;
	nStringInputs_ = aIncrementalFMU.nStringInputs_;
	nRealOutputs_ = aIncrementalFMU.nRealOutputs_;
	nIntegerOutputs_ = aIncrementalFMU.nIntegerOutputs_;
	nBooleanOutputs_ = aIncrementalFMU.nBooleanOutputs_;
	nStringOutputs_ = aIncrementalFMU.nStringOutputs_;
}


IncrementalFMU::~IncrementalFMU()
{
	delete fmu_;
}


void IncrementalFMU::defineRealInputs( const string inputs[], const size_t nInputs ) {
	nRealInputs_ = nInputs;
	realInputRefs_ = new size_t[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		realInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void IncrementalFMU::defineIntegerInputs( const string inputs[], const size_t nInputs ) {
	nIntegerInputs_ = nInputs;
	integerInputRefs_ = new size_t[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		integerInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void IncrementalFMU::defineBooleanInputs( const string inputs[], const size_t nInputs ) {
	nBooleanInputs_ = nInputs;
	booleanInputRefs_ = new size_t[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		booleanInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void IncrementalFMU::defineStringInputs( const string inputs[], const size_t nInputs ) {
	nStringInputs_ = nInputs;
	stringInputRefs_ = new size_t[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		stringInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void IncrementalFMU::defineRealOutputs( const string outputs[], const size_t nOutputs ) {
	nRealOutputs_ = nOutputs;
	realOutputRefs_ = new size_t[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		realOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void IncrementalFMU::defineIntegerOutputs( const string outputs[], const size_t nOutputs ) {
	nIntegerOutputs_ = nOutputs;
	integerOutputRefs_ = new size_t[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		integerOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void IncrementalFMU::defineBooleanOutputs( const string outputs[], const size_t nOutputs ) {
	nBooleanOutputs_ = nOutputs;
	booleanOutputRefs_ = new size_t[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		booleanOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void IncrementalFMU::defineStringOutputs( const string outputs[], const size_t nOutputs ) {
	nStringOutputs_ = nOutputs;
	stringOutputRefs_ = new size_t[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		stringOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


bool IncrementalFMU::checkForEvent( const HistoryEntry& newestPrediction )
{
	return fmu_->getStateEventFlag();
}


void IncrementalFMU::handleEvent()
{
}


void IncrementalFMU::setInitialInputs( const std::string realVariableNames[],
									   const fmiReal* realValues,
									   std::size_t nRealVars,
									   const std::string integerVariableNames[],
									   const fmiInteger* integerValues,
									   std::size_t nIntegerVars,
									   const std::string booleanVariableNames[],
									   const fmiBoolean* booleanValues,
									   std::size_t nBooleanVars,
									   const std::string stringVariableNames[],
									   const std::string* stringValues,
									   std::size_t nStringVars )
{
	for ( size_t i = 0; i < nRealVars; ++i ) {
		fmu_->setValue(realVariableNames[i], realValues[i]);
	}
	for ( size_t i = 0; i < nIntegerVars; ++i ) {
		fmu_->setValue(integerVariableNames[i], integerValues[i]);
	}
	for ( size_t i = 0; i < nBooleanVars; ++i ) {
		fmu_->setValue(booleanVariableNames[i], booleanValues[i]);
	}
	for ( size_t i = 0; i < nStringVars; ++i ) {
		fmu_->setValue(stringVariableNames[i], stringValues[i]);
	}
}


void IncrementalFMU::initializeIntegration( HistoryEntry& initialPrediction )
{
	fmiReal* initialState = initialPrediction.state_;
	fmu_->setContinuousStates(initialState);
}


void IncrementalFMU::getContinuousStates( fmiReal* state ) const
{
	fmu_->getContinuousStates( state );
}


void IncrementalFMU::getOutputs( fmiReal* outputs ) const
{
	for ( size_t i = 0; i < nRealOutputs_; ++i ) {
		fmu_->getValue( realOutputRefs_[i], outputs[i] );
	}
}


void IncrementalFMU::getOutputs( fmiInteger* outputs ) const
{
	for ( size_t i = 0; i < nIntegerOutputs_; ++i ) {
		fmu_->getValue( integerOutputRefs_[i], outputs[i] );
	}
}


void IncrementalFMU::getOutputs( fmiBoolean* outputs ) const
{
	for ( size_t i = 0; i < nBooleanOutputs_; ++i ) {
		fmu_->getValue( booleanOutputRefs_[i], outputs[i] );
	}
}


void IncrementalFMU::getOutputs( std::string* outputs ) const
{
	for ( size_t i = 0; i < nStringOutputs_; ++i ) {
		fmu_->getValue( stringOutputRefs_[i], outputs[i] );
	}
}


int IncrementalFMU::init( const std::string& instanceName,
						  const std::string realVariableNames[],
						  const fmiReal* realValues,
						  const std::size_t nRealVars,
						  const std::string integerVariableNames[],
						  const fmiInteger* integerValues,
						  const std::size_t nIntegerVars,
						  const std::string booleanVariableNames[],
						  const fmiBoolean* booleanValues,
						  const std::size_t nBooleanVars,
						  const std::string stringVariableNames[],
						  const std::string* stringValues,
						  const std::size_t nStringVars,
						  const fmiTime startTime,
						  const fmiTime lookAheadHorizon,
						  const fmiTime lookAheadStepSize,
						  const fmiTime integratorStepSize )
{
	assert( lookAheadHorizon > 0. );
	assert( lookAheadStepSize > 0. );
	assert( integratorStepSize > 0. );

	fmiStatus status = fmu_->instantiate( instanceName, fmiFalse );

	if ( status != fmiOK ) return 0;

	// Set inputs (has to happen before initialization of FMU).
	setInitialInputs( realVariableNames, realValues, nRealVars, integerVariableNames, integerValues, nIntegerVars, booleanVariableNames, booleanValues, nBooleanVars, stringVariableNames, stringValues, nStringVars );

	// Intialize FMU.
	if ( fmu_->initialize() != fmiOK ) return 0;

	// Define the initial state: The initial state might include guesses. In such
	// cases we have to raise an event (and iterate over fmiEventUpdate) until the
	// FMU has found a solution ...

	HistoryEntry init( startTime, fmu_->nStates(), nRealOutputs_, nIntegerOutputs_, nBooleanOutputs_, nStringOutputs_ );
	getContinuousStates( init.state_ );
	getOutputs( init.realValues_ );
	getOutputs( init.integerValues_ );
	getOutputs( init.booleanValues_ );
	getOutputs( init.stringValues_ );

	initializeIntegration( init ); // Set values (but don't integrate afterwards) ...
	fmu_->raiseEvent(); // ... then raise an event ...
	fmu_->handleEvents( startTime, false ); // ... and finally take proper actions.
	retrieveFMUState( init.state_, init.realValues_, init.integerValues_, init.booleanValues_, init.stringValues_ ); // Then retrieve the result and ...
	predictions_.push_back( init ); // ... store as prediction -> will be used by first call to updateState().

	lookAheadHorizon_ = lookAheadHorizon;
	lookaheadStepSize_ = lookAheadStepSize;
	integratorStepSize_ = integratorStepSize;

	return 1;  /* return 1 on success, 0 on failure */
}


/* In case no look-ahead prediction is given for time t, this function is responsible to provide
 * an estimate for the corresponding state. For convenience, a REVERSE iterator pointing to the
 * next prediction available AFTER time t is handed over to the function.
 */
void IncrementalFMU::interpolateState( fmiTime t,
				       History_const_reverse_iterator& historyEntry,
				       HistoryEntry& result)
{
	const HistoryEntry& right = *(historyEntry-1);
	const HistoryEntry& left = *(historyEntry);

	for ( size_t i = 0; i < fmu_->nStates(); ++i ) {
		result.state_[i] = interpolateValue( t, left.time_, left.state_[i], right.time_, right.state_[i] );
	}

	for ( size_t i = 0; i < nRealOutputs_; ++i ) {
		result.realValues_[i] = interpolateValue( t, left.time_, left.realValues_[i], right.time_, right.realValues_[i] );
	}

	// no sense in interpolating other values.

	result.time_ = t;
}


/* Linear value interpolation. */
fmiReal IncrementalFMU::interpolateValue( fmiReal x, fmiReal x0, fmiReal y0, fmiReal x1, fmiReal y1 ) const
{
	return y0 + (x - x0)*(y1 - y0)/(x1 - x0);
}


fmiTime IncrementalFMU::sync( fmiTime t0, fmiTime t1 )
{
	fmiTime t_update = updateState( t1 ); // Update state.

	if ( t_update != t1 ) {
		return t_update; // Return t_update in case of failure.
	}

	// Predict the future state (but make no update yet!), return time for next update.
	fmiTime t2 = predictState( t1 );
	return t2;
}


/* be very careful with this sync function, as the inputs are set for the prediction
   i.e. at the _end_ of the interval [t0, t1], before the lookahead takes place */
fmiTime IncrementalFMU::sync( fmiTime t0, fmiTime t1, fmiReal* realInputs, fmiInteger* integerInputs, fmiBoolean* booleanInputs, std::string* stringInputs )
{
	fmiTime t_update = updateState( t1 ); // Update state.

	if ( t_update != t1 ) {
		return t_update; // Return t_update in case of failure.
	}

	// Set the new inputs before making a prediction.
	syncState( t1, realInputs, integerInputs, booleanInputs, stringInputs );

	// Predict the future state (but make no update yet!), return time for next update.
	fmiTime t2 = predictState( t1 );

	return t2;
}


void IncrementalFMU::getState( fmiTime t, HistoryEntry& state )
{
	fmiTime oldestPredictionTime = predictions_.front().time_;
	fmiTime newestPredictionTime = predictions_.back().time_;

	// Check if time stamp t is within the range of the predictions.
	if ( ( t < oldestPredictionTime ) || ( t > newestPredictionTime ) ) {
		state.time_ = INVALID_FMI_TIME;
		return;
	}

	// If necessary, rewind the internal FMU time.
	if ( t < newestPredictionTime ) {
		// fmu_->rewindTime( newestPredictionTime - t );
		fmu_->setTime( t );
	}

	// Search the previous predictions for the state at time t. The search is
	// performed from back to front, because the last entry is hopefully the
	// correct one ...
	History_const_reverse_iterator itFind = predictions_.rbegin();
	History_const_reverse_iterator itEnd = predictions_.rend();
	for ( ; itFind != itEnd; ++itFind ) {
		if ( fabs( t - itFind->time_ ) < timeDiffResolution_ ) {
			state = *itFind;
			/* should not be necessary, remove again, but have a look ;) !!!
			   if ( t < newestPredictionTime ) {
			   fmu_->setContinuousStates(state.state);
			   fmu_->rewindTime( newestPredictionTime - t );
			   }
			*/
			return;
		}
		if ( t > itFind->time_ ) {
			interpolateState(t, itFind, state);
			return;
		}
	}

	state.time_ = INVALID_FMI_TIME;
}


/* Apply the most recent prediction and make a state update. */
fmiTime IncrementalFMU::updateState( fmiTime t1 )
{
	// Get prediction for time t1.
	getState( t1, currentState_ );

	if ( INVALID_FMI_TIME == currentState_.time_ ) {
		return INVALID_FMI_TIME;
	}

	// somewhere i have to do this, ask EW which functions he overloads, so we can solve this better!!!
	initializeIntegration( currentState_ );
	fmu_->setTime( t1 );

	return t1;
}


/* Predict the future state but make no update yet. */
fmiTime IncrementalFMU::predictState( fmiTime t1 )
{
	lastEventTime_ = numeric_limits<fmiTime>::infinity();

	// Return if initial state is invalid.
	if ( INVALID_FMI_TIME == currentState_.time_ ) {
		return INVALID_FMI_TIME;
	}

	// Clear previous predictions.
	predictions_.clear();

	// Initialize the first state and the FMU.
	HistoryEntry prediction;

	prediction = currentState_;
	prediction.time_ = t1;

	// Retrieve the current state of the FMU, considering altered inputs. --> handled now by syncState(...).
	//fmu_->handleEvents( prediction.time_, false );
	//retrieveFMUState( prediction.state_, prediction.realValues_, prediction.integerValues_, prediction.booleanValues_, prediction.stringValues_ );

	// Initialize integration.
	initializeIntegration( prediction );

	// Set the initial prediction.
	predictions_.push_back( prediction );

	// Make predictions ...
	fmiTime horizon = t1 + lookAheadHorizon_;
	while ( prediction.time_ < horizon ) {

		// if used with other version of FMU.h, remove "prediction.time +"
		// Integration step.
		lastEventTime_ = fmu_->integrate( prediction.time_ + lookaheadStepSize_, integratorStepSize_ );

		// Retrieve results from FMU integration.
		retrieveFMUState( prediction.state_, prediction.realValues_, prediction.integerValues_, prediction.booleanValues_, prediction.stringValues_ );

		// Add latest prediction.
		prediction.time_ += lookaheadStepSize_;

		predictions_.push_back( prediction );

		if ( lastEventTime_ >= prediction.time_ ) {
			fmu_->setStateEventFlag( fmiFalse );
		}

		// Check if an event has occured.
		// interpolation for the events or something better than just stopping and integration
		// until the end of the step after which the event has occurred would be nice !!!
		if ( checkForEvent( prediction ) ) {
			handleEvent();

			// "handleEvent()" might alter the last prediction stored
			// in vector "predictions_"  --> use reference instead of
			// loop variable "prediction"!
			HistoryEntry& lastPrediction = predictions_.back();

			// this has to be changed if the event is detected precisely
			// and is not just within the last step !!!
			lastPrediction.time_ = lastEventTime_ + integratorStepSize_;

			fmu_->handleEvents( lastPrediction.time_, false );

			retrieveFMUState( lastPrediction.state_, lastPrediction.realValues_, lastPrediction.integerValues_, lastPrediction.booleanValues_, lastPrediction.stringValues_ );

			return lastPrediction.time_;
		}
	}

	//if ((0 == lookAheadHorizon_) && (prediction.time > horizon)) return horizon;

	return prediction.time_;
}


void IncrementalFMU::retrieveFMUState( fmiReal* result, fmiReal* realValues, fmiInteger* integerValues, fmiBoolean* booleanValues, std::string* stringValues ) const
{
	fmu_->getContinuousStates(result);
	for ( size_t i = 0; i < nRealOutputs_; ++i ) {
		fmu_->getValue(realOutputRefs_[i], realValues[i]);
	}
	for ( size_t i = 0; i < nIntegerOutputs_; ++i ) {
		fmu_->getValue(integerOutputRefs_[i], integerValues[i]);
	}
	for ( size_t i = 0; i < nBooleanOutputs_; ++i ) {
		fmu_->getValue(booleanOutputRefs_[i], booleanValues[i]);
	}
	for ( size_t i = 0; i < nStringOutputs_; ++i ) {
		fmu_->getValue(stringOutputRefs_[i], stringValues[i]);
	}
}


fmiStatus IncrementalFMU::setInputs(fmiReal* inputs) const {

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nRealInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue(realInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


fmiStatus IncrementalFMU::setInputs(fmiInteger* inputs) const {

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nIntegerInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue(integerInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


fmiStatus IncrementalFMU::setInputs(fmiBoolean* inputs) const {

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nBooleanInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue(booleanInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


fmiStatus IncrementalFMU::setInputs(std::string* inputs) const {

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nStringInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue(stringInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


/** Sync state according to the current inputs **/
void IncrementalFMU::syncState( fmiTime t1, fmiReal* realInputs, fmiInteger* integerInputs, fmiBoolean* booleanInputs, std::string* stringInputs )
{
	// set the new inputs before makeing a prediction
	setInputs( realInputs );
	setInputs( integerInputs );
	setInputs( booleanInputs );
	setInputs( stringInputs );

	currentState_.time_ = t1;

	// Retrieve the current state of the FMU, considering altered inputs.
	fmu_->handleEvents( t1 , false );
	retrieveFMUState( currentState_.state_,
			  currentState_.realValues_, currentState_.integerValues_,
			  currentState_.booleanValues_, currentState_.stringValues_ );
}
