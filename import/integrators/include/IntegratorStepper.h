/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

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
 **/


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
	 * this operation might use interpolation formulas and should therefore **not** be used in
	 * a fashion like the following
	 *
	 * \code{.cpp}
	 *     double t = 0.0, tstop = 100.0;
	 *     int nSteps = 10000;
	 *     while ( t < tstop )
	 *          integrator.do_step_const( ..., t, 0.001 );
	 * \endcode
	 */
	virtual void do_step_const( Integrator::EventInfo& eventInfo, std::vector<fmiReal>& states,
				    fmiTime& currentTime, fmiTime& dt ){};

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
	 */
	virtual void invokeMethod( Integrator::EventInfo& eventInfo,
				   Integrator::state_type& states,
				   fmiReal time, 
				   fmiReal step_size, 
				   fmiReal dt,
				   fmiReal eventSearchPrecision
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
	 */
	static IntegratorStepper* createStepper( Integrator::Properties& properties, DynamicalSystem* fmu );
};


#endif // _FMIPP_INTEGRATORSTEPPER_H
