// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file FixedStepSizeFMU.cpp 
 */ 

#include <cassert>
#include <sstream>

#include "import/base/include/FMUCoSimulation_v1.h"
#include "import/base/include/FMUCoSimulation_v2.h"
#include "import/base/include/ModelManager.h"

#include "import/utility/include/FixedStepSizeFMU.h"

using namespace std;

FixedStepSizeFMU::FixedStepSizeFMU( const fmippString& fmuDirUri,
		const fmippString& modelIdentifier,
		const fmippBoolean loggingOn,
		const fmippTime timeDiffResolution ) :
	currentCommunicationPoint_( numeric_limits<fmippTime>::quiet_NaN() ),
	finalCommunicationPoint_( numeric_limits<fmippTime>::quiet_NaN() ),
	communicationStepSize_( numeric_limits<fmippTime>::quiet_NaN() ),	fmu_( 0 ),
	realInputRefs_( 0 ), integerInputRefs_( 0 ), booleanInputRefs_( 0 ), stringInputRefs_( 0 ),
	nRealInputs_( 0 ), nIntegerInputs_( 0 ), nBooleanInputs_( 0 ), nStringInputs_( 0 ),
	realOutputRefs_( 0 ), integerOutputRefs_( 0 ), booleanOutputRefs_( 0 ), stringOutputRefs_( 0 ),
	nRealOutputs_( 0 ), nIntegerOutputs_( 0 ), nBooleanOutputs_( 0 ), nStringOutputs_( 0 ),
	loggingOn_( loggingOn )
{
	// Load the FMU.
	FMUType fmuType = invalid;
	ModelManager::LoadFMUStatus loadStatus = ModelManager::loadFMU( modelIdentifier, fmuDirUri, loggingOn, fmuType );

	if ( ( ModelManager::success != loadStatus ) && ( ModelManager::duplicate != loadStatus ) ) { // Loading the FMU failed.
		fmu_ = 0;
		return;
	}
	
	if ( fmi_1_0_cs == fmuType ) // FMI CS 1.0
	{
		fmu_ = new fmi_1_0::FMUCoSimulation( modelIdentifier, loggingOn, timeDiffResolution );
	}
	else if ( ( fmi_2_0_cs == fmuType ) || ( fmi_2_0_me_and_cs == fmuType ) ) // FMI ME 2.0
	{
		fmu_ = new fmi_2_0::FMUCoSimulation( modelIdentifier, loggingOn, timeDiffResolution );		
	}
}

FixedStepSizeFMU::~FixedStepSizeFMU()
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

