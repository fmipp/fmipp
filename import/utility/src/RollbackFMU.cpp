/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

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

#include "import/utility/include/RollbackFMU.h"


using namespace std;


// RollbackFMU::RollbackFMU( const string& modelName ) :
// 	FMU( modelName ),
// 	rollbackState_( getTime(), nStates(), 0, 0, 0, 0 ),
// 	rollbackStateSaved_( false )
// {
// #ifdef FMI_DEBUG
// 	cout << "[RollbackFMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
// #endif
// }


RollbackFMU::RollbackFMU( const string& fmuPath,
			  const string& modelName ) :
	fmu_( new fmi_1_0::FMUModelExchange( fmuPath, modelName ) ),
	// \todo: check wether fmu_ == 0 before calling member functions
	rollbackState_( fmu_->getTime(), fmu_->nStates(), 0, 0, 0, 0 ),
	rollbackStateSaved_( false )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif
}


RollbackFMU::RollbackFMU( const string& xmlPath,
			  const string& dllPath,
			  const string& modelName ) :
	fmu_( new fmi_1_0::FMUModelExchange( xmlPath, dllPath, modelName ) ),
	// \todo: check wether fmu_ == 0 before calling member functions
	rollbackState_( fmu_->getTime(), fmu_->nStates(), 0, 0, 0, 0 ),
	rollbackStateSaved_( false )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::ctor] MODEL_IDENTIFIER = " << modelName.c_str() << endl; fflush( stdout );
#endif
}


/**
 * \todo: implement copy constructor for FMUModelExchangeBase and RollbackFMU
 *
 * RollbackFMU::RollbackFMU( const RollbackFMU& aRollbackFMU ) :
 *	fmu_( aRollbackFMU.fmu_ ),                        // use copy constructor FMUMEBase( *fmu_ ) instead
 *	rollbackState_( aRollbackFMU.rollbackState_ ),
 *	rollbackStateSaved_( false )
 * {
 * #ifdef FMI_DEBUG
 *	cout << "[RollbackFMU::ctor]" << endl; fflush( stdout );
 * #endif
 * }
 */


RollbackFMU::~RollbackFMU() {
	if ( 0 != fmu_ )
		delete fmu_;
}


fmiReal RollbackFMU::integrate( fmiReal tstop, unsigned int nsteps )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::integrate]" << endl; fflush( stdout );
#endif
	fmiTime now = fmu_->getTime();

	if ( tstop < now ) { // Make a rollback.
		if ( fmiOK != rollback( tstop ) ) return now;
	} else if ( false == rollbackStateSaved_ ) { // Retrieve current state and store it as rollback state.
		rollbackState_.time_ = now;
		if ( 0 != fmu_->nStates() ) fmu_->getContinuousStates( rollbackState_.state_ );
	}

	// Integrate.
	assert( nsteps > 0 );
	double deltaT = ( tstop - fmu_->getTime() ) / nsteps;
	return fmu_->integrate( tstop, deltaT );
}


fmiReal RollbackFMU::integrate( fmiReal tstop, double deltaT )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::integrate]" << endl; fflush( stdout );
#endif
	fmiTime now = fmu_->getTime();

	if ( tstop < now ) { // Make a rollback.
		if ( fmiOK != rollback( tstop ) ) return now;
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

#ifdef FMI_DEBUG
		cout << "[RollbackFMU::saveCurrentStateForRollback] saved state at time = " << rollbackState_.time_ << endl; fflush( stdout );
#endif
		rollbackStateSaved_ = true;
	}
}


/** Realease an internal rollback state, that was previously
    saved via "saveCurrentStateForRollback()". **/
void RollbackFMU::releaseRollbackState()
{
	rollbackStateSaved_ = false;
}


fmiStatus RollbackFMU::rollback( fmiTime time )
{
#ifdef FMI_DEBUG
	cout << "[RollbackFMU::rollback]" << endl; fflush( stdout );
#endif

	if ( time < rollbackState_.time_ ) {
#ifdef FMI_DEBUG
		cout << "[RollbackFMU::rollback] FAILED. requested time = " << time
		     << " - rollback state time = " << rollbackState_.time_ << endl; fflush( stdout );
#endif
		return fmiFatal;
	}

	fmu_->setTime( rollbackState_.time_ );
	fmu_->raiseEvent();
	fmu_->handleEvents();

	if ( 0 != fmu_->nStates() ) {
		fmu_->setContinuousStates( rollbackState_.state_ );
		fmu_->raiseEvent();
	}

	fmu_->handleEvents();

	return fmiOK;
}


/// Getter function for real variables
fmiStatus RollbackFMU::getValue( const std::string& name, fmiReal& val )
{
	return fmu_->getValue( name, val );
}


/// Getter function for integer variables
fmiStatus RollbackFMU::getValue( const std::string& name, fmiInteger& val )
{
	return fmu_->getValue( name, val );
}


/// Getter function for boolean variables
fmiStatus RollbackFMU::getValue( const std::string& name, fmiBoolean& val )
{
	return fmu_->getValue( name, val );
}


/// Getter function for string variables
fmiStatus RollbackFMU::getValue( const std::string& name, std::string& val )
{
	return fmu_->getValue( name, val );
}

/// Setter function for real variables
fmiStatus RollbackFMU::setValue( const std::string& name, fmiReal val )
{
	return fmu_->setValue( name, val );
}


/// Setter function for string variables
fmiStatus RollbackFMU::setValue( const std::string& name, fmiInteger val )
{
	return fmu_->setValue( name, val );
}


/// Setter function for boolean variables
fmiStatus RollbackFMU::setValue( const std::string& name, fmiBoolean val )
{
	return fmu_->setValue( name, val );
}


/// Setter function for string variables
fmiStatus RollbackFMU::setValue( const std::string& name, std::string val )
{
	return fmu_->setValue( name, val );
}


fmiStatus RollbackFMU::initialize()
{
	return fmu_->initialize();
}


fmiTime RollbackFMU::getTime()
{
	return fmu_->getTime();
}


fmiStatus RollbackFMU::instantiate( const std::string& instanceName )
{
	return fmu_->instantiate( instanceName );
}
