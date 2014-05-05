/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


/**
 * \file FMUIntegrator.cpp 
 */ 

#include <cstdio>
#include <cassert>
#include <limits>
#include "FMUIntegrator.h"
#include "FMUIntegratorStepper.h"


using namespace std;

/**
 * 
 * @param[in] fmu an FMU model to be integrated 
 * @param[in] type integerator method
 */
FMUIntegrator::FMUIntegrator( FMUBase* fmu, IntegratorType type ) :
	fmu_( fmu ),
	stepper_( FMUIntegratorStepper::createStepper( type ) ),
	states_( fmu_->nStates(), std::numeric_limits<fmiReal>::quiet_NaN() ),
	time_( std::numeric_limits<fmiReal>::quiet_NaN() ),
	is_copy_( false )
{
	assert( 0 != stepper_ );
}


FMUIntegrator::FMUIntegrator( const FMUIntegrator& other ) :
	fmu_( other.fmu_ ),
	stepper_( other.stepper_ ),
	states_( other.states_ ),
	time_( other.time_ ),
	is_copy_( true )
{
}


FMUIntegrator::~FMUIntegrator()
{
	// Internally, ODEINT makes copies of this class (when using
	// boost::fusion::for_each). This copies can however all use
	// the same stepper. Only when the destructor of the original
	// instance is called, the stepper is deleted.
	if ( false == is_copy_ ) delete stepper_;
}


FMUIntegrator::IntegratorType
FMUIntegrator::type() const
{
	return stepper_->type();
}


// System function (right hand side of ODE).
void
FMUIntegrator::operator()( const state_type& x, state_type& dx, fmiReal time )
{
	// In case there has been an event, then the integrator shall do nothing
	if ( fmiFalse == fmu_->getIntEvent() ) {

		// Update to current time.
		fmu_->setTime( time );

		// Update to current states.
		fmu_->setContinuousStates( &x.front() );

		// Check if a state event has occured.
		fmu_->checkStateEvent();

		// In case no immediate event has occured, check for other possibilities.
		if ( fmiFalse == fmu_->getIntEvent() ) {
			fmu_->handleEvents( time );
		}

		// Evaluate derivatives and store them to vector dx.
		fmu_->getDerivatives( &dx.front() );
	}
}


// Observer.
void
FMUIntegrator::operator()( const state_type& state, fmiReal time )
{
	// In case there has been an event, then the integrator shall do nothing.
	if ( fmiFalse == fmu_->getIntEvent() ) {


		// Update to current time.
		fmu_->setTime( time );

		// Update to current states.
		fmu_->setContinuousStates( &state.front() );

		// Check if a state event has occured.
		fmu_->checkStateEvent();

		// In case no immediate event has occured, check for other possibilities.
		if ( fmiFalse == fmu_->getIntEvent() ) {
			fmu_->handleEvents( time );

			// In contrast to the system function (see above), the
			// observer is responsible for accepting integrator steps.
			fmu_->completedIntegratorStep();

			// Save current state and time.
			states_ = state;
			time_ = time;
		}
	} else {
		// Reset to last known valid state.
		fmu_->setTime( time_ );
		fmu_->setContinuousStates( &states_.front() );
	}
}


void
FMUIntegrator::integrate( fmiReal step_size, fmiReal dt )
{
	// Get current time.
	time_ = fmu_->getTime();

	// Get current continuous states.
	fmu_->getContinuousStates( &states_.front() );

	// Invoke integration method.
  	stepper_->invokeMethod( this, states_, fmu_->getTime(), step_size, dt );
}


FMUIntegrator* FMUIntegrator::clone() const
{
	return new FMUIntegrator( this->fmu_, this->type() );
}
