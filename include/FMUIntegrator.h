#ifndef _FMIPP_FMUINTEGRATOR_H
#define _FMIPP_FMUINTEGRATOR_H

#include <vector>

#include "FMU.h"

class StepperBase;


class FMUIntegrator
{

public:

	typedef std::vector<fmiReal> state_type;

	// Enum IntegratorType defines the integration method:
	//  - rk: 4th order Runge-Kutta method with constant step size.
	//  - dp: 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
	//  - fe: 7th order Runge-Kutta-Fehlberg method with controlled step size.
	//  - bs: Bulirsch-Stoer method with controlled step size.
	//  - abm: Adams-Bashforth-Moulton multistep method with adjustable order and adaptive
	//         step size. FIXME: Doesn't work properly, something with the step size?
	enum IntegratorType { rk, dp, fe, bs, abm };

	// Constructor.
	FMUIntegrator( FMU* fmu, IntegratorType type = dp );

	// Destructor.
	~FMUIntegrator();

	// Evaluates the right hand side of the ODE.
	void operator()( const state_type& x, state_type& dx, fmiReal time );

	// ODEINT solvers call observer function with two parameters after each succesfull step.
	void operator()( const state_type& state, fmiReal time );

	void integrate( fmiReal step_size, size_t n_steps );

	const IntegratorType& type() const { return type_; }

private:

	// Pointer to FMU.
	FMU* fmu_;

	// What method does this integrator use?
	const IntegratorType type_;

	// Number of state variables.
	size_t nstates_;

	// This vector holds (temporarily) the values of the FMU's continuous states.
	state_type states_;

	// The stepper implements the actual integration method.
	StepperBase* stepper_;

};


#endif // _FMIPP_FMUINTEGRATOR_H
