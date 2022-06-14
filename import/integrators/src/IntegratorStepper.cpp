// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file IntegratorStepper.cpp
 * The integrator steppers that actually wrap the methods provided by Boost's ODEINT library and CVode
 * are implemented here.
 */

#include <cstdio>

// Boost Ublas type checks drastically slow down the rosenbrock4 integrator
// performance. Hence, they were disabled.
#define BOOST_UBLAS_TYPE_CHECK 0
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/odeint/stepper/controlled_step_result.hpp>

#ifdef USE_SUNDIALS
#include <cvode/cvode.h>             /* prototypes for CVODE fcts., consts. */
#include <nvector/nvector_serial.h>  /* serial N_Vector types, fcts., macros */
#include <cvode/cvode_direct.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sundials/sundials_dense.h> /* definitions DlsMat DENSE_ELEM */
#include <sundials/sundials_types.h> /* definition of type realtype */
#define Ith(v,i)    NV_Ith_S(v,i)       /* Ith numbers components 1..NEQ */
#endif // USE_SUNDIALS

#include "common/FMIPPConfig.h"
#include "common/fmi_v1.0/fmiModelTypes.h"

#include "import/base/include/FMUModelExchangeBase.h"
#include "import/base/include/DynamicalSystem.h"
#include "import/integrators/include/IntegratorStepper.h"

using namespace boost::numeric::odeint;

IntegratorStepper::~IntegratorStepper() {}

/** Wrapper around DynamicalSystem to be used by the OdeintSteppers. It fullfills odeints
    [system concept](http://www.boost.org/doc/libs/1_55_0/libs/numeric/odeint/doc/html/boost_numeric_odeint/concepts/system.html) */
struct SystemWrapper {

	DynamicalSystem* ds_;

	SystemWrapper( DynamicalSystem* ds ) : ds_( ds ){}

	void operator()( const StateType& x, StateType& dx, fmippTime t ){
		ds_->setTime( t );
		ds_->setContinuousStates( &x[0] );
		ds_->getDerivatives( &dx[0] );
	}
};

/**
 * Base class for all implementations of odeint steppers
 *
 * The event detection is done by this class and the derived classes only have to implement the
 * following mehtods
 *   * do_step
 *   * do_step_const
 */
class OdeintStepper : public IntegratorStepper
{
	StateType states_bak_;  ///< backup states to be retrieved after an event
	double time_bak_;        ///< backup time to be retrieved after an event
protected:
	/// wrapped version of the DynamicalSystem
	SystemWrapper sys_;
public:
	/// Constructor
	OdeintStepper( int ord, DynamicalSystem* fmu ) : IntegratorStepper( fmu ),
							 sys_( fmu ){}
	/// Make a (possibly adaptive) step and try the step size dt for the first attempt.
	virtual void do_step( EventInfo& eventInfo, StateType& states,
			      fmippTime& currentTime, fmippTime& dt ) = 0;

	virtual void do_step_const( EventInfo& eventInfo, StateType& states,
				    fmippTime& currentTime, fmippTime& dt ){
		/* in case of non adaptive steppers, just use do_step. Otherwise, overwrite this
		   function */
		do_step( eventInfo, states,
			 currentTime, dt );
	}

