/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file IntegratorStepper.cpp
 * The integrator steppers that actually wrap the methods provided by Boost's ODEINT library and CVode
 * are implemented here.
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
#include "import/base/include/DynamicalSystem.h"
#include "import/integrators/include/IntegratorStepper.h"

#include <boost/numeric/odeint/stepper/controlled_step_result.hpp>

using namespace boost::numeric::odeint;

typedef Integrator::state_type state_type;


IntegratorStepper::~IntegratorStepper() {}

struct system_wrapper{
	FMUModelExchangeBase* ds_;
	system_wrapper( FMUModelExchangeBase* ds ) : ds_( ds ){}
	void operator()( const state_type& x, state_type& dx, double t ){
		ds_->setTime( t );
		ds_->setContinuousStates( &x[0] );
		ds_->getDerivatives( &dx[0] );
	}
};

/**
 * Base class for all implementations of odeint steppers
 *
 * this class only exists to create a more structured inheritance
 *
 * Note: just because the function integrate_adaptive is used does *not* mean, that the corresponding
 *       stepper has adaptive step size. This method is also available for non adaptive steppers and
 *       always preferaable since step_size/dt is not an integer in general.
 *       To see which steppers are adaptive and which are not, read the descriptions in this file or
 *       in ./../include/IntegratorType.hpp
 */
class OdeintStepper : public IntegratorStepper
{
	state_type states_bak_;  ///< backup states to be retrieved after an event
	double time_bak_;        ///< backup time to be retrieved after an event
protected:
	system_wrapper sys_;
public:
	/// Constructor
	OdeintStepper( int ord, FMUModelExchangeBase* fmu ) : IntegratorStepper( ord, fmu ),
							  sys_( fmu ){}

	virtual void do_step( Integrator* fmuint, state_type& states,
			      fmiTime& currentTime, fmiTime& dt ) = 0;

	virtual void do_step_const( Integrator* fmuint, state_type& states,
				    fmiTime& currentTime, fmiTime& dt ){
		do_step( fmuint, states,
			 currentTime, dt );
	}

	void invokeMethod( Integrator* fmuint,
			   Integrator::state_type& states,
			   fmiTime time,
			   fmiTime step_size,
			   fmiReal dt,
			   fmiReal eventSearchPrecision )
	{
		reset();           // \TODO: only reset if necessary, not at every call to the stepper
		fmiTime currentTime = time;
		while ( currentTime < time + step_size ){
			// make backup
			time_bak_ = currentTime;
			states_bak_ = states;

			if ( currentTime + dt >= time + step_size ){
				dt = time + step_size - currentTime;
			}

			//do_step
			do_step( fmuint, states, currentTime, dt );

			// update the state and time
			fmu_->setTime( currentTime );
			fmu_->setContinuousStates( &states[0] );

			/* check whether an int event occured
			 * this is done by the integrator since he knows the event indicators at the beginning of the
			 * integrate call
			 */
			if( fmuint->checkStateEvent() ){
				//std::cout << "STATE EVENT: " << time_bak_-.1 << " " << currentTime-.1 << std::endl;
				// a state event has occured. Inform the FMUME
				fmu_->failedIntegratorStep( currentTime );

				// set the fmu back to the backup state/time
				states = states_bak_;
				fmu_->setTime( time_bak_ );
				fmu_->setContinuousStates( &states_bak_[0] );

				// tell the integrator about the event and the event location
				fmuint->eventHappened_ = true;
				fmuint->tLower_ = time_bak_;
				fmuint->tUpper_ = currentTime;

				return;
			}
			// \TODO: check for a step event here
			/*
			fmu_->completedIntegratorStep();
			if ( fmu_->callEventUpdate() ){
				fmuint->eventHappened_ = true;   ?????
				return;                          ?????
			}
			*/
		}
		fmuint->eventHappened_ = false;
	}
};

/// Forward Euler method with constant step size.
class Euler : public OdeintStepper
{
	/// Euler stepper.
	euler< state_type > stepper;

public:
	Euler( FMUModelExchangeBase* fmu ) : OdeintStepper( 1, fmu ){}

	void do_step( Integrator* fmuint, state_type& states,
		      fmiTime& currentTime, fmiTime& dt ){
		stepper.do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}

	virtual IntegratorType type() const { return IntegratorType::eu; }
};


/// 4th order Runge-Kutta method with constant step size.
class RungeKutta : public OdeintStepper
{
	/// Runge-Kutta 4 stepper.
	runge_kutta4< state_type > stepper;

public:
	RungeKutta( FMUModelExchangeBase* fmu ) : OdeintStepper( 4, fmu ){}

