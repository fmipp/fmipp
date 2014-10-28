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

#include "import/integrators/include/IntegratorStepper.h"

using namespace boost::numeric::odeint;

typedef Integrator::state_type state_type;


IntegratorStepper::~IntegratorStepper() {}


/// Forward Euler method with constant step size.
class Euler : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
{
		// Runge-Kutta 4 stepper.
		static euler< state_type > stepper; // Static: initialize only once.

		// Integrator function with constant step size.
		integrate_const( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::eu; }
};


/// 4th order Runge-Kutta method with constant step size.
class RungeKutta : public IntegratorStepper
{

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Runge-Kutta 4 stepper.
		static runge_kutta4< state_type > stepper; // Static: initialize only once.

		// Integrator function with constant step size.
		integrate_const( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::rk; }

};


/// 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
class CashKarp : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Runge-Kutta-Dormand-Prince controlled stepper.
		typedef runge_kutta_cash_karp54< state_type > error_stepper_type;
		typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::ck; }

};


/// 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
class DormandPrince : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Runge-Kutta-Dormand-Prince controlled stepper.
		typedef runge_kutta_dopri5< state_type > error_stepper_type;
		typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::dp; }

};


/// 7th order Runge-Kutta-Fehlberg method with controlled step size.
class Fehlberg : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Runge-Kutta-Fehlberg controlled stepper.
		typedef runge_kutta_fehlberg78< state_type > error_stepper_type;
		typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::fe; }
};


/// Bulirsch-Stoer method with controlled step size.
class BulirschStoer : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Bulirsch-Stoer controlled stepper.
		typedef bulirsch_stoer< state_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::bs; }
};




/// Adams-Bashforth-Moulton multistep method with adjustable order and adaptive step size.
class AdamsBashforthMoulton : public IntegratorStepper
{

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Adams-Bashforth-Moulton stepper, first argument is the order of the method.
		adams_bashforth_moulton< 5, state_type > abm; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( abm, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::abm; }
};


#ifdef USE_SUNDIALS
class CVODE : public IntegratorStepper
{

public:

	static int f( realtype t, N_Vector x, N_Vector dx, void *user_data )
		{
			Integrator* fmuint = (Integrator*)user_data;
		        int NEQ = NV_LENGTH_S( x );
			int i;
			state_type x_S( NEQ );
			state_type dx_S( NEQ );
			for ( i=0 ; i < NEQ; i++ ){
				x_S[i] = Ith( x, i );
				dx_S[i] = Ith( dx, i );
			}
			fmuint->rhs(x_S, dx_S, t);

			for (i=0; i<NEQ; i++){
				Ith( dx, i ) = dx_S[i] ;
			}
			return(0);
		}

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		int NEQ = states.size();
		int i;
		void *cvode_mem;
		N_Vector states_N = N_VNew_Serial( NEQ );
		realtype reltol = 1e-10;
		realtype abstol = 1e-50; // 1e-50 works with all tests
		realtype t;
		state_type states2 = states;
		realtype t2;

		//// Write states into N_Vector format
		t = time;
		for ( i = 0; i < NEQ; i++ ){
			Ith( states_N , i ) = states[i];
		}

		//// choose solution procedure
		cvode_mem = CVodeCreate( CV_BDF, CV_NEWTON );
		//cvode_mem = CVodeCreate( CV_ADAMS, CV_FUNCTIONAL );
		//cvode_mem = CVodeCreate( CV_ADAMS, CV_NEWTON);
		//cvode_mem = CVodeCreate( CV_BDF, CV_FUNCTIONAL ); // segfault in fmipp_testFixedStepSizeFMU??

		// set the solver as (void*) user_data
		CVodeSetUserData( cvode_mem, fmuint );

		//// set initial conditions and RHS
		CVodeInit( cvode_mem, f, t, states_N );

		// set tolerances
		CVodeSStolerances( cvode_mem ,reltol ,abstol );

		// Detrmine which procedure to use for linear equations. Since the jacobean is dense,
		// CVDense is the choice here.
		CVDense( cvode_mem, NEQ );

		//CVodeSetErrFile( cvode_mem, NULL ); // suppress error messages

		// Make Iterations
		t2 = t;

		while (t < time + step_size - dt/2.0){
			CVode( cvode_mem, t + dt, states_N, &t, CV_NORMAL );

			for ( i = 0; i < NEQ; i++ )
				states2[i] = Ith( states_N, i );

			if ( fmuint->getIntEvent( t, states2 ) )
				break;

			states = states2;
			t2 = t;
		}

		time = t2;

		// write solution into model
		fmuint->rhs( states, states, time );
		// free memory
		N_VDestroy_Serial( states_N );
		CVodeFree( &cvode_mem );
	}

	virtual IntegratorType type() const { return IntegratorType::cv; }
};
#endif



IntegratorStepper* IntegratorStepper::createStepper( IntegratorType type )
{
	switch ( type ) {
	case IntegratorType::eu: return new Euler;
	case IntegratorType::rk: return new RungeKutta;
	case IntegratorType::ck: return new CashKarp;
	case IntegratorType::dp: return new DormandPrince;
	case IntegratorType::fe: return new Fehlberg;
	case IntegratorType::bs: return new BulirschStoer;
	case IntegratorType::abm: return new AdamsBashforthMoulton;
#ifdef USE_SUNDIALS
	case IntegratorType::cv: return new CVODE;
#endif
	}

	return 0;
}
