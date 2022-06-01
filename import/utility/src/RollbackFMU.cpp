// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file RollbackFMU.cpp
 */

#ifdef FMI_DEBUG
#include <iostream>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <cassert>
#include <limits>

#include "import/base/include/ModelDescription.h"
#include "import/base/include/ModelManager.h"

#include "import/utility/include/RollbackFMU.h"

using namespace std;

RollbackFMU::RollbackFMU( const fmippString& fmuDirUri,
		const fmippString& modelIdentifier,
		const fmippBoolean loggingOn,
		const fmippReal timeDiffResolution,
		const IntegratorType integratorType ) :
	fmu_( 0 ),
	rollbackState_(),
	rollbackStateSaved_( false )
{
	// Load the FMU.
	FMUType fmuType = invalid;
	ModelManager::LoadFMUStatus loadStatus = ModelManager::loadFMU( modelIdentifier, fmuDirUri, loggingOn, fmuType );

	if ( ( ModelManager::success != loadStatus ) && ( ModelManager::duplicate != loadStatus ) ) { // Loading the FMU failed.
		fmu_ = 0;
		return;
	}

	if ( fmi_1_0_me == fmuType ) // FMI ME 1.0
	{
		fmu_ = new fmi_1_0::FMUModelExchange( modelIdentifier, loggingOn, fmippTrue, timeDiffResolution, integratorType );
	}
	else if ( ( fmi_2_0_me == fmuType ) || ( fmi_2_0_me_and_cs == fmuType ) ) // FMI ME 2.0
	{
		fmu_ = new fmi_2_0::FMUModelExchange( modelIdentifier, loggingOn, fmippTrue, timeDiffResolution, integratorType );
	}

	// create history entry
	rollbackState_ = HistoryEntry( fmu_->getTime(), fmu_->nStates(), 0, 0, 0, 0 );
}

RollbackFMU::~RollbackFMU() {
	if ( 0 != fmu_ ) delete fmu_;
}

fmippReal RollbackFMU::integrate( fmippReal tstop, unsigned int nsteps )
{
	fmippTime now = fmu_->getTime();

	if ( tstop < now ) { // Make a rollback.
		if ( fmippOK != rollback( tstop ) ) return now;
	} else if ( false == rollbackStateSaved_ ) { // Retrieve current state and store it as rollback state.
		rollbackState_.time_ = now;
		if ( 0 != fmu_->nStates() ) fmu_->getContinuousStates( rollbackState_.state_ );
	}

	// Integrate.
	assert( nsteps > 0 );
	double deltaT = ( tstop - fmu_->getTime() ) / nsteps;
	return fmu_->integrate( tstop, deltaT );
}

fmippReal RollbackFMU::integrate( fmippReal tstop, fmippTime deltaT )
{
	fmippTime now = fmu_->getTime();

	if ( tstop < now ) { // Make a rollback.
		if ( fmippOK != rollback( tstop ) ) return now;
	} else if ( false == rollbackStateSaved_ ) { // Retrieve current state and store it as rollback state.
		rollbackState_.time_ = now;
		if ( 0 != fmu_->nStates() ) fmu_->getContinuousStates( rollbackState_.state_ );
	}

	// Integrate.
	return fmu_->integrate( tstop, deltaT );
}

/** Saves the current state of the FMU as internal rollback
    state. This rollback state will not be overwritten until
    "releaseRollbackState()" is called; **/
void RollbackFMU::saveCurrentStateForRollback()
{
	if ( false == rollbackStateSaved_ ) {
		rollbackState_.time_ = fmu_->getTime();
		if ( 0 != fmu_->nStates() )
			fmu_->getContinuousStates( rollbackState_.state_ );

		rollbackStateSaved_ = true;
	}
}

/** Realease an internal rollback state, that was previously
    saved via "saveCurrentStateForRollback()". **/
void RollbackFMU::releaseRollbackState()
{
	rollbackStateSaved_ = false;
}

fmippStatus RollbackFMU::rollback( fmippTime time )
{
	if ( time < rollbackState_.time_ ) {
		return fmippFatal;
	}

	fmu_->setTime( rollbackState_.time_ );
	fmu_->raiseEvent();
	fmu_->handleEvents();

	if ( 0 != fmu_->nStates() ) {
		fmu_->setContinuousStates( rollbackState_.state_ );
		fmu_->raiseEvent();
	}

	fmu_->handleEvents();

	return fmippOK;
}

/// Getter function for real variables
fmippStatus RollbackFMU::getValue( const fmippString& name, fmippReal& val )
{
	return fmu_->getValue( name, val );
}


/// Getter function for integer variables
fmippStatus RollbackFMU::getValue( const fmippString& name, fmippInteger& val )
{
	return fmu_->getValue( name, val );
}


/// Getter function for boolean variables
fmippStatus RollbackFMU::getValue( const fmippString& name, fmippBoolean& val )
{
	return fmu_->getValue( name, val );
}


/// Getter function for string variables
fmippStatus RollbackFMU::getValue( const fmippString& name, fmippString& val )
{
	return fmu_->getValue( name, val );
}


/// Get single value of type fmippReal, using the variable name.
fmippReal RollbackFMU::getRealValue( const fmippString& name )
{
	return fmu_->getRealValue( name );
}


/// Get single value of type fmippInteger, using the variable name.
fmippInteger RollbackFMU::getIntegerValue( const fmippString& name )
{
	return fmu_->getIntegerValue( name );
}


/// Get single value of type fmippBoolean, using the variable name.
fmippBoolean RollbackFMU::getBooleanValue( const fmippString& name )
{
	return fmu_->getBooleanValue( name );
}


/// Get single value of type fmippString, using the variable name.
fmippString RollbackFMU::getStringValue( const fmippString& name )
{
	return fmu_->getStringValue( name );
}


/// Setter function for real variables
fmippStatus RollbackFMU::setValue( const fmippString& name, fmippReal val )
{
	return fmu_->setValue( name, val );
}


/// Setter function for string variables
fmippStatus RollbackFMU::setValue( const fmippString& name, fmippInteger val )
{
	return fmu_->setValue( name, val );
}


/// Setter function for boolean variables
fmippStatus RollbackFMU::setValue( const fmippString& name, fmippBoolean val )
{
	return fmu_->setValue( name, val );
}


/// Setter function for string variables
fmippStatus RollbackFMU::setValue( const fmippString& name, fmippString val )
{
	return fmu_->setValue( name, val );
}


fmippStatus RollbackFMU::initialize( const bool toleranceDefined, const double tolerance )
{
	return fmu_->initialize( toleranceDefined, tolerance );
}


fmippTime RollbackFMU::getTime()
{
	return fmu_->getTime();
}


fmippStatus RollbackFMU::instantiate( const fmippString& instanceName )
{
	return fmu_->instantiate( instanceName );
}


/// Get the status of the last operation on the FMU.
fmippStatus RollbackFMU::getLastStatus() const
{
	if ( 0 == fmu_ ) return fmippFatal;
	return fmu_->getLastStatus();
}
