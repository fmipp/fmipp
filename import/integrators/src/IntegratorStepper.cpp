/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file IntegratorStepper.cpp
 * The integrator steppers that actually wrap the methods provided by Boost's ODEINT library are implemented here.
 */ 

#include <cstdio>
#include <boost/numeric/odeint.hpp>

#ifdef USE_SUNDIALS
#include <cvode/cvode.h>             /* prototypes for CVODE fcts., consts. */
#include <nvector/nvector_serial.h>  /* serial N_Vector types, fcts., macros */
#include <cvode/cvode_dense.h>       /* prototype for CVDense */
#include <sundials/sundials_dense.h> /* definitions DlsMat DENSE_ELEM */
#include <sundials/sundials_types.h> /* definition of type realtype */
#define Ith(v,i)    NV_Ith_S(v,i)       /* Ith numbers components 1..NEQ */
#endif // USE_SUNDIALS

#include "common/FMIPPConfig.h"
#include "common/fmi_v1.0/fmiModelTypes.h"

#include "import/base/include/FMUModelExchangeBase.h"

#include "import/integrators/include/IntegratorStepper.h"

using namespace boost::numeric::odeint;

typedef Integrator::state_type state_type;


IntegratorStepper::~IntegratorStepper() {}


/// Forward Euler method with constant step size.
class Euler : public IntegratorStepper
{
	// Runge-Kutta 4 stepper.
	euler< state_type > stepper;

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
{
		// Integrator function with constant step size.
		integrate_const( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::eu; }
};


/// 4th order Runge-Kutta method with constant step size.
class RungeKutta : public IntegratorStepper
{
	// Runge-Kutta 4 stepper.
	runge_kutta4< state_type > stepper;

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Integrator function with constant step size.
		integrate_const( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::rk; }
};


/// 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
class CashKarp : public IntegratorStepper
{
	// Runge-Kutta-Dormand-Prince controlled stepper.
	typedef runge_kutta_cash_karp54< state_type > error_stepper_type;
	typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
	controlled_stepper_type stepper;

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::ck; }
};


/// 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
class DormandPrince : public IntegratorStepper
{
	// Runge-Kutta-Dormand-Prince controlled stepper.
	typedef runge_kutta_dopri5< state_type > error_stepper_type;
	typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
	controlled_stepper_type stepper;

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::dp; }
};


/// 7th order Runge-Kutta-Fehlberg method with controlled step size.
class Fehlberg : public IntegratorStepper
{
	// Runge-Kutta-Fehlberg controlled stepper.
	typedef runge_kutta_fehlberg78< state_type > error_stepper_type;
	typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
	controlled_stepper_type stepper;

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::fe; }
};


/// Bulirsch-Stoer method with controlled step size.
class BulirschStoer : public IntegratorStepper
{
	// Bulirsch-Stoer controlled stepper.
	typedef bulirsch_stoer< state_type > controlled_stepper_type;
	controlled_stepper_type stepper;

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::bs; }
};


/// Adams-Bashforth-Moulton multistep method with adjustable order and adaptive step size.
class AdamsBashforthMoulton : public IntegratorStepper
{

	// Adams-Bashforth-Moulton stepper, first argument is the order of the method.
	adams_bashforth_moulton< 5, state_type > abm;

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Integrator function with adaptive step size.
		integrate_adaptive( abm, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::abm; }
};


#ifdef USE_SUNDIALS
class BackwardDifferentiationFormula : public IntegratorStepper
{

	int NEQ_;                             // dimension of state space
	N_Vector states_N_;                   // states in N_Vector format
	realtype t_bak_;                      // backup time
	realtype reltol_, abstol_;            // tolerances for the stepper
	state_type states_bak_;               // backup states
	void *cvode_mem_;                     // memory of the stepper. This memory later stores
	                                      // the RHS, states, time and buffer datas for the
	                                      // multistep methods
public:

	BackwardDifferentiationFormula( FMUModelExchangeBase* fmu )
	{
		NEQ_ = fmu->nStates();
		states_N_ = N_VNew_Serial( NEQ_ );
		reltol_   = 1e-10;
		abstol_   = 1e-50;
	}

	~BackwardDifferentiationFormula()
	{
		N_VDestroy_Serial( states_N_ );
	}

	static int f( realtype t, N_Vector x, N_Vector dx, void *user_data )
	{
		Integrator* fmuint = (Integrator*) user_data;
		int NEQ = NV_LENGTH_S( x );
		state_type x_S( NEQ );
		state_type dx_S( NEQ );

		for ( int i = 0; i < NEQ; i++ ) {
			x_S[ i ] = Ith( x, i );
			dx_S[ i ] = Ith( dx, i );
		}
			
		fmuint->rhs(x_S, dx_S, t);

		for ( int i = 0; i < NEQ; i++ ) {
			Ith( dx, i ) = dx_S[ i ];
		}
			
		return 0 ;
	}

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Write states into N_Vector format
		for ( int i = 0; i < NEQ_; i++ ) {
			Ith( states_N_ , i ) = states[ i ];
		}

		// set up CVode \TODO only delete and set up again if necessary
		if ( true ) {
			// choose solution procedure
			cvode_mem_ = CVodeCreate( CV_BDF, CV_NEWTON );
			//cvode_mem = CVodeCreate( CV_ADAMS, CV_FUNCTIONAL );
			//cvode_mem = CVodeCreate( CV_ADAMS, CV_NEWTON );
			//cvode_mem = CVodeCreate( CV_BDF, CV_FUNCTIONAL );

			// set the solver as (void*) user_data
			CVodeSetUserData( cvode_mem_, fmuint );

			//// set initial conditions and RHS
			CVodeInit( cvode_mem_, f, time, states_N_ );

			// set tolerances
			CVodeSStolerances( cvode_mem_ ,reltol_ ,abstol_ );

			// Detrmine which procedure to use for linear equations. Since the jacobean is dense,
			// CVDense is the choice here.
			CVDense( cvode_mem_, NEQ_ );

			//CVodeSetErrFile( cvode_mem, NULL ); // suppress error messages
		}
		// set initial step size
		CVodeSetInitStep( cvode_mem_, dt );

		// create backup states and backup time
		t_bak_      = time;
		states_bak_ = states;

		// make iteration
		CVode( cvode_mem_, time + step_size, states_N_, &time, CV_NORMAL );
		for ( int i = 0; i < NEQ_; i++ ) {
			states[i] = Ith( states_N_, i );
		}

		// revert states and time in case an event happened
		if ( fmuint->getIntEvent( time, states ) ){
			states = states_bak_;
			time   = t_bak_;
		}

		// write solution into model
		fmuint->rhs( states, states_bak_, time );

		// free memory
		CVodeFree( &cvode_mem_ );
	}

	virtual IntegratorType type() const { return IntegratorType::bdf; }
};
#endif


IntegratorStepper* IntegratorStepper::createStepper( IntegratorType type, FMUModelExchangeBase* fmu )
{
	switch ( type ) {
	case IntegratorType::eu : return new Euler;
	case IntegratorType::rk : return new RungeKutta;
	case IntegratorType::ck : return new CashKarp;
	case IntegratorType::dp : return new DormandPrince;
	case IntegratorType::fe : return new Fehlberg;
	case IntegratorType::bs : return new BulirschStoer;
	case IntegratorType::abm : return new AdamsBashforthMoulton;
#ifdef USE_SUNDIALS
	case IntegratorType::bdf : return new BackwardDifferentiationFormula( fmu );
#endif
	}

	return 0;
}
