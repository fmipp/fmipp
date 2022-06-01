// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file VariableStepSizeFMU.cpp 
 */

#include <cmath>
#include <cassert>
#include <sstream>

#include "import/base/include/FMUCoSimulation_v1.h"
#include "import/base/include/FMUCoSimulation_v2.h"
#include "import/base/include/ModelManager.h"

#include "import/utility/include/VariableStepSizeFMU.h"

using namespace std;

VariableStepSizeFMU::VariableStepSizeFMU( const fmippString& fmuDirUri,
		const fmippString& modelIdentifier,
		const fmippBoolean loggingOn,
		const fmippTime timeDiffResolution ) :
	currentCommunicationPoint_( numeric_limits<fmippTime>::quiet_NaN() ),
	finalCommunicationPoint_( numeric_limits<fmippTime>::quiet_NaN() ),
	defaultCommunicationStepSize_( numeric_limits<fmippTime>::quiet_NaN() ), fmu_( 0 ),
	realInputRefs_( 0 ), integerInputRefs_( 0 ), booleanInputRefs_( 0 ), stringInputRefs_( 0 ),
	nRealInputs_( 0 ), nIntegerInputs_( 0 ), nBooleanInputs_( 0 ), nStringInputs_( 0 ),
	realOutputRefs_( 0 ), integerOutputRefs_( 0 ), booleanOutputRefs_( 0 ), stringOutputRefs_( 0 ),
	nRealOutputs_( 0 ), nIntegerOutputs_( 0 ), nBooleanOutputs_( 0 ), nStringOutputs_( 0 ),
	loggingOn_( loggingOn ), timeDiffResolution_( timeDiffResolution )
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

VariableStepSizeFMU::~VariableStepSizeFMU()
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

void VariableStepSizeFMU::defineRealInputs( const fmippString inputs[], const fmippSize nInputs )
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

void VariableStepSizeFMU::defineIntegerInputs( const fmippString inputs[], const fmippSize nInputs )
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

void VariableStepSizeFMU::defineBooleanInputs( const fmippString inputs[], const fmippSize nInputs )
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

void VariableStepSizeFMU::defineStringInputs( const fmippString inputs[], const fmippSize nInputs )
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

void VariableStepSizeFMU::defineRealOutputs( const fmippString outputs[], const fmippSize nOutputs )
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

void VariableStepSizeFMU::defineIntegerOutputs( const fmippString outputs[], const fmippSize nOutputs )
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

void VariableStepSizeFMU::defineBooleanOutputs( const fmippString outputs[], const fmippSize nOutputs )
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

void VariableStepSizeFMU::defineStringOutputs( const fmippString outputs[], const fmippSize nOutputs )
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

void VariableStepSizeFMU::setInitialInputs( const fmippString realVariableNames[],
					 const fmippReal* realValues,
					 fmippSize nRealVars,
					 const fmippString integerVariableNames[],
					 const fmippInteger* integerValues,
					 fmippSize nIntegerVars,
					 const fmippString booleanVariableNames[],
					 const fmippBoolean* booleanValues,
					 fmippSize nBooleanVars,
					 const fmippString stringVariableNames[],
					 const fmippString* stringValues,
					 fmippSize nStringVars )
{
	for ( fmippSize i = 0; i < nRealVars; ++i ) {
		fmu_->setValue(realVariableNames[i], realValues[i]);
	}
	for ( fmippSize i = 0; i < nIntegerVars; ++i ) {
		fmu_->setValue(integerVariableNames[i], integerValues[i]);
	}
	for ( fmippSize i = 0; i < nBooleanVars; ++i ) {
		fmu_->setValue(booleanVariableNames[i], booleanValues[i]);
	}
	for ( fmippSize i = 0; i < nStringVars; ++i ) {
		fmu_->setValue(stringVariableNames[i], stringValues[i]);
	}
}

void VariableStepSizeFMU::getOutputs( fmippReal* outputs ) const
{
	for ( fmippSize i = 0; i < nRealOutputs_; ++i ) {
		fmu_->getValue( realOutputRefs_[i], outputs[i] );
	}
}

