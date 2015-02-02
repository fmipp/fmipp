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
class SundialsStepper : public IntegratorStepper
{

private:

        static int f( realtype t, N_Vector x, N_Vector dx, void *user_data )
        {
                FMUModelExchangeBase* fmu = (FMUModelExchangeBase*) user_data;
		int NEQ = NV_LENGTH_S( x );
		state_type x_S( NEQ );
		state_type dx_S( NEQ );

		for ( int i = 0; i < NEQ; i++ ) {
		        x_S[ i ] = Ith( x, i );
		}
		fmu->setTime( t );
		fmu->setContinuousStates( &x_S.front() );
		fmu->getDerivatives( &dx_S.front() );
		
		for ( int i = 0; i < NEQ; i++ ) {
		        Ith( dx, i ) = dx_S[ i ];
		}
		return 0 ;
  }

        static int g( fmiReal t, N_Vector x, fmiReal *eventsind, void *user_data )
        {
		FMUModelExchangeBase* fmu = (FMUModelExchangeBase*)user_data;
		int NEQ = NV_LENGTH_S( x );
		state_type x_S( NEQ );
		for ( int i = 0; i < NEQ; i++ )
			x_S[i] = Ith( x, i );
		fmu->setTime( t );
		fmu->setContinuousStates( &x_S.front() );
		return fmu->getEventIndicators( eventsind );
	}

  
	const int NEQ_;                       // dimension of state space
	const int NEV_;                       // number of event indicators
	N_Vector states_N_;                   // states in N_Vector format
	realtype t_;                          // internal time
	const realtype reltol_, abstol_;      // tolerances for the stepper
	void *cvode_mem_;                     // memory of the stepper. This memory later stores
	                                      // the RHS, states, time and buffer datas for the
	                                      // multistep methods
	FMUModelExchangeBase* fmu_;           // pointer to the fmu to be integrated

  
public:

  SundialsStepper( FMUModelExchangeBase* fmu, bool bdf = true ):
		NEQ_( fmu->nStates() ),
		NEV_(fmu->nEventInds() ),
		states_N_( N_VNew_Serial( NEQ_ ) ),
		reltol_( 1e-10 ),
		abstol_( 1e-10 ),
		cvode_mem_( 0 ),
		fmu_( fmu )
	{
		// choose solution procedure
		if ( bdf )
			cvode_mem_ = CVodeCreate( CV_BDF, CV_NEWTON );
		else
			cvode_mem_ = CVodeCreate( CV_ADAMS, CV_NEWTON );
		
		// other possible options are not available even tough they perform well
		//   *  cvode_mem = CVodeCreate( CV_ADAMS, CV_FUNCTIONAL );
		//   *  cvode_mem = CVodeCreate( CV_BDF, CV_FUNCTIONAL );

		// set the fmu as (void*) user_data
		CVodeSetUserData( cvode_mem_, fmu_ );

		// set initial conditions and RHS
		CVodeInit( cvode_mem_, f, t_, states_N_ );

		// pass the event indicators as root function
		CVodeRootInit( cvode_mem_, NEV_, g );

		// set tolerances
		CVodeSStolerances( cvode_mem_ ,reltol_ ,abstol_ );

		// Detrmine which procedure to use for linear equations. Since the jacobean is dense,
		// CVDense is the choice here.
		CVDense( cvode_mem_, NEQ_ );

		//CVodeSetErrFile( cvode_mem, NULL ); // suppress error messages
	}

	~SundialsStepper()
	{
		N_VDestroy_Serial( states_N_ );
	}


	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// \TODO: add more proper event handling to sundials: currently, time events are
		//        just checked at the begining of each invokeMethod call. Step events are
		//        completely ignored.

		// in case of a time event, adjust the communication step size and tell the FMUME about it
		if( time + step_size > fmu_->getTimeEvent() ){
			step_size = fmu_->getTimeEvent() - time - 1.0e-14 ;
			fmu_->setEventFlag( fmiTrue );
			fmu_->failedIntegratorStep( fmu_->getTimeEvent() );
		}

		// write input into internal time
		t_ = time;
	
		// Write states into N_Vector format
		for ( int i = 0; i < NEQ_; i++ ) {
			Ith( states_N_ , i ) = states[ i ];
		}

		// reinitialize cvode. this deletes internal memeory
		CVodeReInit( cvode_mem_, t_, states_N_ );     // \TODO: reset only if states changed externally

		// set initial step size
		CVodeSetInitStep( cvode_mem_, dt );

		// make iteration
		int flag = CVode( cvode_mem_, t_ + step_size, states_N_, &t_, CV_NORMAL );

		// convert output of cvode in state_type format
		for ( int i = 0; i < NEQ_; i++ ) {
			states[i] = Ith( states_N_, i );
		}

		if ( flag == CV_ROOT_RETURN ){
			// an event happened -> make sure to return a state before the event.
			state_type dx( NEQ_ );
			// \TODO: make rewind dependend on eventSearchPrecision_
			double rewind = 1e-10;
			(*fmuint)( states, dx, time );
			for ( int i = 0; i < NEQ_; i++ ){
				states[i] -= rewind*dx[i];
			}
			t_ -= rewind;
			// wrtite solution into the fmu
			fmu_->setTime( t_ );
			fmu_->setContinuousStates( &states.front() );
			// tell FMUModelexchange the EventHorizon
			fmu_->completedIntegratorStep();
			fmu_->failedIntegratorStep( t_ + 2*rewind );
			fmu_->setEventFlag( fmiTrue );
			return;
		}
		else{
			// no event happened -> just write the result into the fmu
			fmu_->setTime( t_ );
			fmu_->setContinuousStates( &states.front() );
		}
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
	case IntegratorType::bdf : return new SundialsStepper( fmu, 1 );
	case IntegratorType::abm2: return new SundialsStepper( fmu, 0 );
#endif
	}

	return 0;
}