	void invokeMethod( EventInfo& eventInfo,
			   Integrator::StateType& states,
			   fmippTime time,
			   fmippTime step_size,
			   fmippTime dt,
			   fmippTime eventSearchPrecision )
	{
		fmippTime currentTime = time;
		bool stop = false;
		while ( ( currentTime < time + step_size ) && !stop ){
			// make backup
			time_bak_   = currentTime;
			states_bak_ = states;

			if ( currentTime + dt >= time + step_size ){
				// perform the last step
				dt = time + step_size - currentTime;

				// force stepsize
				do_step_const( eventInfo, states, currentTime, dt );
				reset();

				// exit the while loop next time
				stop = true;
			} else{
				//do_step
				do_step( eventInfo, states, currentTime, dt );
			}
			// update the state and time
			fmu_->setTime( currentTime );
			fmu_->setContinuousStates( &states[0] );

			// check whether a state event occured
			if( fmu_->checkStateEvent() ){
				// set the fmu back to the backup state/time
				states = states_bak_;
				fmu_->setTime( time_bak_ );
				fmu_->setContinuousStates( &states_bak_[0] );

				// tell the integrator about the event and the event location
				eventInfo.stateEvent = true;
				eventInfo.stepEvent  = false;
				eventInfo.tLower = time_bak_;
				eventInfo.tUpper = currentTime;

				return;
			}

			// check for step events
			if ( fmu_->checkStepEvent() ){
				// do not set back anything. Just update the flags
				eventInfo.stepEvent  = true;
				eventInfo.stateEvent = false;

				return;
			}
		}
		// while loop terminated without events.
		eventInfo.stateEvent = false;
		eventInfo.stepEvent  = false;
	}
};

/// Forward Euler method with constant step size.
class Euler : public OdeintStepper
{
	/// Euler stepper.
	euler< StateType > stepper;

public:
	Euler( DynamicalSystem* fmu, Integrator::Properties& properties ) :
		OdeintStepper( 1, fmu )
	{
		properties.name   = "Euler";
		properties.order  = 1;
		properties.abstol = std::numeric_limits< fmippReal >::infinity();
		properties.reltol = std::numeric_limits< fmippReal >::infinity();
	}

	void do_step( EventInfo& eventInfo, StateType& states,
		      fmippTime& currentTime, fmippTime& dt ){
		stepper.do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}
};


/// 4th order Runge-Kutta method with constant step size.
class RungeKutta : public OdeintStepper
{
	/// Runge-Kutta 4 stepper.
	runge_kutta4< StateType > stepper;

public:
	RungeKutta( DynamicalSystem* fmu, Integrator::Properties& properties ) :
		OdeintStepper( 4, fmu )
	{
		properties.name   = "Runge Kutta";
		properties.order  = 4;
		properties.abstol = std::numeric_limits< fmippReal >::infinity();
		properties.reltol = std::numeric_limits< fmippReal >::infinity();
	}

	void do_step( EventInfo& eventInfo, StateType& states,
		      fmippTime& currentTime, fmippTime& dt ){
		stepper.do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}
};


/**
 * 5th order Cash-Karp method with controlled step size.
 *
 * Very similar to the dormand-prince method (same order and same number of rhs evaluations per step), but
 * without dense output capability. Since the current implmentation hardly benefits from dense output, this
 * stepper should yoield almost the same results and same performance as dp.
 */
class CashKarp : public OdeintStepper
{
	typedef runge_kutta_cash_karp54< StateType > error_stepper_type;
	typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
	/// Runge-Kutta-Cash-Karp controlled stepper.
	controlled_stepper_type stepper;
	controlled_step_result res_;

public:
	CashKarp( DynamicalSystem* fmu, Integrator::Properties& properties ) :
		OdeintStepper( 5, fmu )
	{
		// set the "read only" properties
		properties.name  = "Cash Karp";
		properties.order = 5;

		// add missing tolerances if necessary
		if ( properties.abstol != properties.abstol )
			properties.abstol = 1.0e-6;
		if ( properties.reltol != properties.reltol )
			properties.reltol = 1.0e-6;

		// apply tolerances to the stepper
		stepper = make_controlled( properties.abstol, properties.reltol, error_stepper_type() );
	};

	void do_step_const( EventInfo& eventInfo, StateType& states,
			    fmippTime& currentTime, fmippTime& dt ){
		stepper.stepper().do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}

	void do_step( EventInfo& eventInfo, StateType& states,
		      fmippTime& currentTime, fmippTime& dt ){
		do {
			res_ = stepper.try_step( sys_, states, currentTime, dt );
		}
		while ( res_ == fail );
	}
};


/**
 * 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
 *
 * This stepper is the default for ode solving in matlab. It is a simple, yet powerful version of an
 * adaptive runge kutta method. The dense output functionality leads to faster location of state events.
 */
