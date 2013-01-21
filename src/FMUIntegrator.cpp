#include <iostream>
#include <cstdio>
#include <cassert>
#include "FMUIntegrator.h"
#include "FMUIntegratorStepper.h"

using namespace std;


FMUIntegrator::FMUIntegrator( FMU* fmu, IntegratorType type ) :
	fmu_( fmu ),
	stepper_( FMUIntegratorStepper::createStepper( type ) ),
	is_copy_( false )
{
	assert( 0 != stepper_ );
}


FMUIntegrator::FMUIntegrator( const FMUIntegrator& other ) :
	fmu_( other.fmu_ ),
	stepper_( other.stepper_ ),
	is_copy_( true )
{}


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


void
FMUIntegrator::operator()( const state_type& x, state_type& dx, fmiReal time )
{
	// // Update to current time.
	// fmu_->getContinuousStates( fmu_->cstates_ );

	// Update cstates_ with current states saved in vector x.
	//for ( size_t i = 0; i < fmu_->nStateVars_; ++i ) fmu_->cstates_[i] = x[i];

	// Update to current time.
	fmu_->fmuFun_->setTime( fmu_->instance_, time );

	// Update to current states.
	//fmu_->fmuFun_->setContinuousStates( fmu_->instance_, fmu_->cstates_, fmu_->nStateVars_ );
	fmu_->fmuFun_->setContinuousStates( fmu_->instance_, &x.front(), fmu_->nStateVars_ );

	// Evaluate derivatives and store them to vector dx.
 	fmu_->fmuFun_->getDerivatives( fmu_->instance_, &dx.front(), fmu_->nStateVars_ );
}


void
FMUIntegrator::operator()( const state_type& state, fmiReal time )
{
	// Call "fmiCompletedIntegratorStep" and handle events.
	fmu_->handleEvents( fmu_->time_, true );
}


void
FMUIntegrator::integrate( fmiReal step_size, size_t n_steps )
{
	// This vector holds (temporarily) the values of the FMU's continuous states.
	static state_type states( fmu_->nStateVars_ );

	// Copy values of cstates_ to states.
	//for ( size_t i = 0; i < fmu_->nStateVars_; ++i ) states[i] = fmu_->cstates_[i];

	fmu_->getContinuousStates( &states.front() );

  	stepper_->invokeMethod( this, states, fmu_->time_, step_size, n_steps );
      
	// Copy the values of states to cstates_.
	//for ( size_t i = 0; i < fmu_->nStateVars_; ++i ) fmu_->cstates_[i] = states[i];
}


FMUIntegrator* FMUIntegrator::clone() const
{
	return new FMUIntegrator( this->fmu_, this->type() );
}
