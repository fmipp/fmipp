// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
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


FixedStepSizeFMU::FixedStepSizeFMU( const std::string& fmuDirUri,
		const std::string& modelIdentifier,
		const fmiBoolean loggingOn,
		const fmiReal timeDiffResolution ) :
	currentCommunicationPoint_( numeric_limits<fmiReal>::quiet_NaN() ),
	finalCommunicationPoint_( numeric_limits<fmiTime>::quiet_NaN() ),
	communicationStepSize_( numeric_limits<fmiReal>::quiet_NaN() ),	fmu_( 0 ),
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


void FixedStepSizeFMU::defineRealInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != realInputRefs_ ) delete realInputRefs_;

	nRealInputs_ = nInputs;
	realInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		realInputRefs_[i] = fmu_->getValueRef( inputs[i] );

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << inputs[i] << " (" << realInputRefs_[i] << ") "
			    << "to real input variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}


void FixedStepSizeFMU::defineIntegerInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != integerInputRefs_ ) delete integerInputRefs_;

	nIntegerInputs_ = nInputs;
	integerInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		integerInputRefs_[i] = fmu_->getValueRef( inputs[i] );

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << inputs[i] << " (" << integerInputRefs_[i] << ") "
			    << "to integer input variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}


void FixedStepSizeFMU::defineBooleanInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != booleanInputRefs_ ) delete booleanInputRefs_;

	nBooleanInputs_ = nInputs;
	booleanInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		booleanInputRefs_[i] = fmu_->getValueRef( inputs[i] );

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << inputs[i] << " (" << booleanInputRefs_[i] << ") "
			    << "to boolean input variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}


void FixedStepSizeFMU::defineStringInputs( const string inputs[], const size_t nInputs )
{
	if ( 0 != stringInputRefs_ ) delete stringInputRefs_;

	nStringInputs_ = nInputs;
	stringInputRefs_ = new fmiValueReference[nInputs];
	for ( size_t i = 0; i < nInputs; ++i ) {
		stringInputRefs_[i] = fmu_->getValueRef( inputs[i] );

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << inputs[i] << " (" << stringInputRefs_[i] << ") "
			    << "to string input variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}


void FixedStepSizeFMU::defineRealOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != realOutputRefs_ ) delete realOutputRefs_;

	nRealOutputs_ = nOutputs;
	realOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		realOutputRefs_[i] = fmu_->getValueRef( outputs[i] );

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << outputs[i] << " (" << realOutputRefs_[i] << ") "
			    << "to real output variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}


void FixedStepSizeFMU::defineIntegerOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != integerOutputRefs_ ) delete integerOutputRefs_;

	nIntegerOutputs_ = nOutputs;
	integerOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		integerOutputRefs_[i] = fmu_->getValueRef( outputs[i] );

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << outputs[i] << " (" << integerOutputRefs_[i] << ") "
			    << "to integer output variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}


void FixedStepSizeFMU::defineBooleanOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != booleanOutputRefs_ ) delete booleanOutputRefs_;

	nBooleanOutputs_ = nOutputs;
	booleanOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		booleanOutputRefs_[i] = fmu_->getValueRef( outputs[i] );

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << outputs[i] << " (" << booleanOutputRefs_[i] << ") "
			    << "to boolean output variables";
			fmu_->sendDebugMessage( msg.str() );
		}
	}
}


void FixedStepSizeFMU::defineStringOutputs( const string outputs[], const size_t nOutputs )
{
	if ( 0 != stringOutputRefs_ ) delete stringOutputRefs_;

	nStringOutputs_ = nOutputs;
	stringOutputRefs_ = new fmiValueReference[nOutputs];
	for ( size_t i = 0; i < nOutputs; ++i ) {
		stringOutputRefs_[i] = fmu_->getValueRef( outputs[i] );

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "add " << outputs[i] << " (" << stringOutputRefs_[i] << ") "
			    << "to string output variables";
			fmu_->sendDebugMessage( msg.str() );
		}
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

	fmiStatus status = fmu_->instantiate( instanceName, timeout, visible, interactive );

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
	finalCommunicationPoint_ = ( stopTimeDefined == fmiTrue ) ? stopTime : INVALID_FMI_TIME;

	return 1;  /* return 1 on success, 0 on failure */
}


