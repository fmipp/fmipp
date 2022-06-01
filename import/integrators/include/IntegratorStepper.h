// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_INTEGRATORSTEPPER_H
#define _FMIPP_INTEGRATORSTEPPER_H


#include "import/integrators/include/Integrator.h"

/**
 * \file IntegratorStepper.h 
 * The actual integration methods are implemented by integrator steppers.
 *
 * \class IntegratorStepper IntegratorStepper.h 
 * The actual integration methods are implemented by integrator steppers.
 *
 * The following steppers are available.
 *
 * | Stepper | Name                             | Suite    | Order | Adaptive | Recommended usecases           |
 * | ------- | -------------------------------- | -------- | ----- | -------- | ------------------------------ |
 * | eu      | Euler                            | ODEINT   | 1     | No       | Testing                        |
 * | rk      | RungeKutta                       | ODEINT   | 4     | No       | Testing                        |
 * | abm     | AdamsBashforthMoulton            | ODEINT   | 8     | No       | Testing                        |
 * | ck      | CashKarp                         | ODEINT   | 5     | Yes      | Nonstiff Models                |
 * | dp      | DormandPrince                    | ODEINT   | 5     | Yes      | Nonstiff Models                |
 * | fe      | Fehlberg                         | ODEINT   | 8     | Yes      | Nonstiff, smooth Models        |
 * | bs      | BulirschStoer                    | ODEINT   | 1-16  | Yes      | High precision required        |
 * | ro      | Rosenbrock                       | ODEINT   | 4     | Yes      | Stiff Models                   |
 * | bdf     | BackwardsDifferentiationFormula  | SUNDIALS | 1-5   | Yes      | Stiff Models                   |
 * | abm2    | AdamsBashforthMoulton2           | SUNDIALS | 1-12  | Yes      | Nonstiff Models, expensive rhs |
 *
 **/

/// \copydoc Integrator::StateType
typedef Integrator::StateType StateType;
/// \copydoc Integrator::EventInfo
typedef Integrator::EventInfo EventInfo;

class IntegratorStepper
{

protected:
	DynamicalSystem* const fmu_;     ///< pointer to the FMU

	/// Costructor
	IntegratorStepper( DynamicalSystem* fmu ) : fmu_( fmu ){};

public:

	/// Destructor
	virtual ~IntegratorStepper();

	/**
	 * Make a step with a specified step size and do not allow the stepper to adapt the step size
	 *
	 * \warning this operation might use interpolation formulas and should therefore **not** be used in
	 *          a fashion like the following
	 *          \code{.cpp}
	 *                 double t = 0.0, tstop = 100.0;
	 *                 int nSteps = 10000;
	 *                 while ( t < tstop )
	 *                       integrator.do_step_const( ..., t, 0.001 );
	 *          \endcode
	 */
	virtual void do_step_const( EventInfo& eventInfo, StateType& states,
		fmippTime& currentTime, fmippTime& dt ){};

	/**
	 * Invokes the integration method.
	 *
	 * \param[out]    eventInfo             informs the caller about events
	 * \param[in,out] states                continuous stats at the beginning/end of the integration
	 * \param[in]     time                  time at the beginnign of the integration
	 * \param[in]     step_size             time span to be integrated
	 * \param[in]     dt                    initial step size for integration
	 * \param[in]     eventSearchPrecision  accuracy for the location of state events
	 *
	 * \note   In case of an event, the invokemethod call stops the integration before reaching
	 *         the final time `time + step_size`
	 */
	virtual void invokeMethod( Integrator::EventInfo& eventInfo,
		Integrator::StateType& states,
		fmippTime time, 
		fmippTime step_size, 
		fmippTime dt,
		fmippTime eventSearchPrecision
		) = 0;

	/**
	 * Reset the stepper since the states changed externally
	 *
	 * Currently the steppers are reset before every call to invokeMethod. Yet, it would
	 * be adviseable to make less reset calls for the sake of performance.
	 */
	virtual void reset(){};

	/**
	 * Factory: creates a new integrator stepper.
	 *
	 * \param[in]  properties   The properties to be applied
	 * \param[in]  fmu          The fmu to be integrated
	 * \param[out] properties   The properties that could be applied. Not necessarily the same as
	 *                          the input
	 * \returns A pointer to the created stepper.
	 *
	 * \note in general, not all properties will be applied. For example, if you use a non adaptive
	 *       stepper, the tolerances will be ignored and set to inf by constructor of the stepper
	 *       make sure to check the returned properties object to see whether all properties could
	 *       be changed.
	 */
	static IntegratorStepper* createStepper( Integrator::Properties& properties, DynamicalSystem* fmu );
};


#endif // _FMIPP_INTEGRATORSTEPPER_H
