/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/




#ifndef _FMIPP_FMUINTEGRATOR_H
#define _FMIPP_FMUINTEGRATOR_H


#include <vector>

#include "FMUBase.h"

class FMUIntegratorStepper;


/**
 * \file FMUIntegrator.h 
 * \class FMUIntegrator FMUIntegrator.h 
 * Brief descripition. 
 * 
 * Detailed Description.
 * 
 * \todo a new class mytypes.h for common types? 
 */ 
class FMUIntegrator
{

public:

	/**
	 * \enum IntegratorType available integration methods. 
         **/
        enum IntegratorType { eu, /// Forward Euler method. 
			      rk, /// 4th order Runge-Kutta method with constant step size.
			      dp, /// 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
			      fe, /// 7th order Runge-Kutta-Fehlberg method with controlled step size. 
			      bs, /// Bulirsch-Stoer method with controlled step size.
			      abm /** abm: Adams-Bashforth-Moulton multistep method with adjustable order and adaptive
			              step size. FIXME: Doesn't work properly, something with the step size? */
	};

	/**
	 * \typedef std::vector<fmiReal> state_type 
	 */
	typedef std::vector<fmiReal> state_type;  


	FMUIntegrator( FMUBase* fmu, IntegratorType type = dp );  ///< Constructor.
	FMUIntegrator( const FMUIntegrator& ); ///< Copy constructor.
	~FMUIntegrator(); ///< Destructor.

	IntegratorType type() const; ///< Return the integration algorithm type (i.e. the stepper type). 

	void integrate( fmiReal step_size, fmiReal dt ); ///< Integrate FMU state.

	void operator()( const state_type& x, state_type& dx, fmiReal time ); ///< Evaluates the right hand side of the ODE
	void operator()( const state_type& state, fmiReal time ); ///< ODEINT solvers call observer function with two parameters after each succesfull step

	/** Clone this instance of FMUIntegrator (not a copy). **/
	FMUIntegrator* clone() const;

private:


	FMUBase* fmu_; 	///< Pointer to FMU.
	FMUIntegratorStepper* stepper_; ///< The stepper implements the actual integration method.
	bool is_copy_; ///< Is this just a copy of another instance of FMUIntegrator? -> See destructor.

};


#endif // _FMIPP_FMUINTEGRATOR_H
