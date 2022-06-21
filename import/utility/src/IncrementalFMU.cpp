// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file IncrementalFMU.cpp
 */
#include <cassert>
#include <cmath>
#include <sstream>
#include "import/base/include/FMUModelExchange_v1.h"
#include "import/base/include/FMUModelExchange_v2.h"
#include "import/base/include/ModelDescription.h"
#include "import/base/include/ModelManager.h"

#include "import/utility/include/IncrementalFMU.h"

using namespace std;

IncrementalFMU::IncrementalFMU( const fmippString& fmuDirUri,
	const fmippString& modelIdentifier,
	const fmippBoolean loggingOn,
	const fmippReal timeDiffResolution,
	const IntegratorType integratorType ) :
		fmu_(NULL),
		realInputRefs_( 0 ), integerInputRefs_( 0 ), booleanInputRefs_( 0 ), stringInputRefs_( 0 ),
		nRealInputs_( 0 ), nIntegerInputs_( 0 ), nBooleanInputs_( 0 ), nStringInputs_( 0 ),
		realOutputRefs_( 0 ), integerOutputRefs_( 0 ), booleanOutputRefs_( 0 ), stringOutputRefs_( 0 ),
		nRealOutputs_( 0 ), nIntegerOutputs_( 0 ), nBooleanOutputs_( 0 ), nStringOutputs_( 0 ),
		lookAheadHorizon_( numeric_limits<fmippTime>::quiet_NaN() ),
		lookaheadStepSize_( numeric_limits<fmippTime>::quiet_NaN() ),
		integratorStepSize_( numeric_limits<fmippTime>::quiet_NaN() ),
		lastEventTime_( numeric_limits<fmippTime>::infinity() ),
		timeDiffResolution_( timeDiffResolution ), loggingOn_( loggingOn )
{
	// Load the FMU.
	FMUType fmuType = invalid;
	ModelManager::LoadFMUStatus loadStatus = ModelManager::loadFMU( modelIdentifier, fmuDirUri, loggingOn, fmuType );

	if ( ( ModelManager::success != loadStatus ) && ( ModelManager::duplicate != loadStatus ) ) { // Loading the FMU failed.
		fmu_ = 0;
		return;
	}

	// No better error reporting, yet
	(void) instantiateModelExchangeFMU(modelIdentifier, fmuType, loggingOn,
		timeDiffResolution, integratorType);
}

IncrementalFMU::IncrementalFMU( const fmippString& modelIdentifier,
	const fmippBoolean loggingOn,
	const fmippReal timeDiffResolution,
	const IntegratorType integratorType ) :
		fmu_(NULL),
		realInputRefs_( 0 ), integerInputRefs_( 0 ), booleanInputRefs_( 0 ), stringInputRefs_( 0 ),
		nRealInputs_( 0 ), nIntegerInputs_( 0 ), nBooleanInputs_( 0 ), nStringInputs_( 0 ),
		realOutputRefs_( 0 ), integerOutputRefs_( 0 ), booleanOutputRefs_( 0 ), stringOutputRefs_( 0 ),
		nRealOutputs_( 0 ), nIntegerOutputs_( 0 ), nBooleanOutputs_( 0 ), nStringOutputs_( 0 ),
		lookAheadHorizon_( numeric_limits<fmippTime>::quiet_NaN() ),
		lookaheadStepSize_( numeric_limits<fmippTime>::quiet_NaN() ),
		integratorStepSize_( numeric_limits<fmippTime>::quiet_NaN() ),
		lastEventTime_( numeric_limits<fmippTime>::infinity() ),
		timeDiffResolution_( timeDiffResolution ), loggingOn_( loggingOn )
{
	// Load the FMU.
	FMUType fmuType = invalid;
	ModelManager::LoadFMUStatus loadStatus;

	loadStatus = ModelManager::getTypeOfLoadedFMU(modelIdentifier, &fmuType);
	if (ModelManager::success != loadStatus) return;

	// No better error reporting, yet
	(void) instantiateModelExchangeFMU(modelIdentifier, fmuType, loggingOn,
		timeDiffResolution, integratorType);
}