void VariableStepSizeFMU::getOutputs( fmippInteger* outputs ) const
{
	for ( fmippSize i = 0; i < nIntegerOutputs_; ++i ) {
		fmu_->getValue( integerOutputRefs_[i], outputs[i] );
	}
}

void VariableStepSizeFMU::getOutputs( fmippBoolean* outputs ) const
{
	for ( fmippSize i = 0; i < nBooleanOutputs_; ++i ) {
		fmu_->getValue( booleanOutputRefs_[i], outputs[i] );
	}
}

void VariableStepSizeFMU::getOutputs( fmippString* outputs ) const
{
	for ( fmippSize i = 0; i < nStringOutputs_; ++i ) {
		fmu_->getValue( stringOutputRefs_[i], outputs[i] );
	}
}

int VariableStepSizeFMU::init( const string& instanceName,
			    const fmippString realVariableNames[],
			    const fmippReal* realValues,
			    const fmippSize nRealVars,
			    const fmippTime startTime,
			    const fmippTime defaultCommunicationStepSize,
			    const fmippBoolean stopTimeDefined,
			    const fmippTime stopTime,
			    const fmippReal timeout,
			    const fmippBoolean visible,
			    const fmippBoolean interactive )
{
	return init( instanceName,
	      realVariableNames, realValues, nRealVars,
	      NULL, NULL, 0,
	      NULL, NULL, 0,
	      NULL, NULL, 0,
	      startTime, defaultCommunicationStepSize,
	      stopTimeDefined, stopTime,
	      timeout, visible, interactive );
}

int VariableStepSizeFMU::init( const string& instanceName,
			    const fmippString realVariableNames[],
			    const fmippReal* realValues,
			    const fmippSize nRealVars,
			    const fmippString integerVariableNames[],
			    const fmippInteger* integerValues,
			    const fmippSize nIntegerVars,
			    const fmippString booleanVariableNames[],
			    const fmippBoolean* booleanValues,
			    const fmippSize nBooleanVars,
			    const fmippString stringVariableNames[],
			    const fmippString* stringValues,
			    const fmippSize nStringVars,
			    const fmippTime startTime,
			    const fmippTime defaultCommunicationStepSize,
			    const fmippBoolean stopTimeDefined,
			    const fmippTime stopTime,
			    const fmippReal timeout,
			    const fmippBoolean visible,
			    const fmippBoolean interactive )
{
	assert( timeout >= 0. );
	assert( defaultCommunicationStepSize > 0. );

	fmippStatus status = fmu_->instantiate( instanceName, timeout, visible, interactive );

	if ( status != fmippOK ) return 0;

	if ( false == fmu_->canHandleVariableCommunicationStepSize() )
	{
		fmippString message( "the FMU cannot handle variable communication step sizes (according to model description)" );
		fmu_->sendDebugMessage( message );
		return 0;
	}
	
	// Set inputs.
	setInitialInputs( realVariableNames, realValues, nRealVars,
			  integerVariableNames, integerValues, nIntegerVars,
			  booleanVariableNames, booleanValues, nBooleanVars,
			  stringVariableNames, stringValues, nStringVars );

	// Intialize FMU.
	if ( fmu_->initialize( startTime, stopTimeDefined, stopTime ) != fmippOK ) return 0;

	HistoryEntry initState( startTime, 0, nRealOutputs_, nIntegerOutputs_, nBooleanOutputs_, nStringOutputs_ );

	getOutputs( initState.realValues_ );
	getOutputs( initState.integerValues_ );
	getOutputs( initState.booleanValues_ );
	getOutputs( initState.stringValues_ );

	currentState_ = initState;

	currentCommunicationPoint_ = startTime;
	defaultCommunicationStepSize_ = defaultCommunicationStepSize;
	finalCommunicationPoint_ = ( stopTimeDefined == fmippTrue ) ? stopTime : INVALID_FMI_TIME;

	return 1;  /* return 1 on success, 0 on failure */
}

