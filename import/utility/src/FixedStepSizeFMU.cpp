/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


/**
 * \file FixedStepSizeFMU.cpp 
 */ 
#include <iostream> /// \FIXME remove
#include <cassert>

#include "import/base/include/FMUCoSimulation.h"

#include "import/utility/include/FixedStepSizeFMU.h"



using namespace std;


FixedStepSizeFMU::FixedStepSizeFMU( const string& fmuPath,
				    const string& modelName ) :
	currentCommunicationPoint_( numeric_limits<fmiReal>::quiet_NaN() ),
	communicationStepSize_( numeric_limits<fmiReal>::quiet_NaN() ),
	fmu_( new FMUCoSimulation( fmuPath, modelName ) ),
	realInputRefs_( 0 ), integerInputRefs_( 0 ), booleanInputRefs_( 0 ), stringInputRefs_( 0 ),
	nRealInputs_( 0 ), nIntegerInputs_( 0 ), nBooleanInputs_( 0 ), nStringInputs_( 0 ),
	realOutputRefs_( 0 ), integerOutputRefs_( 0 ), booleanOutputRefs_( 0 ), stringOutputRefs_( 0 ),
	nRealOutputs_( 0 ), nIntegerOutputs_( 0 ), nBooleanOutputs_( 0 ), nStringOutputs_( 0 )
{}


FixedStepSizeFMU::~FixedStepSizeFMU()
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


void FixedStepSizeFMU::defineRealInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != realInputRefs_ ) delete realInputRefs_;

	nRealInputs_ = nInputs;
	realInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		realInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void FixedStepSizeFMU::defineIntegerInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != integerInputRefs_ ) delete integerInputRefs_;

	nIntegerInputs_ = nInputs;
	integerInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		integerInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void FixedStepSizeFMU::defineBooleanInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != booleanInputRefs_ ) delete booleanInputRefs_;

	nBooleanInputs_ = nInputs;
	booleanInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		booleanInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void FixedStepSizeFMU::defineStringInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != stringInputRefs_ ) delete stringInputRefs_;

	nStringInputs_ = nInputs;
	stringInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		stringInputRefs_[i] = fmu_->getValueRef( inputs[i] );
	}
}


void FixedStepSizeFMU::defineRealOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != realOutputRefs_ ) delete realOutputRefs_;

	nRealOutputs_ = nOutputs;
	realOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		realOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void FixedStepSizeFMU::defineIntegerOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != integerOutputRefs_ ) delete integerOutputRefs_;

	nIntegerOutputs_ = nOutputs;
	integerOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		integerOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void FixedStepSizeFMU::defineBooleanOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != booleanOutputRefs_ ) delete booleanOutputRefs_;

	nBooleanOutputs_ = nOutputs;
	booleanOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		booleanOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void FixedStepSizeFMU::defineStringOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != stringOutputRefs_ ) delete stringOutputRefs_;

	nStringOutputs_ = nOutputs;
	stringOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		stringOutputRefs_[i] = fmu_->getValueRef( outputs[i] );
	}
}


void FixedStepSizeFMU::setInitialInputs( const string realVariableNames[],
					 const fmiReal* realValues,
					 size_t nRealVars,
					 const string integerVariableNames[],
					 const fmiInteger* integerValues,
					 size_t nIntegerVars,
					 const string booleanVariableNames[],
					 const fmiBoolean* booleanValues,
					 size_t nBooleanVars,
					 const string stringVariableNames[],
					 const string* stringValues,
					 size_t nStringVars )
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


void FixedStepSizeFMU::getOutputs( fmiReal* outputs ) const
{
	for ( size_t i = 0; i < nRealOutputs_; ++i ) {
		fmu_->getValue( realOutputRefs_[i], outputs[i] );
	}
}


void FixedStepSizeFMU::getOutputs( fmiInteger* outputs ) const
{
	for ( size_t i = 0; i < nIntegerOutputs_; ++i ) {
		fmu_->getValue( integerOutputRefs_[i], outputs[i] );
	}
}


void FixedStepSizeFMU::getOutputs( fmiBoolean* outputs ) const
{
	for ( size_t i = 0; i < nBooleanOutputs_; ++i ) {
		fmu_->getValue( booleanOutputRefs_[i], outputs[i] );
	}
}


void FixedStepSizeFMU::getOutputs( string* outputs ) const
{
	for ( size_t i = 0; i < nStringOutputs_; ++i ) {
		fmu_->getValue( stringOutputRefs_[i], outputs[i] );
	}
}


int FixedStepSizeFMU::init( const string& instanceName,
			    const string realVariableNames[],
			    const fmiReal* realValues,
			    const size_t nRealVars,
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


int FixedStepSizeFMU::init( const string& instanceName,
			    const string realVariableNames[],
			    const fmiReal* realValues,
			    const size_t nRealVars,
			    const string integerVariableNames[],
			    const fmiInteger* integerValues,
			    const size_t nIntegerVars,
			    const string booleanVariableNames[],
			    const fmiBoolean* booleanValues,
			    const size_t nBooleanVars,
			    const string stringVariableNames[],
			    const string* stringValues,
			    const size_t nStringVars,
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

	currentState_ = initState;
	currentCommunicationPoint_ = startTime;
	communicationStepSize_ = communicationStepSize;

	return 1;  /* return 1 on success, 0 on failure */
}


fmiTime FixedStepSizeFMU::sync( fmiTime t0, fmiTime t1 )
{
	if ( t1 >= currentCommunicationPoint_ )
	{
		getOutputs( currentState_.realValues_ );
		getOutputs( currentState_.integerValues_ );
		getOutputs( currentState_.booleanValues_ );
		getOutputs( currentState_.stringValues_ );

		fmiStatus status = fmu_->doStep( currentCommunicationPoint_, communicationStepSize_, fmiTrue );

		if ( fmiOK != status ) {
			fmu_->logger( status, "SYNC", "doStep(...) failed" );
			return status;
		}

		currentCommunicationPoint_ += communicationStepSize_;
	}

	currentState_.time_ = t1;

	return currentCommunicationPoint_;
}


/* Note that the inputs are set at the _end_ of the interval [t0, t1]. */
fmiTime FixedStepSizeFMU::sync( fmiTime t0, fmiTime t1,
				fmiReal* realInputs, fmiInteger* integerInputs,
				fmiBoolean* booleanInputs, string* stringInputs )
{
	fmiTime returnTime = sync( t0, t1 );

	// Set the new inputs.
	// \FIXME Should this function issue a warning/exception in case an input is a null pointer but the number of defined inputs is not zero? Or should it be quietly tolerated that there are sometimes no inputs?
	if ( 0 != realInputs ) setInputs( realInputs );
	if ( 0 != integerInputs ) setInputs( integerInputs );
	if ( 0 != booleanInputs ) setInputs( booleanInputs );
	if ( 0 != stringInputs ) setInputs( stringInputs );

	return returnTime;
}


fmiStatus FixedStepSizeFMU::setInputs(fmiReal* inputs) const {

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nRealInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( realInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


fmiStatus FixedStepSizeFMU::setInputs( fmiInteger* inputs ) const
{

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nIntegerInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( integerInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


fmiStatus FixedStepSizeFMU::setInputs( fmiBoolean* inputs ) const
{

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nBooleanInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( booleanInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}


fmiStatus FixedStepSizeFMU::setInputs( string* inputs ) const
{

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nStringInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( stringInputRefs_[i], inputs[i]) ) status = fmiError;
	}

	return status;
}