void FixedStepSizeFMU::defineRealInputs( const fmippString inputs[], const size_t nInputs )
{
	if ( 0 != realInputRefs_ ) delete realInputRefs_;

	nRealInputs_ = nInputs;
	realInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
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

void FixedStepSizeFMU::defineIntegerInputs( const fmippString inputs[], const size_t nInputs )
{
	if ( 0 != integerInputRefs_ ) delete integerInputRefs_;

	nIntegerInputs_ = nInputs;
	integerInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
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

void FixedStepSizeFMU::defineBooleanInputs( const fmippString inputs[], const size_t nInputs )
{
	if ( 0 != booleanInputRefs_ ) delete booleanInputRefs_;

	nBooleanInputs_ = nInputs;
	booleanInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
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

void FixedStepSizeFMU::defineStringInputs( const fmippString inputs[], const size_t nInputs )
{
	if ( 0 != stringInputRefs_ ) delete stringInputRefs_;

	nStringInputs_ = nInputs;
	stringInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
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

void FixedStepSizeFMU::defineRealOutputs( const fmippString outputs[], const size_t nOutputs )
{
	if ( 0 != realOutputRefs_ ) delete realOutputRefs_;

	nRealOutputs_ = nOutputs;
	realOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
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

void FixedStepSizeFMU::defineIntegerOutputs( const fmippString outputs[], const size_t nOutputs )
{
	if ( 0 != integerOutputRefs_ ) delete integerOutputRefs_;

	nIntegerOutputs_ = nOutputs;
	integerOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
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

void FixedStepSizeFMU::defineBooleanOutputs( const fmippString outputs[], const size_t nOutputs )
{
	if ( 0 != booleanOutputRefs_ ) delete booleanOutputRefs_;

	nBooleanOutputs_ = nOutputs;
	booleanOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
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

void FixedStepSizeFMU::defineStringOutputs( const fmippString outputs[], const size_t nOutputs )
{
	if ( 0 != stringOutputRefs_ ) delete stringOutputRefs_;

	nStringOutputs_ = nOutputs;
	stringOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
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

void FixedStepSizeFMU::setInitialInputs( const fmippString realVariableNames[],
					 const fmippReal* realValues,
					 size_t nRealVars,
					 const fmippString integerVariableNames[],
					 const fmippInteger* integerValues,
					 size_t nIntegerVars,
					 const fmippString booleanVariableNames[],
					 const fmippBoolean* booleanValues,
					 size_t nBooleanVars,
					 const fmippString fmippStringVariableNames[],
					 const fmippString* fmippStringValues,
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
		fmu_->setValue(fmippStringVariableNames[i], fmippStringValues[i]);
	}
}

void FixedStepSizeFMU::getOutputs( fmippReal* outputs ) const
{
	for ( size_t i = 0; i < nRealOutputs_; ++i ) {
		fmu_->getValue( realOutputRefs_[i], outputs[i] );
	}
}

void FixedStepSizeFMU::getOutputs( fmippInteger* outputs ) const
{
	for ( size_t i = 0; i < nIntegerOutputs_; ++i ) {
		fmu_->getValue( integerOutputRefs_[i], outputs[i] );
	}
}

void FixedStepSizeFMU::getOutputs( fmippBoolean* outputs ) const
{
	for ( size_t i = 0; i < nBooleanOutputs_; ++i ) {
		fmu_->getValue( booleanOutputRefs_[i], outputs[i] );
	}
}

void FixedStepSizeFMU::getOutputs( fmippString* outputs ) const
{
	for ( size_t i = 0; i < nStringOutputs_; ++i ) {
		fmu_->getValue( stringOutputRefs_[i], outputs[i] );
	}
}

int FixedStepSizeFMU::init( const fmippString& instanceName,
			    const fmippString realVariableNames[],
			    const fmippReal* realValues,
			    const size_t nRealVars,
			    const fmippTime startTime,
			    const fmippTime communicationStepSize,
			    const fmippBoolean stopTimeDefined,
			    const fmippTime stopTime,
			    const fmippTime timeout,
			    const fmippBoolean visible,
			    const fmippBoolean interactive )
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

int FixedStepSizeFMU::init( const fmippString& instanceName,
			    const fmippString realVariableNames[],
			    const fmippReal* realValues,
			    const size_t nRealVars,
			    const fmippString integerVariableNames[],
			    const fmippInteger* integerValues,
			    const size_t nIntegerVars,
			    const fmippString booleanVariableNames[],
			    const fmippBoolean* booleanValues,
			    const size_t nBooleanVars,
			    const fmippString fmippStringVariableNames[],
			    const fmippString* fmippStringValues,
			    const size_t nStringVars,
			    const fmippTime startTime,
			    const fmippTime communicationStepSize,
			    const fmippBoolean stopTimeDefined,
			    const fmippTime stopTime,
			    const fmippTime timeout,
			    const fmippBoolean visible,
			    const fmippBoolean interactive )
{
	assert( timeout >= 0. );
	assert( communicationStepSize > 0. );

	fmippStatus status = fmu_->instantiate( instanceName, timeout, visible, interactive );

	if ( status != fmippOK ) return 0;

	// Set inputs.
	setInitialInputs( realVariableNames, realValues, nRealVars,
			  integerVariableNames, integerValues, nIntegerVars,
			  booleanVariableNames, booleanValues, nBooleanVars,
			  fmippStringVariableNames, fmippStringValues, nStringVars );

	// Intialize FMU.
	if ( fmu_->initialize( startTime, stopTimeDefined, stopTime ) != fmippOK ) return 0;

	HistoryEntry initState( startTime, 0, nRealOutputs_, nIntegerOutputs_, nBooleanOutputs_, nStringOutputs_ );

	getOutputs( initState.realValues_ );
	getOutputs( initState.integerValues_ );
	getOutputs( initState.booleanValues_ );
	getOutputs( initState.stringValues_ );

	currentState_ = initState;

	currentCommunicationPoint_ = startTime;
	communicationStepSize_ = communicationStepSize;
	finalCommunicationPoint_ = ( stopTimeDefined == fmippTrue ) ? stopTime : INVALID_FMI_TIME;

	return 1;  /* return 1 on success, 0 on failure */
}

fmippTime FixedStepSizeFMU::sync( fmippTime t0, fmippTime t1 )
{
	if ( fmippTrue == loggingOn_ )
	{
		stringstream msg;
		msg << "syncing FMU - t0 = " << t0 << ", t1 = " << t1;
		fmu_->sendDebugMessage( msg.str() );
	}

	if ( ( t1 >= currentCommunicationPoint_ + communicationStepSize_ ) &&
	     ( currentCommunicationPoint_ < finalCommunicationPoint_ ) )
	{
		do
		{
			fmippStatus status = fmu_->doStep( currentCommunicationPoint_, communicationStepSize_, fmippTrue );

			if ( fmippOK != status ) {
				stringstream message;
				message << "doStep( " << currentCommunicationPoint_ 
					<< ", " << communicationStepSize_
					<< ", fmippTrue ) failed - status = " << status << std::endl;
				fmu_->sendDebugMessage( message.str() );
				return currentCommunicationPoint_;
			}

			currentCommunicationPoint_ += communicationStepSize_;

			currentState_.time_ = currentCommunicationPoint_;
			getOutputs( currentState_.realValues_ );
			getOutputs( currentState_.integerValues_ );
			getOutputs( currentState_.booleanValues_ );
			getOutputs( currentState_.stringValues_ );
		}
		while ( t1 >= ( currentCommunicationPoint_ + communicationStepSize_ ) );
	}

	return getNextSyncTime( t1 );
}

/* Note that the inputs are set at the _end_ of the interval [t0, t1]. */
fmippTime FixedStepSizeFMU::sync( fmippTime t0, fmippTime t1,
	fmippReal* realInputs, fmippInteger* integerInputs,
	fmippBoolean* booleanInputs, fmippString* stringInputs,
	fmippBoolean iterateOnce )
{
	fmippTime returnTime = sync( t0, t1 );

	if ( fmippTrue == loggingOn_ ) fmu_->sendDebugMessage( "syncing FMU with inputs" );

	// Set the new inputs.
	// \FIXME Should this function issue a warning/exception in case an input is a null pointer but the number of defined inputs is not zero? Or should it be quietly tolerated that there are sometimes no inputs?
	if ( 0 != realInputs ) setInputs( realInputs );
	if ( 0 != integerInputs ) setInputs( integerInputs );
	if ( 0 != booleanInputs ) setInputs( booleanInputs );
	if ( 0 != stringInputs ) setInputs( stringInputs );

	if ( ( fmippTrue == iterateOnce ) && 
	     ( t1 == currentCommunicationPoint_ ) ) this->iterateOnce();

	return returnTime;
}

void FixedStepSizeFMU::iterateOnce()
{
	fmippStatus status = fmu_->doStep( currentCommunicationPoint_, 0., fmippTrue );

	if ( fmippOK != status ) {
		stringstream message;
		message << "doStep( " << currentCommunicationPoint_
			<< ", 0., fmippTrue ) failed - status = " << status << std::endl;
		fmu_->sendDebugMessage( message.str() );
	}

	currentState_.time_ = currentCommunicationPoint_;
	getOutputs( currentState_.realValues_ );
	getOutputs( currentState_.integerValues_ );
	getOutputs( currentState_.booleanValues_ );
	getOutputs( currentState_.stringValues_ );
}

fmippStatus
FixedStepSizeFMU::getLastStatus() const
{
	if ( 0 == fmu_ ) return fmippFatal;
	return fmu_->getLastStatus();
}

fmippTime FixedStepSizeFMU::getNextSyncTime( const fmippTime& currentSyncTime ) const
{
	return ( currentSyncTime < currentCommunicationPoint_ ) ?
		currentCommunicationPoint_ : 
		currentCommunicationPoint_ + communicationStepSize_;
}

fmippStatus FixedStepSizeFMU::setInputs( fmippReal* inputs ) const
{
	fmippStatus status = fmippOK;

	for ( size_t i = 0; i < nRealInputs_; ++i ) {
		if ( fmippOK != fmu_->setValue( realInputRefs_[i], inputs[i]) ) status = fmippError;

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set real input " << realInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}

fmippStatus FixedStepSizeFMU::setInputs( fmippInteger* inputs ) const
{

	fmippStatus status = fmippOK;

	for ( size_t i = 0; i < nIntegerInputs_; ++i ) {
		if ( fmippOK != fmu_->setValue( integerInputRefs_[i], inputs[i]) ) status = fmippError;

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set integer input " << integerInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}

fmippStatus FixedStepSizeFMU::setInputs( fmippBoolean* inputs ) const
{

	fmippStatus status = fmippOK;

	for ( size_t i = 0; i < nBooleanInputs_; ++i ) {
		if ( fmippOK != fmu_->setValue( booleanInputRefs_[i], inputs[i]) ) status = fmippError;

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set boolean input " << booleanInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}

fmippStatus FixedStepSizeFMU::setInputs( fmippString* inputs ) const
{

	fmippStatus status = fmippOK;

	for ( size_t i = 0; i < nStringInputs_; ++i ) {
		if ( fmippOK != fmu_->setValue( stringInputRefs_[i], inputs[i]) ) status = fmippError;

		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set fmippString input " << stringInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}