	void do_step( Integrator* fmuint, state_type& states,
		      fmiTime& currentTime, fmiTime& dt ){
		stepper.do_step( sys_, states, currentTime, dt );
		currentTime += dt;
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
	controlled_step_result res_;

public:
	CashKarp( FMUModelExchangeBase* fmu ) : OdeintStepper( 5, fmu ){};

	void do_step_const( Integrator* fmuint, state_type& states,
			    fmiTime& currentTime, fmiTime& dt ){
		stepper.stepper().do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}

	void do_step( Integrator* fmuint, state_type& states,
		      fmiTime& currentTime, fmiTime& dt ){
		do {
			res_ = stepper.try_step( sys_, states, currentTime, dt );
		}
		while ( res_ == fail );
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
	controlled_step_result res_;

public:
	DormandPrince( FMUModelExchangeBase* fmu ) : OdeintStepper( 5, fmu ){};

	void do_step_const( Integrator* fmuint, state_type& states,
			    fmiTime& currentTime, fmiTime& dt ){
		stepper.stepper().do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}

	void do_step( Integrator* fmuint, state_type& states,
		      fmiTime& currentTime, fmiTime& dt ){
		do {
			res_ = stepper.try_step( sys_, states, currentTime, dt );
		}
		while ( res_ == fail );
	}

	void reset(){
		stepper = controlled_stepper_type();
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
	controlled_step_result res_;

public:
	Fehlberg( FMUModelExchangeBase* fmu ) : OdeintStepper( 8, fmu ){};

	void do_step_const( Integrator* fmuint, state_type& states,
			    fmiTime& currentTime, fmiTime& dt ){
		stepper.stepper().do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}

	void do_step( Integrator* fmuint, state_type& states,
		      fmiTime& currentTime, fmiTime& dt ){
		do {
			stepper.try_step( sys_, states, currentTime, dt );
		}
		while ( res_ == fail );
	}

	virtual IntegratorType type() const { return IntegratorType::fe; }
};


/// Bulirsch-Stoer method with controlled step size.
class BulirschStoer : public OdeintStepper
{
	/// Bulirsch-Stoer controlled stepper.
	bulirsch_stoer< state_type > stepper;
	controlled_step_result res_;

public:
	BulirschStoer( FMUModelExchangeBase* fmu ) : OdeintStepper( 0, fmu ){};

	void do_step_const( Integrator* fmuint, state_type& states,
			    fmiTime& currentTime, fmiTime& dt ){
		euler<state_type> eu;
		eu.do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}

	void do_step( Integrator* fmuint, state_type& states,
		      fmiTime& currentTime, fmiTime& dt ){
		do {
			res_ = stepper.try_step( sys_, states, currentTime, dt );
		}
		while ( res_ == fail );
	}

	void reset(){
		stepper.reset();
	}

	virtual IntegratorType type() const { return IntegratorType::bs; }
};


/// Adams-Bashforth-Moulton multistep method with adjustable order and adaptive step size.
class AdamsBashforthMoulton : public OdeintStepper
{
	/// Adams-Bashforth-Moulton stepper, first argument is the order of the method.
	adams_bashforth_moulton< 8, state_type> stepper;

public:
	AdamsBashforthMoulton( FMUModelExchangeBase* fmu ) : OdeintStepper( 8, fmu ){};

	void do_step( Integrator* fmuint, state_type& states,
		      fmiTime& currentTime, fmiTime& dt ){
		stepper.do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}
	void reset(){
		stepper = adams_bashforth_moulton< 8, state_type>();
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
		FMUModelExchangeBase* fmu = (FMUModelExchangeBase*) user_data;
		fmu->setTime( t );
		fmu->setContinuousStates( N_VGetArrayPointer(x) );
		fmu->getDerivatives( N_VGetArrayPointer(dx) );
		return 0 ;
		// \FIXME: return 1 in case one of rhe calls fmu->setContinuousStates, fmu->setTime
		//         or fmu->getDerivatives was *not* sucessful

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
		FMUModelExchangeBase* fmu = (FMUModelExchangeBase*)user_data;
		fmu->setTime( t );
		fmu->setContinuousStates( N_VGetArrayPointer( x ) );
		return fmu->getEventIndicators( eventsind );
		// \FIXME: return 1 in case one of rhe calls fmu->setContinuousStates, fmu->setTime
		//         was *not* sucessful
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

  
public:
	/**
	 * Constructor
	 *
	 * @param[in] fmu	 the fmu to be integrated
	 * @param[in] isBDF	 bool saying wether the bdf or the abm version is required
	 */
	SundialsStepper( FMUModelExchangeBase* fmu, bool isBDF ) :
		IntegratorStepper( 0, fmu ),
		NEQ_( fmu->nStates() ),
		NEV_( fmu->nEventInds() ),
		states_N_( N_VNew_Serial( NEQ_ ) ),
		reltol_( 1e-10 ),
		abstol_( 1e-10 ),
		cvode_mem_( 0 )
	{
		// choose solution procedure
		if ( isBDF )
			cvode_mem_ = CVodeCreate( CV_BDF, CV_NEWTON );
		else
			cvode_mem_ = CVodeCreate( CV_ADAMS, CV_FUNCTIONAL );
		
		// other possible options are not available even tough they perform well
		//   *  cvode_mem_ = CVodeCreate( CV_ADAMS, CV_NEWTON );
		//   *  cvode_mem_ = CVodeCreate( CV_BDF, CV_FUNCTIONAL );

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
			   fmiReal time, fmiReal step_size, fmiReal dt,
			   fmiReal eventSearchPrecision )
	{
		// \TODO: add more proper event handling to sundials: currently, time events are
		//        just checked at the begining of each invokeMethod call. Step events are
		//        completely ignored.

		// in case of a time event, adjust the communication step size and tell the FMUME about it
		/*
		if( time + step_size > fmu_->getTimeEvent() ){
			step_size = fmu_->getTimeEvent() - time - fmu_->getEventSearchPrecision()/3.0 ;
			fmu_->setEventFlag( fmiTrue );
			fmu_->failedIntegratorStep( fmu_->getTimeEvent() );
		}
		*/
		// write input into internal time
		t_ = time;
	
		// Convert states into N_Vector format
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
			fmuint->eventHappened_ = true;
			// an event happened -> make sure to return a state before the event.
			state_type dx( NEQ_ );

			// rewind the states to make sure the returned state/time is shortly *before* the
			// event. The rewinding tends to cause bugs if rewind is smaller than the precision
			// of the sundials solvers. This precision is 100 times the precision of doubles (~1e-14) 
			// according to the official documentation of CVode. However, if the fmu is coded in
			// floats it might be necessary to adapt the figure rewind
			// \TODO: test with float fmu
			double rewind = fmu_->getEventSearchPrecision()/10.0;
			if ( rewind >= 1.0e-12 ){
				std::cout << "WARNING: the specified eventsearchprecision might be too small"
					  << "for the use with sundials" << std::endl;
			}
			fmu_->setTime( t_ );
			fmu_->setContinuousStates( &states.front() );
			fmu_->getDerivatives( &dx[0] );

			for ( int i = 0; i < NEQ_; i++ ){
				states[i] -= rewind*dx[i];
			}
			t_ -= rewind;

			// wrtite solution into the fmu ( i.e. set back the time/states )
			fmu_->setTime( t_ );
			fmu_->setContinuousStates( &states.front() );
			
			// tell FMUModelexchange the EventHorizon ( upper and lower limit for the
			// state-event-time )
			fmuint->tUpper_ = t_+2*rewind;
			fmuint->tLower_ = t_;

			fmu_->completedIntegratorStep();
			fmu_->failedIntegratorStep( t_ + 2*rewind );
			fmu_->setEventFlag( fmiTrue );
			return;
		}
		else{
			fmuint->eventHappened_ = false;
			// no event happened -> just write the result into the fmu
			fmu_->setTime( t_ );
			fmu_->setContinuousStates( &states.front() );
		}
	}
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
	case IntegratorType::eu		: return new Euler( fmu );
	case IntegratorType::rk		: return new RungeKutta( fmu );
	case IntegratorType::ck		: return new CashKarp( fmu );
	case IntegratorType::dp		: return new DormandPrince( fmu );
	case IntegratorType::fe		: return new Fehlberg( fmu );
	case IntegratorType::bs		: return new BulirschStoer( fmu );
	case IntegratorType::abm	: return new AdamsBashforthMoulton( fmu );
#ifdef USE_SUNDIALS
	case IntegratorType::bdf	: return new BackwardsDifferentiationFormula( fmu );
	case IntegratorType::abm2	: return new AdamsBashforthMoulton2( fmu );
#endif
	}

	return 0;
}