class DormandPrince : public IntegratorStepper
{
	typedef dense_output_runge_kutta< controlled_runge_kutta< runge_kutta_dopri5< StateType > > > dense_stepper;
	/// Runge-Kutta-Dormand-Prince controlled stepper with dense output.
	dense_stepper stepper;
	SystemWrapper sys_;

public:
	DormandPrince( DynamicalSystem* fmu, Integrator::Properties& properties ) :
		IntegratorStepper( fmu ),
		sys_( fmu )
	{
		properties.name  = "Dormand Prince";
		properties.order = 5;

		// add missing tolerances if necessary
		if ( properties.abstol != properties.abstol )
			properties.abstol = 1.0e-6;
		if ( properties.reltol != properties.reltol )
			properties.reltol = 1.0e-6;

		// apply tolerances to the stepper
		stepper = make_dense_output( properties.abstol, properties.reltol,
					     runge_kutta_dopri5< StateType >()
					     );
	};
	void invokeMethod( EventInfo& eventInfo,
			   Integrator::StateType& states,
			   fmippTime time,
			   fmippTime step_size,
			   fmippReal dt,
			   fmippReal eventSearchPrecision ){
		stepper.initialize( states, time, dt );
		while ( true ){
			// perform a step
			stepper.do_step( sys_ );

			// event detection like in OdeintStepper
			fmu_->setTime( stepper.current_time() );
			fmu_->setContinuousStates( &stepper.current_state()[0] );
			if ( fmu_->checkStateEvent() ){
				// set back to the backup state/time
				fmu_->setTime( stepper.previous_time() );
				fmu_->setContinuousStates( &stepper.previous_state()[0] );

				// tell the integrator about the event
				eventInfo.stepEvent  = false;
				eventInfo.stateEvent = true;
				eventInfo.tLower     = stepper.previous_time();
				eventInfo.tUpper     = stepper.current_time();

				return;
			}

			if ( stepper.current_time() >= time + step_size )
				break;
			else if ( fmu_->checkStepEvent() ){
				// tell the integrator about the event
				eventInfo.stepEvent  = true;
				eventInfo.stateEvent = false;

				return;
			}
		}
		// use interoplation to get an approximation for time t.
		stepper.calc_state( time + step_size, states );

		// write the results in the FMU
		fmu_->setTime( time + step_size );
		fmu_->setContinuousStates( &states[0] );

		// check for step events one more time
		if ( fmu_->checkStepEvent() )
			eventInfo.stepEvent = true;

		eventInfo.stateEvent = false;
	}

	void do_step_const( EventInfo& eventInfo,
			    std::vector<fmippReal>& states,
			    fmippTime& time,
			    fmippReal& dt ){
		// use interpolation for do_step_const
		stepper.calc_state( time + dt, states );
		time += dt;
		fmu_->setTime( time );
		fmu_->setContinuousStates( &states[0] );
	}

	void reset(){
		/// \todo Test if this is really OK. Semms like initialize makes reset unnecessary.
	}
};


/**
 * 8th order Runge-Kutta-Fehlberg method with controlled step size.
 *
 * A high order adaptive runge-kutta method. Recommended for smooth problems.
 */
class Fehlberg : public OdeintStepper
{
	typedef runge_kutta_fehlberg78< StateType > error_stepper_type;
	typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
	/// Runge-Kutta-Fehlberg controlled stepper.
	controlled_stepper_type stepper;
	controlled_step_result res_;

public:
	Fehlberg( DynamicalSystem* fmu, Integrator::Properties& properties ) :
		OdeintStepper( 8, fmu )
	{
		properties.name  = "Fehlberg";
		properties.order = 8;

		// add missing tolerances if necessary
		if ( properties.abstol != properties.abstol )
			properties.abstol = 1.0e-6;
		if ( properties.reltol != properties.reltol )
			properties.reltol = 1.0e-6;

		// apply tolerances to the stepper
		stepper = make_controlled( properties.abstol, properties.reltol, error_stepper_type() );
	};

