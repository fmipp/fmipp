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
	 * this operation might use interpolation formulas and should therefore *not* be used in
	 * a fashion like the following
	 *
	 *     double t = 0.0, tstop = 100.0;
	 *     int nSteps = 10000;
	 *     while ( t < tstop )
	 *          integrator.do_step_const( ..., t, 0.001 );
	 *
	 */
	virtual void do_step_const( Integrator::EventInfo& eventInfo, std::vector<fmiReal>& states,
				    fmiTime& currentTime, fmiTime& dt ){};

	/// Invokes the integration method. 
	virtual void invokeMethod( Integrator::EventInfo& eventInfo,
				   Integrator::state_type& states,
				   fmiReal time, 
				   fmiReal step_size, 
				   fmiReal dt,
				   fmiReal eventSearchPrecision
				   ) = 0;

	/// Reset the stepper since the states changed externally
	virtual void reset(){};

	/// Factory: creates a new integrator stepper.
	static IntegratorStepper* createStepper( Integrator::Properties& properties, DynamicalSystem* fmu );
};


#endif // _FMIPP_INTEGRATORSTEPPER_H