IncrementalFMU::~IncrementalFMU()
{
	if ( 0 != fmu_ ) delete fmu_;

	if ( realInputRefs_ ) delete realInputRefs_;
	if ( integerInputRefs_ ) delete integerInputRefs_;
	if ( booleanInputRefs_ ) delete booleanInputRefs_;
	if ( stringInputRefs_ ) delete stringInputRefs_;

	if ( realOutputRefs_ ) delete realOutputRefs_;
	if ( integerOutputRefs_ ) delete integerOutputRefs_;
	if ( booleanOutputRefs_ ) delete booleanOutputRefs_;
	if ( stringOutputRefs_ ) delete stringOutputRefs_;
}

void IncrementalFMU::setIntegratorProperties( Integrator::Properties& prop )
{
	assert( fmu_ );
	if ( !fmu_ ) return;

	fmu_->setIntegratorProperties( prop );
}

Integrator::Properties IncrementalFMU::getIntegratorProperties() const
{
	assert( fmu_ );
	if ( !fmu_ ) return Integrator::Properties();

	return fmu_->getIntegratorProperties();
}

FMIPPVariableType IncrementalFMU::getType( const fmippString& varName ) const
{
	return fmu_->getType( varName );
}

void IncrementalFMU::defineRealInputs( const fmippString inputs[], const fmippSize nInputs )
{
	if ( 0 != realInputRefs_ ) delete realInputRefs_;

	nRealInputs_ = nInputs;
	realInputRefs_ = new fmiValueReference[nInputs];
	for ( fmippSize i = 0; i < nInputs; ++i ) {
		realInputRefs_[i] = fmu_->getValueRef( inputs[i] );

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << inputs[i] << " (" << realInputRefs_[i] << ") "
			    << "to real input variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}

void IncrementalFMU::defineIntegerInputs( const fmippString inputs[], const fmippSize nInputs )
{
	if ( 0 != integerInputRefs_ ) delete integerInputRefs_;

	nIntegerInputs_ = nInputs;
	integerInputRefs_ = new fmiValueReference[nInputs];
	for ( fmippSize i = 0; i < nInputs; ++i ) {
		integerInputRefs_[i] = fmu_->getValueRef( inputs[i] );

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << inputs[i] << " (" << integerInputRefs_[i] << ") "
			    << "to integer input variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}

void IncrementalFMU::defineBooleanInputs( const fmippString inputs[], const fmippSize nInputs )
{
	if ( 0 != booleanInputRefs_ ) delete booleanInputRefs_;

	nBooleanInputs_ = nInputs;
	booleanInputRefs_ = new fmiValueReference[nInputs];
	for ( fmippSize i = 0; i < nInputs; ++i ) {
		booleanInputRefs_[i] = fmu_->getValueRef( inputs[i] );

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << inputs[i] << " (" << booleanInputRefs_[i] << ") "
			    << "to boolean input variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}

void IncrementalFMU::defineStringInputs( const fmippString inputs[], const fmippSize nInputs )
{
	if ( 0 != stringInputRefs_ ) delete stringInputRefs_;

	nStringInputs_ = nInputs;
	stringInputRefs_ = new fmiValueReference[nInputs];
	for ( fmippSize i = 0; i < nInputs; ++i ) {
		stringInputRefs_[i] = fmu_->getValueRef( inputs[i] );

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << inputs[i] << " (" << stringInputRefs_[i] << ") "
			    << "to fmippString input variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}

void IncrementalFMU::defineRealOutputs( const fmippString outputs[], const fmippSize nOutputs )
{
	if ( 0 != realOutputRefs_ ) delete realOutputRefs_;

	nRealOutputs_ = nOutputs;
	realOutputRefs_ = new fmiValueReference[nOutputs];
	for ( fmippSize i = 0; i < nOutputs; ++i ) {
		realOutputRefs_[i] = fmu_->getValueRef( outputs[i] );

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << outputs[i] << " (" << realOutputRefs_[i] << ") "
			    << "to real output variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}

void IncrementalFMU::defineIntegerOutputs( const fmippString outputs[], const fmippSize nOutputs )
{
	if ( 0 != integerOutputRefs_ ) delete integerOutputRefs_;

	nIntegerOutputs_ = nOutputs;
	integerOutputRefs_ = new fmiValueReference[nOutputs];
	for ( fmippSize i = 0; i < nOutputs; ++i ) {
		integerOutputRefs_[i] = fmu_->getValueRef( outputs[i] );

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << outputs[i] << " (" << integerOutputRefs_[i] << ") "
			    << "to integer output variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}

void IncrementalFMU::defineBooleanOutputs( const fmippString outputs[], const fmippSize nOutputs )
{
	if ( 0 != booleanOutputRefs_ ) delete booleanOutputRefs_;

	nBooleanOutputs_ = nOutputs;
	booleanOutputRefs_ = new fmiValueReference[nOutputs];
	for ( fmippSize i = 0; i < nOutputs; ++i ) {
		booleanOutputRefs_[i] = fmu_->getValueRef( outputs[i] );

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << outputs[i] << " (" << booleanOutputRefs_[i] << ") "
			    << "to boolean output variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}

void IncrementalFMU::defineStringOutputs( const fmippString outputs[], const fmippSize nOutputs )
{
	if ( 0 != stringOutputRefs_ ) delete stringOutputRefs_;

	nStringOutputs_ = nOutputs;
	stringOutputRefs_ = new fmiValueReference[nOutputs];
	for ( fmippSize i = 0; i < nOutputs; ++i ) {
		stringOutputRefs_[i] = fmu_->getValueRef( outputs[i] );

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << outputs[i] << " (" << stringOutputRefs_[i] << ") "
			    << "to fmippString output variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}

bool IncrementalFMU::checkForEvent( const HistoryEntry& newestPrediction )
{
	fmu_->checkEvents();
	return ( fmippTrue == fmu_->getEventFlag() );
}

void IncrementalFMU::handleEvent() {}

fmippStatus IncrementalFMU::setInitialInputs( const fmippString realVariableNames[],
					    const fmippReal* realValues,
					    fmippSize nRealVars,
					    const fmippString integerVariableNames[],
					    const fmippInteger* integerValues,
					    fmippSize nIntegerVars,
					    const fmippString booleanVariableNames[],
					    const fmippBoolean* booleanValues,
					    fmippSize nBooleanVars,
					    const fmippString fmippStringVariableNames[],
					    const fmippString* stringValues,
					    fmippSize nfmippStringVars )
{
	fmippStatus ret = fmippOK;

	for ( fmippSize i = 0; i < nRealVars; ++i ) {
		ret = fmu_->setValue(realVariableNames[i], realValues[i]);
		if ( ret != fmippOK )
			return ret;
	}
	for ( fmippSize i = 0; i < nIntegerVars; ++i ) {
		ret = fmu_->setValue(integerVariableNames[i], integerValues[i]);
		if ( ret != fmippOK )
			return ret;
	}
	for ( fmippSize i = 0; i < nBooleanVars; ++i ) {
		ret = fmu_->setValue(booleanVariableNames[i], booleanValues[i]);
		if ( ret != fmippOK )
			return ret;
	}
	for ( fmippSize i = 0; i < nfmippStringVars; ++i ) {
		ret = fmu_->setValue(fmippStringVariableNames[i], stringValues[i]);
		if ( ret != fmippOK )
			return ret;
	}
	return ret;
}

void IncrementalFMU::initializeIntegration( HistoryEntry& initialPrediction )
{
	fmippReal* initialState = initialPrediction.state_;
	fmu_->setContinuousStates(initialState);
}

void IncrementalFMU::getContinuousStates( fmippReal* state ) const
{
	fmu_->getContinuousStates( state );
}

void IncrementalFMU::getOutputs( fmippReal* outputs ) const
{
	for ( fmippSize i = 0; i < nRealOutputs_; ++i ) {
		fmu_->getValue( realOutputRefs_[i], outputs[i] );
	}
}

void IncrementalFMU::getOutputs( fmippInteger* outputs ) const
{
	for ( fmippSize i = 0; i < nIntegerOutputs_; ++i ) {
		fmu_->getValue( integerOutputRefs_[i], outputs[i] );
	}
}

void IncrementalFMU::getOutputs( fmippBoolean* outputs ) const
{
	for ( fmippSize i = 0; i < nBooleanOutputs_; ++i ) {
		fmu_->getValue( booleanOutputRefs_[i], outputs[i] );
	}
}

void IncrementalFMU::getOutputs( fmippString* outputs ) const
{
	for ( fmippSize i = 0; i < nStringOutputs_; ++i ) {
		fmu_->getValue( stringOutputRefs_[i], outputs[i] );
	}
}

int IncrementalFMU::init( const fmippString& instanceName,
			  const fmippString realVariableNames[],
			  const fmippReal* realValues,
			  const fmippSize nRealVars,
			  const fmippString integerVariableNames[],
			  const fmippInteger* integerValues,
			  const fmippSize nIntegerVars,
			  const fmippString booleanVariableNames[],
			  const fmippBoolean* booleanValues,
			  const fmippSize nBooleanVars,
			  const fmippString fmippStringVariableNames[],
			  const fmippString* stringValues,
			  const fmippSize nfmippStringVars,
			  const fmippTime startTime,
			  const fmippTime lookAheadHorizon,
			  const fmippTime lookAheadStepSize,
			  const fmippTime integratorStepSize,
			  const bool toleranceDefined,
			  const double tolerance )
{
	assert( lookAheadHorizon > 0. );
	assert( lookAheadStepSize > 0. );
	assert( integratorStepSize > 0. );

	fmippStatus status = fmu_->instantiate( instanceName );

	if ( status != fmippOK ) return 0;

	// Set inputs (has to happen before initialization of FMU).
	status = setInitialInputs( realVariableNames, realValues, nRealVars,
				   integerVariableNames, integerValues, nIntegerVars,
				   booleanVariableNames, booleanValues, nBooleanVars,
				   fmippStringVariableNames, stringValues, nfmippStringVars );

	if ( status != fmippOK ) return 0;

	// Intialize FMU.
	if ( fmu_->initialize( toleranceDefined, tolerance ) != fmippOK ) return 0;

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
	fmu_->handleEvents(); // ... and finally take proper actions.
	retrieveFMUState( init.state_, init.realValues_, init.integerValues_, init.booleanValues_, init.stringValues_ ); // Then retrieve the result and ...
	predictions_.push_back( init ); // ... store as prediction -> will be used by first call to updateState().

	predictions_[0] = predictions_[0];

	currentState_ = init;

	lookAheadHorizon_ = lookAheadHorizon;
	lookaheadStepSize_ = lookAheadStepSize;
	integratorStepSize_ = integratorStepSize;

	return 1;  /* return 1 on success, 0 on failure */
}

/* In case no look-ahead prediction is given for time t, this function is responsible to provide
 * an estimate for the corresponding state. For convenience, a REVERSE iterator pointing to the
 * next prediction available AFTER time t is handed over to the function.
 */
void IncrementalFMU::interpolateState( fmippTime t,
	History::const_reverse_iterator& historyEntry,
	HistoryEntry& result )
{
	const HistoryEntry& right = *(historyEntry-1);
	const HistoryEntry& left = *(historyEntry);

	for ( fmippSize i = 0; i < fmu_->nStates(); ++i ) {
		result.state_[i] = interpolateValue( t, left.time_, left.state_[i], right.time_, right.state_[i] );
	}

	for ( fmippSize i = 0; i < nRealOutputs_; ++i ) {
		result.realValues_[i] = interpolateValue( t, left.time_, left.realValues_[i], right.time_, right.realValues_[i] );
	}

	// no sense in interpolating other values.

	result.time_ = t;
}

/* Linear value interpolation. */
fmippReal IncrementalFMU::interpolateValue( fmippReal x, fmippReal x0, fmippReal y0, fmippReal x1, fmippReal y1 ) const
{
	return y0 + (x - x0)*(y1 - y0)/(x1 - x0);
}

fmippTime IncrementalFMU::sync( fmippTime t0, fmippTime t1 )
{
	if ( fmippTrue == loggingOn_ )
	{
		stringstream msg;
		msg << "syncing FMU - t0 = " << t0 << ", t1 = " << t1;
		fmu_->sendDebugMessage( msg.str() );
	}

	fmippTime t_update = updateState( t1 ); // Update state.

	if ( t_update != t1 ) {
		return t_update; // Return t_update in case of failure.
	}

	// Predict the future state (but make no update yet!), return time for next update.
	fmippTime t2 = predictState( t1 );
	return t2;
}

/* be very careful with this sync function, as the inputs are set for the prediction
   i.e. at the _end_ of the interval [t0, t1], before the lookahead takes place */
fmippTime IncrementalFMU::sync( fmippTime t0, fmippTime t1, fmippReal* realInputs, fmippInteger* integerInputs, fmippBoolean* booleanInputs, fmippString* stringInputs )
{
	if ( fmippTrue == loggingOn_ )
	{
		stringstream msg;
		msg << "syncing FMU with inputs - t0 = " << t0 << ", t1 = " << t1;
		fmu_->sendDebugMessage( msg.str() );
	}

	fmippTime t_update = updateState( t1 ); // Update state.

	if ( t_update != t1 ) {
		return t_update; // Return t_update in case of failure.
	}

	// Set the new inputs before making a prediction.
	syncState( t1, realInputs, integerInputs, booleanInputs, stringInputs );

	// Predict the future state (but make no update yet!), return time for next update.
	fmippTime t2 = predictState( t1 );

	return t2;
}

void IncrementalFMU::getState( fmippTime t, HistoryEntry& state )
{
	fmippTime oldestPredictionTime = predictions_.front().time_;
	fmippTime newestPredictionTime = predictions_.back().time_;

	// Check if time stamp t is within the range of the predictions.
	if ( ( t <= oldestPredictionTime - timeDiffResolution_ ) ||
			 ( t >= newestPredictionTime + timeDiffResolution_ ) ) {
		state.time_ = INVALID_FMI_TIME;
		return;
	}

	// If necessary, rewind the internal FMU time.
	if ( fabs( t - newestPredictionTime ) > timeDiffResolution_ ) {
		// fmu_->rewindTime( newestPredictionTime - t );
		fmu_->setTime( t );
	}

	// Search the previous predictions for the state at time t. The search is
	// performed from back to front, because the last entry is hopefully the
	// correct one ...
	History::const_reverse_iterator itFind = predictions_.rbegin();
	History::const_reverse_iterator itEnd = predictions_.rend();
	for ( ; itFind != itEnd; ++itFind ) {

		if ( fabs( t - itFind->time_ ) <= timeDiffResolution_ ) {
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
fmippTime IncrementalFMU::updateState( fmippTime t1 )
{
	// Get prediction for time t1.
	getState( t1, currentState_ );

	if ( t1 <= lastEventTime_ && checkForEvent( currentState_ ) ) {
		fmu_->setEventFlag( fmippFalse );
	}

	if ( INVALID_FMI_TIME == currentState_.time_ ) {
		return INVALID_FMI_TIME;
	}

	if ( fabs( t1 - currentState_.time_ ) < timeDiffResolution_ ) currentState_.time_ = t1;

	// somewhere i have to do this, ask EW which functions he overloads, so we can solve this better!!!
	initializeIntegration( currentState_ );
	fmu_->setTime( t1 );
	fmu_->raiseEvent();
	fmu_->handleEvents();

	return t1;
}

fmippTime IncrementalFMU::updateStateFromTheRight( fmippTime t1 )
{
	if ( !(INVALID_FMI_TIME != t1) ) // Also return on NaN
		return INVALID_FMI_TIME;

	// Decide whether to use the right hand side limit
	// Just a hint, prediction horizon may be reached without an event.
	bool eventFlag = !predictions_.empty()
		&& fabs(predictions_.back().time_ - t1) < timeDiffResolution_;

	fmippTime ret = updateState(t1);

	if(ret != INVALID_FMI_TIME && eventFlag)
	{
		// Slightly forward time
		ret = fmu_->integrate( currentState_.time_ + timeDiffResolution_, integratorStepSize_ );

		if(!(ret != INVALID_FMI_TIME))
			return ret;

		fmu_->handleEvents();

		retrieveFMUState(currentState_.state_, currentState_.realValues_,
			currentState_.integerValues_, currentState_.booleanValues_,
			currentState_.stringValues_);

		currentState_.time_ = ret;

		initializeIntegration( currentState_ );

		predictions_.back() = currentState_;
	}

	return ret;
}

/* Predict the future state but make no update yet. */
fmippTime IncrementalFMU::predictState( fmippTime t1 )
{
	// Return if initial state is invalid.
	if ( INVALID_FMI_TIME == currentState_.time_ ) {
		return INVALID_FMI_TIME;
	}

	// In case the last prediction was caused by an event, the FMU's logical state still corresponds
	// to the left limit of the event. If the current prediction does not start immediately after
	// this event (i.e., t1 < lastEventTime_), the event flags have to be reset. Otherwise the FMU
	// would try to step over this event as soon as it resumes the integration.
	if ( t1 < lastEventTime_ - timeDiffResolution_ ) fmu_->resetEventFlags();

	// Clear previous predictions.
	predictions_.clear();

	// Initialize the first state and the FMU.
	HistoryEntry prediction;

	prediction = currentState_;
	prediction.time_ = t1;

	// Initialize integration.
	initializeIntegration( prediction );

	// Set the initial prediction.
	predictions_.push_back( prediction );

	// Make predictions ...
	fmippTime horizon = t1 + lookAheadHorizon_;
	while ( horizon - prediction.time_ > timeDiffResolution_ ) {
		// if used with other version of FMU.h, remove "prediction.time +"
		// Integration step.
		lastEventTime_ = fmu_->integrate( prediction.time_ + lookaheadStepSize_, integratorStepSize_ );

		// Retrieve results from FMU integration.
		retrieveFMUState( prediction.state_, prediction.realValues_, prediction.integerValues_, prediction.booleanValues_, prediction.stringValues_ );

		// Add latest prediction.
		prediction.time_ = lastEventTime_; //lookaheadStepSize_;

		predictions_.push_back( prediction );

		/*
		if ( lastEventTime_ >= prediction.time_ ) {
			fmu_->setEventFlag( fmippFalse );
		}
		*/
		// Check if an event has occured.
		// interpolation for the events or something better than just stopping and integration
		// until the end of the step after which the event has occurred would be nice !!!
		if ( checkForEvent( prediction ) ) {
			handleEvent();
			return lastEventTime_;
		}
	}

	//if ((0 == lookAheadHorizon_) && (prediction.time > horizon)) return horizon;
	return prediction.time_;
}

fmippStatus
IncrementalFMU::getLastStatus() const
{
	if ( 0 == fmu_ ) return fmippFatal;
	return fmu_->getLastStatus();
}

const ModelDescription*
IncrementalFMU::getModelDescription() const
{
	assert(getLastStatus() != fmippOK || fmu_);
	if (fmu_) {
		return fmu_->getModelDescription();
	} else {
		return NULL;
	}
}

void IncrementalFMU::retrieveFMUState( fmippReal* result, fmippReal* realValues, fmippInteger* integerValues, fmippBoolean* booleanValues, fmippString* stringValues ) const
{
	fmu_->getContinuousStates(result);
	for ( fmippSize i = 0; i < nRealOutputs_; ++i ) {
		fmu_->getValue(realOutputRefs_[i], realValues[i]);
	}
	for ( fmippSize i = 0; i < nIntegerOutputs_; ++i ) {
		fmu_->getValue(integerOutputRefs_[i], integerValues[i]);
	}
	for ( fmippSize i = 0; i < nBooleanOutputs_; ++i ) {
		fmu_->getValue(booleanOutputRefs_[i], booleanValues[i]);
	}
	for ( fmippSize i = 0; i < nStringOutputs_; ++i ) {
		fmu_->getValue(stringOutputRefs_[i], stringValues[i]);
	}
}

fmippStatus IncrementalFMU::setInputs(fmippReal* inputs) const {

	fmippStatus status = fmippOK;

	for ( fmippSize i = 0; i < nRealInputs_; ++i ) {
		if ( fmippOK != fmu_->setValue(realInputRefs_[i], inputs[i]) ) status = fmippError;

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set real input " << realInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}

fmippStatus IncrementalFMU::setInputs(fmippInteger* inputs) const {

	fmippStatus status = fmippOK;

	for ( fmippSize i = 0; i < nIntegerInputs_; ++i ) {
		if ( fmippOK != fmu_->setValue(integerInputRefs_[i], inputs[i]) ) status = fmippError;

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set integer input " << integerInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}

fmippStatus IncrementalFMU::setInputs(fmippBoolean* inputs) const {

	fmippStatus status = fmippOK;

	for ( fmippSize i = 0; i < nBooleanInputs_; ++i ) {
		if ( fmippOK != fmu_->setValue(booleanInputRefs_[i], inputs[i]) ) status = fmippError;

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set boolean input " << booleanInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}

fmippStatus IncrementalFMU::setInputs(fmippString* inputs) const {

	fmippStatus status = fmippOK;

	for ( fmippSize i = 0; i < nStringInputs_; ++i ) {
		if ( fmippOK != fmu_->setValue(stringInputRefs_[i], inputs[i]) ) status = fmippError;

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set fmippString input " << stringInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}

/** Sync state according to the current inputs **/
void IncrementalFMU::syncState( fmippTime t1, fmippReal* realInputs, fmippInteger* integerInputs, fmippBoolean* booleanInputs, fmippString* stringInputs )
{

	// set the new inputs before makeing a prediction
	// \FIXME Should this function issue a warning/exception in case an input is a null pointer but the number of defined inputs is not zero? Or should it be quietly tolerated that there are sometimes no inputs?
	if ( 0 != realInputs ) setInputs( realInputs );
	if ( 0 != integerInputs ) setInputs( integerInputs );
	if ( 0 != booleanInputs ) setInputs( booleanInputs );
	if ( 0 != stringInputs ) setInputs( stringInputs );

	currentState_.time_ = t1;

	// Retrieve the current state of the FMU, considering altered inputs.
	fmu_->handleEvents();
	retrieveFMUState( currentState_.state_,
			  currentState_.realValues_, currentState_.integerValues_,
			  currentState_.booleanValues_, currentState_.stringValues_ );
}

fmippStatus IncrementalFMU::instantiateModelExchangeFMU(
	const fmippString& modelIdentifier,
	FMUType modelType,
	const fmippBoolean loggingOn,
	const fmippReal timeDiffResolution,
	const IntegratorType integratorType)
{
	assert( ModelManager::success ==
		ModelManager::getTypeOfLoadedFMU(modelIdentifier, NULL) );
	assert( !fmu_ );

	if ( fmi_1_0_me == modelType ) // FMI ME 1.0
	{
		fmu_ = new fmi_1_0::FMUModelExchange( modelIdentifier,
			loggingOn, fmippTrue, timeDiffResolution, integratorType );
	}
	else if ( ( fmi_2_0_me == modelType ) || ( fmi_2_0_me_and_cs == modelType ) )
	{ // FMI ME 2.0
		fmu_ = new fmi_2_0::FMUModelExchange( modelIdentifier,
			loggingOn, fmippTrue, timeDiffResolution, integratorType );
	} else { // Unsupported FMU Type
		return fmippError;
		fmu_ = NULL;
	}

	return fmippOK;
}
