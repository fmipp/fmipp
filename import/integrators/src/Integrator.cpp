/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file Integrator.cpp 
 */ 

#include <cstdio>
#include <cassert>
#include <limits>

#include "common/fmi_v1.0/fmiModelTypes.h"

#include "import/base/include/FMUModelExchangeBase.h"

#include "import/integrators/include/Integrator.h"
#include "import/integrators/include/IntegratorStepper.h"


using namespace std;


Integrator::Integrator( FMUModelExchangeBase* fmu, IntegratorType type ) :
	fmu_( fmu ),
	stepper_( IntegratorStepper::createStepper( type ) ),
	states_( fmu_->nStates(), std::numeric_limits<fmiReal>::quiet_NaN() ),
	time_( std::numeric_limits<fmiReal>::quiet_NaN() ),
	is_copy_( false )
{
	assert( 0 != stepper_ );
}


Integrator::Integrator( const Integrator& other ) :
	fmu_( other.fmu_ ),
	stepper_( other.stepper_ ),
	states_( other.states_ ),
	time_( other.time_ ),
	is_copy_( true )
{
}


Integrator::~Integrator()
{
	// Internally, ODEINT makes copies of this class (when using
	// boost::fusion::for_each). This copies can however all use
	// the same stepper. Only when the destructor of the original
	// instance is called, the stepper is deleted.
	if ( false == is_copy_ ) delete stepper_;
}


IntegratorType
Integrator::type() const
{
	return stepper_->type();
}


// System function (right hand side of ODE).
void
Integrator::operator()( const state_type& x, state_type& dx, fmiReal time )
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

// System function (right hand side of ODE, this version does not checlk for events).
void
Integrator::rhs( const state_type& x, state_type& dx, fmiReal time )
{
	fmu_->setTime( time );
	fmu_->setContinuousStates( &x.front() );
	fmu_->getDerivatives( &dx.front() );
}

// For CVODE
bool Integrator::getIntEvent(fmiReal time, state_type states) {
	fmu_->setTime( time );
	fmu_->setContinuousStates( &states.front() );
	fmu_->checkStateEvent();
	if (!fmu_->getIntEvent()){
		fmu_->completedIntegratorStep();
	}else{
		fmu_->failedIntegratorStep();
	}
	return( fmu_->getIntEvent() );
}

// Observer.
void
Integrator::operator()( const state_type& state, fmiReal time )
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
Integrator::integrate( fmiReal step_size, fmiReal dt )
{
	// Get current time.
	time_ = fmu_->getTime();

	// Get current continuous states.
	fmu_->getContinuousStates( &states_.front() );

	// Invoke integration method.
  	stepper_->invokeMethod( this, states_, fmu_->getTime(), step_size, dt );
}


Integrator* Integrator::clone() const
{
	return new Integrator( this->fmu_, this->type() );
}