fmiTime FixedStepSizeFMU::sync( fmiTime t0, fmiTime t1 )
{
	if ( fmiTrue == loggingOn_ )
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
			fmiStatus status = fmu_->doStep( currentCommunicationPoint_, communicationStepSize_, fmiTrue );

			if ( fmiOK != status ) {
				stringstream message;
				message << "doStep( " << currentCommunicationPoint_ 
					<< ", " << communicationStepSize_
					<< ", fmiTrue ) failed - status = " << status << std::endl;
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
fmiTime FixedStepSizeFMU::sync( fmiTime t0, fmiTime t1,
	fmiReal* realInputs, fmiInteger* integerInputs,
	fmiBoolean* booleanInputs, string* stringInputs,
	fmiBoolean iterateOnce )
{
	fmiTime returnTime = sync( t0, t1 );

	if ( fmiTrue == loggingOn_ ) fmu_->sendDebugMessage( "syncing FMU with inputs" );

	// Set the new inputs.
	// \FIXME Should this function issue a warning/exception in case an input is a null pointer but the number of defined inputs is not zero? Or should it be quietly tolerated that there are sometimes no inputs?
	if ( 0 != realInputs ) setInputs( realInputs );
	if ( 0 != integerInputs ) setInputs( integerInputs );
	if ( 0 != booleanInputs ) setInputs( booleanInputs );
	if ( 0 != stringInputs ) setInputs( stringInputs );

	if ( ( fmiTrue == iterateOnce ) && 
	     ( t1 == currentCommunicationPoint_ ) ) this->iterateOnce();

	return returnTime;
}


void FixedStepSizeFMU::iterateOnce()
{
	fmiStatus status = fmu_->doStep( currentCommunicationPoint_, 0., fmiTrue );

	if ( fmiOK != status ) {
		stringstream message;
		message << "doStep( " << currentCommunicationPoint_
			<< ", 0., fmiTrue ) failed - status = " << status << std::endl;
		fmu_->sendDebugMessage( message.str() );
	}

	currentState_.time_ = currentCommunicationPoint_;
	getOutputs( currentState_.realValues_ );
	getOutputs( currentState_.integerValues_ );
	getOutputs( currentState_.booleanValues_ );
	getOutputs( currentState_.stringValues_ );
}


fmiStatus
FixedStepSizeFMU::getLastStatus() const
{
	if ( 0 == fmu_ ) return fmiFatal;
	return fmu_->getLastStatus();
}


fmiTime FixedStepSizeFMU::getNextSyncTime( const fmiTime& currentSyncTime ) const
{
	return ( currentSyncTime < currentCommunicationPoint_ ) ?
		currentCommunicationPoint_ : 
		currentCommunicationPoint_ + communicationStepSize_;
}


fmiStatus FixedStepSizeFMU::setInputs( fmiReal* inputs ) const
{
	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nRealInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( realInputRefs_[i], inputs[i]) ) status = fmiError;

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set real input " << realInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}


fmiStatus FixedStepSizeFMU::setInputs( fmiInteger* inputs ) const
{

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nIntegerInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( integerInputRefs_[i], inputs[i]) ) status = fmiError;

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set integer input " << integerInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}


fmiStatus FixedStepSizeFMU::setInputs( fmiBoolean* inputs ) const
{

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nBooleanInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( booleanInputRefs_[i], inputs[i]) ) status = fmiError;

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set boolean input " << booleanInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}


fmiStatus FixedStepSizeFMU::setInputs( string* inputs ) const
{

	fmiStatus status = fmiOK;

	for ( size_t i = 0; i < nStringInputs_; ++i ) {
		if ( fmiOK != fmu_->setValue( stringInputRefs_[i], inputs[i]) ) status = fmiError;

		if ( fmiTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "set string input " << stringInputRefs_[i] << " = " << inputs[i];
			fmu_->sendDebugMessage( msg.str() );
		}
	}

	return status;
}
