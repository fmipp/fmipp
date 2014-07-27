/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


/**
 * \file InterpolatingFixedStepSizeFMU.cpp 
 */ 

#include <cassert>

#include "import/base/include/FMUCoSimulation.h"

#include "import/utility/include/InterpolatingFixedStepSizeFMU.h"



using namespace std;


InterpolatingFixedStepSizeFMU::InterpolatingFixedStepSizeFMU( const string& fmuPath,
							      const string& modelName ) :
	currentCommunicationPoint_( 0. ),
	communicationStepSize_( 0. ),
	fmu_( new FMUCoSimulation( fmuPath, modelName ) ),
	realInputRefs_( 0 ), integerInputRefs_( 0 ), booleanInputRefs_( 0 ), stringInputRefs_( 0 ),
	nRealInputs_( 0 ), nIntegerInputs_( 0 ), nBooleanInputs_( 0 ), nStringInputs_( 0 ),
	realOutputRefs_( 0 ), integerOutputRefs_( 0 ), booleanOutputRefs_( 0 ), stringOutputRefs_( 0 ),
	nRealOutputs_( 0 ), nIntegerOutputs_( 0 ), nBooleanOutputs_( 0 ), nStringOutputs_( 0 )
{}


InterpolatingFixedStepSizeFMU::~InterpolatingFixedStepSizeFMU()
{
	delete fmu_;

	if ( realInputRefs_ ) delete realInputRefs_;
	if ( integerInputRefs_ ) delete integerInputRefs_;
	if ( booleanInputRefs_ ) delete booleanInputRefs_;
	if ( stringInputRefs_ ) delete stringInputRefs_;

	if ( realOutputRefs_ ) delete realOutputRefs_;
	if ( integerOutputRefs_ ) delete integerOutputRefs_;
	if ( booleanOutputRefs_ ) delete booleanOutputRefs_;
	if ( stringOutputRefs_ ) delete stringOutputRefs_;
}