	void do_step_const( EventInfo& eventInfo, StateType& states,
			    fmippTime& currentTime, fmippTime& dt ){
		stepper.stepper().do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}

	void do_step( EventInfo& eventInfo, StateType& states,
		      fmippTime& currentTime, fmippTime& dt ){
		do {
			res_ = stepper.try_step( sys_, states, currentTime, dt );
		}
		while ( res_ == fail );
	}
};


/**
 * Bulirsch-Stoer method with controlled step size.
 *
 * One of the most complex and powerful methods provided by odeint. A highly adaptive method to be
 * used if high precision is required.
 */
class BulirschStoer : public IntegratorStepper
{
	/// Bulirsch-Stoer dense output stepper.
	bulirsch_stoer_dense_out< StateType > stepper;
	SystemWrapper sys_;

public:
	BulirschStoer( DynamicalSystem* fmu, Integrator::Properties& properties ) :
		IntegratorStepper( fmu ),
		stepper(
			// use default tolerances if necessary
			properties.abstol != properties.abstol
			? 1.0e-6 : properties.abstol ,
			properties.reltol != properties.reltol ?
			1.0e-6 : properties.reltol
			),
		sys_( fmu )
	{
		properties.name  = "Bulirsch Stoer";
		properties.order = 0;

		// add missing tolerances if necessary
		if ( properties.abstol != properties.abstol )
			properties.abstol = 1.0e-6;
		if ( properties.reltol != properties.reltol )
			properties.reltol = 1.0e-6;

	};

	void invokeMethod( EventInfo& eventInfo,
			   StateType& states,
			   fmippTime time,
			   fmippTime step_size,
			   fmippReal dt,
			   fmippReal eventSearchPrecision ){
		reset();
		stepper.initialize( states, time, dt );
		while ( true ){
			// perform a step
			stepper.do_step( sys_ );

			// event detection like in OdeintStepper
			fmu_->setTime( stepper.current_time() );
			fmu_->setContinuousStates( &stepper.current_state()[0] );
			if( fmu_->checkStateEvent() ){
				// set back the backup state/time
				fmu_->setTime( stepper.previous_time() );
				fmu_->setContinuousStates( &stepper.previous_state()[0] );

				// tell the integrator about the event
				eventInfo.stateEvent = true;
				eventInfo.stepEvent  = false;
				eventInfo.tLower = stepper.previous_time();
				eventInfo.tUpper = stepper.current_time();

				return;
			}
			if ( stepper.current_time() >= time + step_size )
				break;

			else if ( fmu_->checkStepEvent() ){
				eventInfo.stepEvent  = true;
				eventInfo.stateEvent = false;
				return;
			}
		}
		// use interoplation to get an approximation for time t.
		stepper.calc_state( time + step_size, states );

		// write the results in the FMU
		fmu_->setTime( time + step_size );
		fmu_->setContinuousStates( &states[0] );

		// check for a step event one more time
		if ( fmu_->checkStepEvent() )
			eventInfo.stepEvent = true;

		eventInfo.stateEvent = false;
	}

	void do_step_const( EventInfo& eventInfo,
			    StateType& states,
			    fmippTime& time,
			    fmippReal& dt ){
		// use interpolation for do_step_const
		stepper.calc_state( time + dt, states );
		time += dt;
	}

	void reset(){
		stepper.reset();
	}
};


/**
 * Adams-Bashforth-Moulton multistep method with adjustable order and adaptive step size.
 *
 * Mustistep coallocation methodw with constant step size. To be used if the evaluation of the
 * righthandside is expensive.
 */
class AdamsBashforthMoulton : public OdeintStepper
{
	/// Adams-Bashforth-Moulton stepper, first argument is the order of the method.
	adams_bashforth_moulton< 5, StateType> stepper;
	fmippTime dt_;

public:
	AdamsBashforthMoulton( DynamicalSystem* fmu, Integrator::Properties& properties ) :
		OdeintStepper( 5, fmu ),
		dt_( 0 )
	{
		properties.name   = "ABM";
		properties.order  = 5;
		properties.abstol = std::numeric_limits< fmippReal >::infinity();
		properties.reltol = std::numeric_limits< fmippReal >::infinity();
	};

