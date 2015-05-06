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
	const int order_;                ///< order of the stepper

protected:
	DynamicalSystem* const fmu_;     ///< pointer to the FMU

	/// Costructor
	IntegratorStepper( int ord, DynamicalSystem* fmu ) : order_( ord ), fmu_( fmu ){};

public:

	/// Destructor
	virtual ~IntegratorStepper();

	virtual void do_step_const( Integrator* fmuint, std::vector<fmiReal>& states,
				    fmiTime& currentTime, fmiTime& dt ){};

	/// Invokes the integration method. 
	virtual void invokeMethod( Integrator* fmuint, 
				   Integrator::state_type& states,
				   fmiReal time, 
				   fmiReal step_size, 
				   fmiReal dt,
				   fmiReal eventSearchPrecision
				   ) = 0;

	/// Returns the integrator type.
	virtual IntegratorType type() const = 0;

	/// Returns the order of the Stepper
	int getOrder(){ return order_; }

	/// Reset the stepper since the states changed externally
	virtual void reset(){};

	/// Factory: creates a new integrator stepper.
	static IntegratorStepper* createStepper( IntegratorType type, DynamicalSystem* fmu );

};


#endif // _FMIPP_INTEGRATORSTEPPER_H
