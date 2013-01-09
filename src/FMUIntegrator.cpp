#include <boost/numeric/odeint.hpp> // FIXME: Include necessary headers explicitly to save compilation time.
#include <iostream>
#include <assert.h>

#include "FMUIntegrator.h"

using namespace std;
using namespace boost::numeric::odeint;

typedef FMUIntegrator::state_type state_type;


class StepperBase
{
public:
	virtual void invokeMethod( FMUIntegrator* fmuint, state_type& states,
				   fmiReal time, fmiReal step_size, size_t n_steps ) = 0;
};


// 4th order Runge-Kutta method with constant step size.
class RungeKutta : public StepperBase
{
public:
	void invokeMethod( FMUIntegrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, size_t n_steps )
	{
		// Runge-Kutta 4 stepper.
		static runge_kutta4< state_type > stepper; // Static: initialize only once.

		// Integrator function with constant step size.
		integrate_const( stepper, *fmuint, states, time, time+step_size, step_size/n_steps, *fmuint );
	}
};


// 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
class DormandPrince : public StepperBase
{
public:
	void invokeMethod( FMUIntegrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, size_t n_steps )
	{
  //cout << "[debug:stepper:invokeMethod] invode method" << endl;
		// Runge-Kutta-Dormand-Prince controlled stepper.
		typedef runge_kutta_dopri5< state_type > error_stepper_type;
		typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

  //cout << "[debug:stepper:invokeMethod] integrate_adaptive" << endl;
		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, step_size/n_steps, *fmuint );
  //cout << "[debug:stepper:invokeMethod] done" << endl;
	}
};


// 7th order Runge-Kutta-Fehlberg method with controlled step size.
class Fehlberg : public StepperBase
{
public:
	void invokeMethod( FMUIntegrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, size_t n_steps )
	{
		// Runge-Kutta-Fehlberg controlled stepper.
		typedef runge_kutta_fehlberg78< state_type > error_stepper_type;
		typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type; 
		static controlled_stepper_type stepper; // Static: initialize only once.
  
		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, step_size/n_steps, *fmuint );
	}
};


// Bulirsch-Stoer method with controlled step size.
class BulirschStoer : public StepperBase
{
public:
	void invokeMethod( FMUIntegrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, size_t n_steps )
	{
		// Bulirsch-Stoer controlled stepper.
		typedef bulirsch_stoer< state_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, step_size/n_steps, *fmuint );
	}
};



// FIXME: Doesn't work properly, something with the step size?
// Adams-Bashforth-Moulton multistep method with adjustable order and adaptive step size.
class AdamsBashforthMoulton : public StepperBase
{
public:
	void invokeMethod( FMUIntegrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, size_t n_steps )
	{
		// Adams-Bashforth-Moulton stepper, first argument is the order of the method.
		adams_bashforth_moulton< 5, state_type > abm; // Static: initialize only once.

		// Initialization step for the multistep method.
		abm.initialize( *fmuint, states, time, step_size/n_steps );

		// Integrator function with adaptive step size.
		integrate_adaptive( abm, *fmuint, states, time, time+step_size, step_size/n_steps, *fmuint );
	}
};



FMUIntegrator::FMUIntegrator( FMU* fmu, IntegratorType type ) :
	fmu_( fmu ),
	type_( type ),
	nstates_( fmu_->nStateVars_ ),
	states_( state_type( nstates_ ) ),
	stepper_( 0 )
{
	switch ( type_ ) {
	case rk: stepper_ = new RungeKutta; break;
	case dp: stepper_ = new DormandPrince; break;
	case fe: stepper_ = new Fehlberg; break;
	case bs: stepper_ = new BulirschStoer; break;
	case abm: stepper_ = new AdamsBashforthMoulton; break;
	}

	assert( 0 != stepper_ );
}


FMUIntegrator::~FMUIntegrator()
{
	delete stepper_;
}


void FMUIntegrator::operator()( const state_type& x, state_type& dx, fmiReal time )
{
	// Update cstates_ with current states saved in vector x.
	for ( size_t i = 0; i < nstates_; ++i ) fmu_->cstates_[i] = x[i];

	// Update to current time.
	fmu_->fmuFun_->setTime( fmu_->instance_, time );

	// Update to current states.
	fmu_->fmuFun_->setContinuousStates( fmu_->instance_, fmu_->cstates_, nstates_ );

	// Evaluate derivatives and save them in deriv.
	fmu_->fmuFun_->getDerivatives( fmu_->instance_, fmu_->derivatives_, nstates_ );

	// Give the values of derivatives_ to the vector dx.
	for ( size_t i = 0; i < nstates_; ++i ) dx[i] = fmu_->derivatives_[i];
}


void FMUIntegrator::operator()( const state_type& state, fmiReal time )
{
	// Handle events.
	fmu_->handleEvents( fmu_->time_, true );
}


void FMUIntegrator::integrate( fmiReal step_size, size_t n_steps )
{

  //cout << "[debug:integrator::integrate] copy values" << endl;

	// Copy values of cstates_ to states_.
	for ( size_t i = 0; i < nstates_; ++i ) states_[i] = fmu_->cstates_[i];

	//cout << "[debug:integrator::integrate] fmu_->time_ = " << fmu_->time_ << endl;

	//cout << "[debug:integrator::integrate] invode method, stepper_ = " << stepper_ << " - fmu_ = " << fmu_ << endl;


	stepper_->invokeMethod( this, states_, fmu_->time_, step_size, n_steps );

  //cout << "[debug:integrator::integrate] copy back" << endl;

	// Copy the values of states_ to cstates_.
	for ( size_t i = 0; i < nstates_; ++i ) fmu_->cstates_[i] = states_[i];
}