	void do_step( EventInfo& eventInfo, StateType& states,
		      fmippTime& currentTime, fmippTime& dt ){
		if ( dt != dt_ ){
			reset();
			dt_ = dt;
		}
		stepper.do_step( sys_, states, currentTime, dt );
		currentTime += dt;
	}
	void reset(){
		stepper = adams_bashforth_moulton< 5, StateType>();
	}
};




/**
 * Implicit 4th order Rosenbrock method
 *
 * Suited for stiff systems.
 */
class Rosenbrock : public IntegratorStepper
{
	/// storage type for states
	typedef boost::numeric::ublas::vector< fmippReal > VectorType;
	/// storage type for jacobians
	typedef boost::numeric::ublas::matrix< fmippReal > MatrixType;

	/// Different system wrapper using the ublas vectors as StateType
	struct SystemWrapper_vector{
		DynamicalSystem* ds_;
		SystemWrapper_vector( DynamicalSystem* ds ) : ds_( ds ){}
		/// rhs function
		void operator()( const VectorType& x , VectorType &dx , fmippTime t ) const
		{
			// call the rhs function from the ds_
			ds_->setTime( t );
			ds_->setContinuousStates( &x[0] );
			ds_->getDerivatives( &dx[0] );
		}
	};

	/// Wrapper around the Jacobian function.
	struct jacobi_wrapper{
		DynamicalSystem* ds_;
		jacobi_wrapper( DynamicalSystem* ds ) : ds_( ds ){}
		/// jacobi function
		void operator()( const VectorType &x , MatrixType &jacobi , const fmippTime &t ,
				 VectorType &dfdt ) const
		{
			if ( ds_->providesJacobian() ){
				ds_->setTime( t );
				ds_->setContinuousStates( &x[0] );
				ds_->getJac( &jacobi(0,0) );
				jacobi = boost::numeric::ublas::trans( jacobi );
			}
			else
				ds_->getNumericalJacobian( &jacobi(0,0), &x[0], &dfdt[0], t );
		}
	};

	typedef rosenbrock4_dense_output< rosenbrock4_controller< rosenbrock4< double > > > Stepper;

	SystemWrapper_vector  sys_;
	jacobi_wrapper         jac_;
	int                    neq;
	fmippTime                time_bak_;
	VectorType             statesV_;
	VectorType             states_bak_;
	Stepper                stepper;
	DynamicalSystem*       ds_;
	controlled_step_result res;

	/*
	 * the rosenbrock stepper only works if the types ublas::vector and ublas::matrix is used.
	 * Other types do not provide the necessary arithmetics ( Matrix vector multiplication, etc. )
	 * apparently.
	 */