fmippTime VariableStepSizeFMU::sync( fmippTime t0, fmippTime t1 )
{
	if ( fmippTrue == loggingOn_ )
	{
		stringstream msg;
		msg << "syncing FMU - t0 = " << t0 << ", t1 = " << t1;
		fmu_->sendDebugMessage( msg.str() );
	}

	if ( t0 > t1 )
	{
		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "invalid input (t0 > t1)" << endl;
			fmu_->sendDebugMessage( msg.str() );
		}
	
		return fmiWarning;
	}

	if ( fabs( t0 - currentCommunicationPoint_ ) > timeDiffResolution_ )
	{
		if ( fmippTrue == loggingOn_ )
		{
			stringstream msg;
			msg << "time t0 does not correspond to the current communication point (" 
				<< currentCommunicationPoint_ << ")" << endl;
			fmu_->sendDebugMessage( msg.str() );
		}
	
		return fmiWarning;
	}
	
	fmippStatus status = fmu_->doStep( currentCommunicationPoint_, t1 - t0, fmippTrue );
	
	if ( fmippOK != status )
	{
		stringstream msg;
		msg << "doStep( " << currentCommunicationPoint_
			<< ", " << (t1 - t0)
			<< ", fmippTrue ) failed - status = " << status << std::endl;
		fmu_->sendDebugMessage( msg.str() );
		return currentCommunicationPoint_;
	}

	currentCommunicationPoint_ = t1;

	currentState_.time_ = currentCommunicationPoint_;
	getOutputs( currentState_.realValues_ );
	getOutputs( currentState_.integerValues_ );
	getOutputs( currentState_.booleanValues_ );
	getOutputs( currentState_.stringValues_ );

	return getNextSyncTime( t1 );
}

/* Note that the inputs are set at the _end_ of the interval [t0, t1]. */
fmippTime VariableStepSizeFMU::sync( fmippTime t0, fmippTime t1,
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

void VariableStepSizeFMU::iterateOnce()
{
	fmippStatus status = fmu_->doStep( currentCommunicationPoint_, 0., fmippTrue );

	if ( fmippOK != status )
	{
		stringstream msg;
		msg << "doStep( " << currentCommunicationPoint_
			<< ", 0., fmippTrue ) failed - status = " << status << std::endl;
		fmu_->sendDebugMessage( msg.str() );
	}

	currentState_.time_ = currentCommunicationPoint_;
	getOutputs( currentState_.realValues_ );
	getOutputs( currentState_.integerValues_ );
	getOutputs( currentState_.booleanValues_ );
	getOutputs( currentState_.stringValues_ );
}

fmippStatus
VariableStepSizeFMU::getLastStatus() const
{
	if ( 0 == fmu_ ) return fmippFatal;
	return fmu_->getLastStatus();
}

fmippTime VariableStepSizeFMU::getNextSyncTime( const fmippTime& currentSyncTime ) const
{
	return currentCommunicationPoint_ + defaultCommunicationStepSize_;
}

fmippStatus VariableStepSizeFMU::setInputs( fmippReal* inputs ) const
{
	fmippStatus status = fmippOK;

	for ( fmippSize i = 0; i < nRealInputs_; ++i ) {
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

fmippStatus VariableStepSizeFMU::setInputs( fmippInteger* inputs ) const
{

	fmippStatus status = fmippOK;

	for ( fmippSize i = 0; i < nIntegerInputs_; ++i ) {
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

fmippStatus VariableStepSizeFMU::setInputs( fmippBoolean* inputs ) const
{

	fmippStatus status = fmippOK;

	for ( fmippSize i = 0; i < nBooleanInputs_; ++i ) {
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

fmippStatus VariableStepSizeFMU::setInputs( fmippString* inputs ) const
{

	fmippStatus status = fmippOK;

	for ( fmippSize i = 0; i < nStringInputs_; ++i ) {
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
