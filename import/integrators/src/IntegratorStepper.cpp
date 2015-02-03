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

/**
 * Base class for all implementations of odeint steppers
 *
 * this class only exists to create a more structured inheritance
 */
class OdeintStepper : public IntegratorStepper
{
};

/// Forward Euler method with constant step size.
class Euler : public OdeintStepper
{
	/// Euler stepper.
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
class RungeKutta : public OdeintStepper
{
	/// Runge-Kutta 4 stepper.
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


/// 5th order Cash-Karp method with controlled step size.
class CashKarp : public OdeintStepper
{
	typedef runge_kutta_cash_karp54< state_type > error_stepper_type;
	typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
	/// Runge-Kutta-Cash-Karp controlled stepper.
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
class DormandPrince : public OdeintStepper
{
	typedef runge_kutta_dopri5< state_type > error_stepper_type;
	typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
	/// Runge-Kutta-Dormand-Prince controlled stepper.
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


/// 8th order Runge-Kutta-Fehlberg method with controlled step size.
class Fehlberg : public OdeintStepper
{
	typedef runge_kutta_fehlberg78< state_type > error_stepper_type;
	typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
	/// Runge-Kutta-Fehlberg controlled stepper.
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
class BulirschStoer : public OdeintStepper
{
	/// Bulirsch-Stoer controlled stepper.
	bulirsch_stoer< state_type > stepper;

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
class AdamsBashforthMoulton : public OdeintStepper
{

	/// Adams-Bashforth-Moulton stepper, first argument is the order of the method.
  	adams_bashforth_moulton< 8, state_type> abm;

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
/// Base class for all implementations of sundials steppers
class SundialsStepper : public IntegratorStepper
{

private:
	/**
	 * the righthandside of the ode
	 *
	 * @param[in]		t		time
	 * @param[in]		x		state
	 * @param[out]		dx		the derivatives at ( x, t )
	 * @param[in,out]	user_data	the fmu to be evaluated. The states of the fmu
	 *					are ( x, t ) after the call
	 */
	static int f( realtype t, N_Vector x, N_Vector dx, void *user_data )
        {
		// cast user_data to the right class, so we have acess to its functions
		FMUModelExchangeBase* fmu = (FMUModelExchangeBase*) user_data;

		// get the size of the problem ( NEQ = number of equations = number of continuous states )
		int NEQ = NV_LENGTH_S( x );
		
		// declare the state_type versions of x and dx
		state_type x_S( NEQ );
		state_type dx_S( NEQ );
		
		// convert x into state_type
		for ( int i = 0; i < NEQ; i++ ) {
		        x_S[ i ] = Ith( x, i );
		}
		
		// evaluate the rhs at ( x_S, t ) and save result into dx_S
		fmu->setTime( t );
		fmu->setContinuousStates( &x_S.front() );
		fmu->getDerivatives( &dx_S.front() );
		
		// convert the result back into N_Vector format
		for ( int i = 0; i < NEQ; i++ ) {
		        Ith( dx, i ) = dx_S[ i ];
		}
		// return 0 to tell CVode that everything was fine
		// \FIXME: return 1 in case one of rhe calls fmu->setStates, fmu->setTimem
		//         or fmu->getDerivatives was *not* sucessful
		return 0 ;
	}

	/**
	 * the event indicator function
	 *
	 * @param[in]		t		time
	 * @param[in]		x		state
	 * @param[out]		eventsind	the event indicators at ( x, t )
	 * @param[in,out]	user_data	the fmu to be evaluated. The states of the fmu
	 *					are ( x, t ) after the call
	 */
	static int g( fmiReal t, N_Vector x, fmiReal *eventsind, void *user_data )
	{
		// cast the user_data to the right class so we have acess to its functions
		FMUModelExchangeBase* fmu = (FMUModelExchangeBase*)user_data;

		// get the size of the problem ( NEQ = number of equations = number of continuous states )
		int NEQ = NV_LENGTH_S( x );

		// declare the state_type version of x
		state_type x_S( NEQ );

		// convert x into state_type
		for ( int i = 0; i < NEQ; i++ )
			x_S[i] = Ith( x, i );

		// write ( x_S, t ) into the fmu
		fmu->setTime( t );
		fmu->setContinuousStates( &x_S.front() );
		
		// return 0 if the call to getEventIndicators was sucessfull, otherwise return 1
		// this gives CVode the possibilities to throw errors and warnings according to
		// the behavior of the fmu
		// \TODO: also tell CVode wether setTime and setStates was sucessfull
		return fmu->getEventIndicators( eventsind );
	}

  
	const int NEQ_;				///< dimension of state space
	const int NEV_;				///< number of event indicators
	N_Vector states_N_;			///< states in N_Vector format
	realtype t_;				///< internal time
	const realtype reltol_;			///< relative tolerance
	const realtype abstol_;			///< absolute tolerance
	void *cvode_mem_;			///< memory of the stepper. This memory later stores
						///< the RHS, states, time and buffer datas for the
						///< multistep methods
	FMUModelExchangeBase* fmu_;		///< pointer to the fmu to be integrated

  
public:
	/**
	 * Constructor
	 *
	 * @param[in] fmu	 the fmu to be integrated
	 * @param[in] isBDF	 bool saying wether the bdf or the abm version is required
	 */
	SundialsStepper( FMUModelExchangeBase* fmu, bool isBDF ):
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
			
			// tell FMUModelexchange the EventHorizon ( upper and lower limit for the
			// event-time )
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

/// Backwards differentiation Formula with controlled step size. Suited for stiff problems
class BackwardsDifferentiationFormula : public SundialsStepper
{
public:
	BackwardsDifferentiationFormula( FMUModelExchangeBase* fmu ) :
		SundialsStepper( fmu, true ){};

	virtual IntegratorType type() const { return IntegratorType::bdf; }

};

/// Adams bashforth moulton formula with controlled step size and order up to 12
class AdamsBashforthMoulton2 : public SundialsStepper
{
public:
	AdamsBashforthMoulton2( FMUModelExchangeBase* fmu ) :
		SundialsStepper( fmu, false ){};

	virtual IntegratorType type() const { return IntegratorType::abm2; }
};
#endif


IntegratorStepper* IntegratorStepper::createStepper( IntegratorType type, FMUModelExchangeBase* fmu )
{
	switch ( type ) {
	case IntegratorType::eu		: return new Euler;
	case IntegratorType::rk		: return new RungeKutta;
	case IntegratorType::ck		: return new CashKarp;
	case IntegratorType::dp		: return new DormandPrince;
	case IntegratorType::fe		: return new Fehlberg;
	case IntegratorType::bs		: return new BulirschStoer;
	case IntegratorType::abm	: return new AdamsBashforthMoulton;
#ifdef USE_SUNDIALS
	case IntegratorType::bdf	: return new BackwardsDifferentiationFormula( fmu );
	case IntegratorType::abm2	: return new AdamsBashforthMoulton2( fmu );
#endif
	}

	return 0;
}