	/// cast from StateType to VectorType and vice versa
	static void change_type( const StateType &x, VectorType &xV ){
		for ( unsigned int i = 0; i < x.size() ; i++ )
			xV[i] = x[i];
	}
	/// cast from VectorType to StateType
	static void change_type( const VectorType &xV, StateType &x ){
		for ( unsigned int i = 0; i < xV.size() ; i++ )
			x[i] = xV[i];
	}
public:
	Rosenbrock( DynamicalSystem* ds, Integrator::Properties& properties ):
		IntegratorStepper( ds ),
		sys_( ds ),
		jac_( ds ),
		neq( ds->nStates() ),
		statesV_( neq ),
		stepper( make_dense_output( properties.abstol != properties.abstol ?
					    1.0e-6 : properties.abstol,
					    properties.reltol != properties.reltol ?
					    1.0e-6 : properties.reltol,
					    rosenbrock4< double >() )
			 ),
		ds_( ds )
	{
		properties.name  = "Rosenbrock";
		properties.order = 4;

		// add missing tolerances if necessary
		if ( properties.abstol != properties.abstol )
			properties.abstol = 1.0e-6;
		if ( properties.reltol != properties.reltol )
			properties.reltol = 1.0e-6;
	};
	void invokeMethod( EventInfo& eventInfo,
			   StateType& states,
			   fmippTime time,
			   fmippTime step_size,
			   fmippTime dt,
			   fmippTime eventSearchPrecision ){
		reset();
		change_type( states, statesV_ );
		stepper.initialize( statesV_, time, dt );
		while ( true ){
			// perform a step
			stepper.do_step( std::make_pair( sys_, jac_ ) );

			// event detection like in OdeintStepper
			fmu_->setTime( stepper.current_time() );
			fmu_->setContinuousStates( &stepper.current_state()[0] );
			if( fmu_->checkStateEvent() ){
				// ste back the backup state/time
				fmu_->setTime( stepper.previous_time() );
				fmu_->setContinuousStates( &stepper.previous_state()[0] );

				// tell the integrator about the event
				eventInfo.stateEvent = true;
				eventInfo.tLower = stepper.previous_time();
				eventInfo.tUpper = stepper.current_time();

				return;
			}
			if ( stepper.current_time() >= time + step_size )
				break;

			else if ( fmu_->checkStepEvent() ){
				eventInfo.stateEvent = false;
				eventInfo.stepEvent  = true;
				return;
			}
		}
		// use interoplation to get an approximation for time t.
		stepper.calc_state( time + step_size, statesV_ );
		change_type( statesV_, states );

		// write the results in the FMU
		fmu_->setTime( time + step_size );
		fmu_->setContinuousStates( &states[0] );

		// check for a step event one more time
		if ( fmu_->checkStepEvent() )
			eventInfo.stepEvent = true;

		eventInfo.stateEvent = false;
	}

	void do_step_const( EventInfo& eventInfo,
			    std::vector<fmippReal>& states,
			    fmippTime& time,
			    fmippTime& dt ){
		// use interpolation for do_step_const
		stepper.calc_state( time + dt, statesV_ );
		change_type( statesV_, states );
		time += dt;
	}

	void reset(){
		//stepper.reset();
	}
};


