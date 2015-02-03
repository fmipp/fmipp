/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_INTEGRATORSTEPPER_H
#define _FMIPP_INTEGRATORSTEPPER_H


#include "import/integrators/include/Integrator.h"

/**
 * \file IntegratorStepper.h 
 *
 * \class IntegratorStepper IntegratorStepper.h 
 * The actual integration methods are implemented by integrator steppers.
 **/


class IntegratorStepper
{

public:
	/// Destructor
	virtual ~IntegratorStepper();

	/// Invokes the integration method. 
	virtual void invokeMethod( Integrator* fmuint, 
				   Integrator::state_type& states,
				   fmiReal time, 
				   fmiReal step_size, 
				   fmiReal dt ) = 0;

	/// Returns the integrator type.
	virtual IntegratorType type() const = 0;

	/// Factory: creates a new integrator stepper.
	static IntegratorStepper* createStepper( IntegratorType type, FMUModelExchangeBase* fmu );
};


#endif // _FMIPP_INTEGRATORSTEPPER_H