void InterpolatingFixedStepSizeFMU::defineRealInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != realInputRefs_ ) delete realInputRefs_;

	nRealInputs_ = nInputs;
	realInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		realInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::defineIntegerInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != integerInputRefs_ ) delete integerInputRefs_;

	nIntegerInputs_ = nInputs;
	integerInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		integerInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::defineBooleanInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != booleanInputRefs_ ) delete booleanInputRefs_;

	nBooleanInputs_ = nInputs;
	booleanInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		booleanInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::defineStringInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != stringInputRefs_ ) delete stringInputRefs_;

	nStringInputs_ = nInputs;
	stringInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		stringInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::defineRealOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != realOutputRefs_ ) delete realOutputRefs_;

	nRealOutputs_ = nOutputs;
	realOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		realOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::defineIntegerOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != integerOutputRefs_ ) delete integerOutputRefs_;

	nIntegerOutputs_ = nOutputs;
	integerOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		integerOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::defineBooleanOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != booleanOutputRefs_ ) delete booleanOutputRefs_;

	nBooleanOutputs_ = nOutputs;
	booleanOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		booleanOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::defineStringOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != stringOutputRefs_ ) delete stringOutputRefs_;

	nStringOutputs_ = nOutputs;
	stringOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		stringOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::setInitialInputs( const std::string realVariableNames[],
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


void InterpolatingFixedStepSizeFMU::getOutputs( fmiReal* outputs ) const
{
	for ( size_t i = 0; i < nRealOutputs_; ++i ) {
		fmu_->getValue( realOutputRefs_[i], outputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::getOutputs( fmiInteger* outputs ) const
{
	for ( size_t i = 0; i < nIntegerOutputs_; ++i ) {
		fmu_->getValue( integerOutputRefs_[i], outputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::getOutputs( fmiBoolean* outputs ) const
{
	for ( size_t i = 0; i < nBooleanOutputs_; ++i ) {
		fmu_->getValue( booleanOutputRefs_[i], outputs[i] );
	}
}


void InterpolatingFixedStepSizeFMU::getOutputs( std::string* outputs ) const
{
	for ( size_t i = 0; i < nStringOutputs_; ++i ) {
		fmu_->getValue( stringOutputRefs_[i], outputs[i] );
	}
}


int InterpolatingFixedStepSizeFMU::init( const std::string& instanceName,
			    const std::string realVariableNames[],
			    const fmiReal* realValues,
			    const std::size_t nRealVars,
			    const fmiTime startTime,
			    const fmiTime communicationStepSize,
			    const fmiBoolean stopTimeDefined,
			    const fmiTime stopTime,
			    const fmiReal timeout,
			    const fmiBoolean visible,
			    const fmiBoolean interactive )
{
	return init( instanceName,
	      realVariableNames, realValues, nRealVars,
	      NULL, NULL, 0,
	      NULL, NULL, 0,
	      NULL, NULL, 0,
	      startTime, communicationStepSize,
	      stopTimeDefined, stopTime,
	      timeout, visible, interactive );
}


int InterpolatingFixedStepSizeFMU::init( const std::string& instanceName,
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
			    const fmiTime communicationStepSize,
			    const fmiBoolean stopTimeDefined,
			    const fmiTime stopTime,
			    const fmiReal timeout,
			    const fmiBoolean visible,
			    const fmiBoolean interactive )
{
	assert( timeout >= 0. );
	assert( communicationStepSize > 0. );

	fmiStatus status = fmu_->instantiate( instanceName, timeout, visible, interactive, fmiFalse );

	if ( status != fmiOK ) return 0;

	// Set inputs.
	setInitialInputs( realVariableNames, realValues, nRealVars,
			  integerVariableNames, integerValues, nIntegerVars,
			  booleanVariableNames, booleanValues, nBooleanVars,
			  stringVariableNames, stringValues, nStringVars );

	// Intialize FMU.
	if ( fmu_->initialize( startTime, stopTimeDefined, stopTime ) != fmiOK ) return 0;

	HistoryEntry initState( startTime, 0, nRealOutputs_, nIntegerOutputs_, nBooleanOutputs_, nStringOutputs_ );

	getOutputs( initState.realValues_ );
	getOutputs( initState.integerValues_ );
	getOutputs( initState.booleanValues_ );
	getOutputs( initState.stringValues_ );

	previousState_ = initState;
	currentState_ = initState;
	nextState_ = initState;

	currentCommunicationPoint_ = startTime;
	communicationStepSize_ = communicationStepSize;

	return 1;  /* return 1 on success, 0 on failure */
}


/** Interpolate FMU state between two steps. **/
void InterpolatingFixedStepSizeFMU::interpolateCurrentState( fmiTime t )
{
	for ( size_t i = 0; i < nRealOutputs_; ++i ) {
		currentState_.realValues_[i] = interpolateValue( t, previousState_.time_, previousState_.realValues_[i], nextState_.time_, nextState_.realValues_[i] );
	}

	currentState_.time_ = t;
}


/* Linear value interpolation. */
fmiReal InterpolatingFixedStepSizeFMU::interpolateValue( fmiReal x, fmiReal x0, fmiReal y0, fmiReal x1, fmiReal y1 ) const
{
	return y0 + (x - x0)*(y1 - y0)/(x1 - x0);
}


fmiTime InterpolatingFixedStepSizeFMU::sync( fmiTime t0, fmiTime t1 )
{
	if ( t1 >= currentCommunicationPoint_ )
	{
		previousState_ = nextState_;
		currentState_ = previousState_;

		fmiStatus status = fmu_->doStep( currentCommunicationPoint_, communicationStepSize_, fmiTrue );

		if ( fmiOK != status ) {
			fmu_->logger( status, "SYNC", "doStep(...) failed" );
			return status;
		}

		currentCommunicationPoint_ += communicationStepSize_;

		nextState_.time_ = currentCommunicationPoint_;
		getOutputs( nextState_.realValues_ );
		getOutputs( nextState_.integerValues_ );
		getOutputs( nextState_.booleanValues_ );
		getOutputs( nextState_.stringValues_ );
	}

	interpolateCurrentState( t1 );

	return currentCommunicationPoint_;
}


/* Note that the inputs are set at the _end_ of the interval [t0, t1]. */
fmiTime InterpolatingFixedStepSizeFMU::sync( fmiTime t0, fmiTime t1,
					     fmiReal* realInputs, fmiInteger* integerInputs,
					     fmiBoolean* booleanInputs, std::string* stringInputs )
{
	fmiTime returnTime = sync( t0, t1 );

	// Set the new inputs.
	setInputs( realInputs );
	setInputs( integerInputs );
	setInputs( booleanInputs );
	setInputs( stringInputs );

	return returnTime;
}


fmiStatus InterpolatingFixedStepSizeFMU::setInputs(fmiReal* inputs) const {

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nRealInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( realInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


fmiStatus InterpolatingFixedStepSizeFMU::setInputs(fmiInteger* inputs) const {

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nIntegerInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( integerInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


fmiStatus InterpolatingFixedStepSizeFMU::setInputs(fmiBoolean* inputs) const {

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nBooleanInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( booleanInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


fmiStatus InterpolatingFixedStepSizeFMU::setInputs(std::string* inputs) const {

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nStringInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( stringInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}