#ifdef USE_SUNDIALS
/**
 * Base class for all implementations of sundials steppers
 *
 * \todo use CV_ONE_STEP instead of CV_NORMAL to add more proper step event handling
 */
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
	static int f( fmippTime t, N_Vector x, N_Vector dx, void *user_data )
    {
		DynamicalSystem* fmu = (DynamicalSystem*) user_data;
		fmippStatus status = fmippOK;

		status = fmu->setTime( t );
		if ( fmippOK != status ) return 1;

		status = fmu->setContinuousStates( N_VGetArrayPointer(x) );
		if ( fmippOK != status ) return 1;

		status = fmu->getDerivatives( N_VGetArrayPointer(dx) );
		if ( fmippOK != status ) return 1;

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
	static int g( fmippTime t, N_Vector x, fmippReal *eventsind, void *user_data )
	{
		DynamicalSystem* fmu = (DynamicalSystem*) user_data;
		fmippStatus status = fmippOK;

		status = fmu->setTime( t );
		if ( fmippOK != status ) return 1;

		status = fmu->setContinuousStates( N_VGetArrayPointer( x ) );
		if ( fmippOK != status ) return 1;

		status = fmu->getEventIndicators( eventsind );
		if ( fmippOK != status ) return 1;

		return 0 ;
	}

	/**
	 * Jacobian matrix
	 * like for the rhs, states have to be converted. This function will not be used in case
	 * the fmu is of type 1.0 or the modeldescription does not explicitly say that the Jacobian
	 * is available
	 *
	 * @param[in]      N                    the dimension of the state space ( equal to NEQ_ )
	 * @param[in]      t,x                  time and state
	 * @param[in]      fx                   current derivative
	 * @param[out]     J                    the dense jacobian martix
	 * @param[in]      user_data            the dynamical system
	 * @param[in,out]  tmp1,tmp2,tmp3       variables used internally by CVode
	 *
	 */
	static int Jac( fmippTime t, N_Vector x, N_Vector fx,
		SUNMatrix J, void *user_data, N_Vector tmp1, N_Vector tmp2, N_Vector tmp3 )
	{
		DynamicalSystem* ds = (DynamicalSystem*) user_data;

		// send the input state/time to the FMU
		ds->setTime( t );
		ds->setContinuousStates( N_VGetArrayPointer( x ) );
		// get the jacobian
		fmippStatus status = ds->getJac( SM_CONTENT_D(J)->data );
		// tell SUNDIALs whether the call to getJac was successful
		if ( status == fmippOK )
			return 0;
		else
			return 1;
	}

	const int NEQ_;				///< dimension of state space
	const int NEV_;				///< number of event indicators
	N_Vector states_N_;			///< states in N_Vector format
	fmippTime t_;				///< internal time
	const realtype reltol_;			///< relative tolerance
	const realtype abstol_;			///< absolute tolerance
	void *cvode_mem_;			///< memory of the stepper. This memory later stores
						///< the RHS, states, time and buffer datas for the
						///< multistep methods

	SUNMatrix A_;
	SUNLinearSolver LS_;

public:
	/**
	 * Constructor
	 *
	 * @param[in] fmu	 the fmu to be integrated
	 * @param[in] isBDF	 bool saying wether the bdf or the abm version is required
	 */
	SundialsStepper( DynamicalSystem* fmu, bool isBDF, Integrator::Properties& properties ) :
		IntegratorStepper( fmu ),
		NEQ_( fmu->nStates() ),
		NEV_( fmu->nEventInds() ),
		states_N_( N_VNew_Serial( NEQ_ ) ),
		reltol_( properties.reltol != properties.reltol ? 1e-10 : properties.reltol ),
		abstol_( properties.abstol != properties.abstol ? 1e-10 : properties.abstol ),
		cvode_mem_( 0 )
	{
		// add missing tolerances if necessary
		if ( properties.abstol != properties.abstol )
			properties.abstol = 1.0e-10;
		if ( properties.reltol != properties.reltol )
			properties.reltol = 1.0e-10;

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

		// Initialize solver with dense jacobian.
		A_ = SUNDenseMatrix( NEQ_, NEQ_ );
		LS_ = SUNDenseLinearSolver( states_N_, A_ );
		CVDlsSetLinearSolver( cvode_mem_, LS_, A_ );

		// Set the Jacobian routine to Jac if available. Do not use the numeric jacobian for sundials
		if ( fmu_->providesJacobian() ) CVDlsSetJacFn( cvode_mem_, Jac );

		//CVodeSetErrFile( cvode_mem, NULL ); // uncomment to suppress error messages

		CVodeSetMaxNumSteps( cvode_mem_, static_cast<long>( 1.0e5 ) );
	}

	~SundialsStepper()
	{
		N_VDestroy_Serial( states_N_ );
	}


	void invokeMethod( EventInfo& eventInfo, StateType& states,
			   fmippTime time, fmippTime step_size, fmippTime dt,
			   fmippTime eventSearchPrecision )
	{
		/// \todo add more proper event handling to sundials: currently, time events are
		///       just checked at the begining of each invokeMethod call.

		// write input into internal time
		t_ = time;

		// Convert states into N_Vector format
		for ( int i = 0; i < NEQ_; i++ ) {
			Ith( states_N_ , i ) = states[ i ];
		}

		// reinitialize cvode. this deletes internal memeory
		CVodeReInit( cvode_mem_, t_, states_N_ );     /// \todo reset only if states changed externally

		// set initial step size
		CVodeSetInitStep( cvode_mem_, dt );

		// make iteration
		int flag = CVode( cvode_mem_, t_ + step_size, states_N_, &t_, CV_NORMAL );

		// convert output of cvode in StateType format
		for ( int i = 0; i < NEQ_; i++ ) {
			states[i] = Ith( states_N_, i );
		}

		if ( flag == CV_ROOT_RETURN ){
			eventInfo.stateEvent = true;
			// an event happened -> make sure to return a state before the event.
			StateType dx( NEQ_ );

			/*
			 * rewind the states to make sure the returned state/time is shortly *before* the
			 * event. The rewinding tends to cause bugs if rewind is smaller than the precision
			 * of the sundials solvers. This precision is 100 times the precision of doubles (~1e-14)
			 * according to the official documentation of CVode. However, if the fmu is coded in
			 * floats it might be necessary to adapt the figure rewind
			 *
			 * \todo test with float fmu
			*/
			fmippTime rewind = eventSearchPrecision/10.0;
			if ( rewind <= 1.0e-12 ){
				std::cout << "WARNING: the specified eventsearchprecision might be too small"
					  << " for the use with sundials" << std::endl;
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
			eventInfo.tUpper = t_+2*rewind;
			eventInfo.tLower = t_;

			return;
		}
		else if ( flag == CV_SUCCESS ){
			eventInfo.stateEvent = false;
			// no event happened -> just write the result into the fmu
			fmu_->setTime( t_ );
			fmu_->setContinuousStates( &states.front() );

			if ( fmu_->checkStepEvent() )
				eventInfo.stepEvent = true;
		}
		else{
			std::cout << "an exception happened when running the sundials stepper" << std::endl;
		}
	}
};

/**
 * Backwards differentiation Formula with controlled step size.
 *
 * Suited for stiff problems. Since bdf is a multistep method, the number of righthandside evaluations
 * is much smaller than for other steppers. This might lead to a significant performance gain. In case the
 * jacobian function is missing, SUNDIALS uses an internal algorithm to calculate the numeric jacobian.
 */
class BackwardsDifferentiationFormula : public SundialsStepper
{
public:
	BackwardsDifferentiationFormula( DynamicalSystem* fmu, Integrator::Properties& properties ) :
		SundialsStepper( fmu, true, properties )
	{
		properties.name  = "BDF";
		properties.order = 0;
	};
};

/**
 * Adams bashforth moulton formula with controlled step size and order up to 12
 *
 * A high order multistep method to be used for smooth systems. Since abm2 is a multistep method,
 * the number of righthandside evaluations is much smaller than for other steppers. This might lead
 * to a significant performance gain.
 */
class AdamsBashforthMoulton2 : public SundialsStepper
{
public:
	AdamsBashforthMoulton2( DynamicalSystem* fmu, Integrator::Properties& properties ) :
		SundialsStepper( fmu, false, properties )
	{
		properties.name  = "ABM2";
		properties.order = 0;
	}
};
#endif // USE_SUNDIALS


IntegratorStepper* IntegratorStepper::createStepper( Integrator::Properties& properties, DynamicalSystem* fmu )
{
	IntegratorType type = properties.type;

	// correct ill formated inputs
	if (  properties.abstol == std::numeric_limits<double>::infinity() || properties.abstol < 0 )
		properties.abstol = std::numeric_limits<double>::quiet_NaN();
	if ( properties.reltol == std::numeric_limits<double>::infinity() || properties.reltol < 0 )
		properties.reltol = std::numeric_limits<double>::quiet_NaN();

	switch ( type ) {
	case IntegratorType::eu		: return new Euler                ( fmu, properties );
	case IntegratorType::rk		: return new RungeKutta           ( fmu, properties );
	case IntegratorType::ck		: return new CashKarp             ( fmu, properties );
	case IntegratorType::dp		: return new DormandPrince        ( fmu, properties );
	case IntegratorType::fe		: return new Fehlberg             ( fmu, properties );
	case IntegratorType::bs		: return new BulirschStoer        ( fmu, properties );
	case IntegratorType::abm	: return new AdamsBashforthMoulton( fmu, properties );
	case IntegratorType::ro         : return new Rosenbrock           ( fmu, properties );
#ifdef USE_SUNDIALS
	case IntegratorType::bdf	: return new BackwardsDifferentiationFormula( fmu, properties );
	case IntegratorType::abm2	: return new AdamsBashforthMoulton2         ( fmu, properties );
#endif
	case IntegratorType::NSTEPPERS  : return 0;
	}

	return 0;
}
