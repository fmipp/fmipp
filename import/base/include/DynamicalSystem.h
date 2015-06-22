#ifndef _DYNAMICAL_SYSTEM_H
#define _DYNAMICAL_SYSTEM_H

typedef double real_type;
typedef real_type time_type;

//#include "import/integrators/include/IntegratorProperties.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "import/integrators/include/Integrator.h"


/**
 * \file DynamicalSystem.h
 *
 * \class DynamicalSystem DynamicalSystem.h
 * Reduces FMUs for ME to the functionalities necessary for integration. It also defines which parts
 * of an fmu are actually exposed to the Integrator and IntegratorStepper class
 *
 * these functionalities are
 *   * get/set States
 *   * get/set Time
 *   * get     EventIndicators
 *   * get     Derivatives (  righthandside of the ODE )
 *   * get     Jacobian
 *
 * In case an FMU does not supply a Jacobian, the jacobian function is calculated using a finite
 * difference method.
 *
 * MAINTAINANCE NOTE: make sure to keep this class slim, since the evaluation speed of this functions
 *                    has a tremendous effect on the overall performance of the fmippim module
 */

class __FMI_DLL DynamicalSystem
{
public:

	/// constructor
	DynamicalSystem();

	/// Destructor.
	virtual ~DynamicalSystem();

	/// set the FMU time
	virtual void setTime( time_type time ) = 0;

	/// get the FMU time
	virtual real_type getTime() const = 0;  //\TODO: change the input of Integrator from
	//uncomment

	/// get continuous states
	virtual fmiStatus getContinuousStates( real_type* x ) = 0;

	/// set continuous states
	virtual fmiStatus setContinuousStates( const real_type* x ) = 0;

	/// get the derivatives / righthandside of the ODE
	virtual fmiStatus getDerivatives( real_type* dx ) = 0;

	/// get EventIndicators at curret FMU state/time
	virtual fmiStatus getEventIndicators( real_type* eventsind ) = 0;

	/// return the number of continuous states
	virtual std::size_t nStates() const = 0;

	/// return the number of event indicators
	virtual std::size_t nEventInds() const = 0;

	/// say wheteher the FMU provides a jacobian ( always false for 1.0 FMUs )
	virtual bool providesJacobian(){ return providesJacobian_; };

	/// get Jacobian for the current FMU state/time. J[i][j] is the derivative of the i-th component
	/// of the righthandside with respect to the j-th continuous state.
	virtual fmiStatus getJac( real_type* J );

	/** calculate the Jacobian and store the result as c-array (double*) of length NEQ*NEQ
	 *
	 * For example, J = ( J00 J01 )   gets returned as ( J00, J01, J10, J11 )
	 *                  ( J10 J11 )
	 *
	 * the method used is of 8th order and uses 8*NEQ rhs evaluations.
	 *
	 * for comparison - the forward differences method (1st order) uses NEQ+1 rhs evaluations.
	 */
	virtual void getNumericalJacobian( real_type* J, const real_type* x, real_type* dfdt, const real_type t );

	// check whether the sign of at least one event indicator changed since the last call
	// to saveEventIndicators()
	bool checkStateEvent();

	/// call completedIntegratorStep and check for a step event
	virtual bool checkStepEvent() = 0;

	/// Get a struct containig the name and the tolerances of the stepper.
	Integrator::Properties getIntegratorProperties(){
		return integrator_->getProperties();
	}

protected:
	/// Integrator Instance
	Integrator* integrator_;

	/// Flag indicating whether the jacobian can be computed by the fmu
	bool providesJacobian_;

	// save current event indicators for later calls to checkStateEvent()
	void saveEventIndicators();

private:
	/// Avoid naming conflict with FMUModelExchange::eventsind_
	double* savedEventIndicators_;
};

#endif
