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

	/// Reset the stepper since the states changed externally
	virtual void reset(){};

	/// Factory: creates a new integrator stepper.
	static IntegratorStepper* createStepper( Integrator::Properties& properties, DynamicalSystem* fmu );
};


#endif // _FMIPP_INTEGRATORSTEPPER_H
