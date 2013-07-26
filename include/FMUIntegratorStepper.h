/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMUINTEGRATORSTEPPER_H
#define _FMIPP_FMUINTEGRATORSTEPPER_H


#include "FMUIntegrator.h"

/**
 * \file FMUIntegratorStepper.h 
 * \class FMUIntegratorStepper FMUIntegratorStepper.h 
 * The actual integration methods are implemented by integrator steppers.
 **/


class FMUIntegratorStepper
{

public:

	virtual ~FMUIntegratorStepper();

	/**
	 * \typedef FMUIntegrator::IntegratorType IntegratorType 
	 */ 
	typedef FMUIntegrator::IntegratorType IntegratorType;

	virtual void invokeMethod( FMUIntegrator* fmuint, 
				   FMUIntegrator::state_type& states,
				   fmiReal time, 
				   fmiReal step_size, 
				   fmiReal dt ) = 0; ///<  Invokes integration method. 

	virtual IntegratorType type() const = 0; ///< Returns the integrator type

	static FMUIntegratorStepper* createStepper( IntegratorType type ); ///< Factory: creates a new integrator stepper.
};


#endif // _FMIPP_FMUINTEGRATORSTEPPER_H
