/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file IntegratorStepper.cpp
 * The integrator steppers that actually wrap the methods provided by Boost's ODEINT library are implemented here.
 */ 

#include <iostream>
#include <fstream>
#include <cstdio>
#include <boost/numeric/odeint.hpp>
#include <iomanip>

#include "common/fmi_v1.0/fmiModelTypes.h"

#include "import/integrators/include/IntegratorStepper.h"


using namespace boost::numeric::odeint;

typedef Integrator::state_type state_type;


IntegratorStepper::~IntegratorStepper() {}


/// Forward Euler method with constant step size.
class Euler : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
{
		// Runge-Kutta 4 stepper.
		static euler< state_type > stepper; // Static: initialize only once.

		// Integrator function with constant step size.
		integrate_const( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::eu; }
};


/// 4th order Runge-Kutta method with constant step size.
class RungeKutta : public IntegratorStepper
{

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Runge-Kutta 4 stepper.
		static runge_kutta4< state_type > stepper; // Static: initialize only once.

		// Integrator function with constant step size.
		integrate_const( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::rk; }

};


/// 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
class CashKarp : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Runge-Kutta-Dormand-Prince controlled stepper.
		typedef runge_kutta_cash_karp54< state_type > error_stepper_type;
		typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::ck; }

};


/// 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
class DormandPrince : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Runge-Kutta-Dormand-Prince controlled stepper.
		typedef runge_kutta_dopri5< state_type > error_stepper_type;
		typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::dp; }

};


/// 7th order Runge-Kutta-Fehlberg method with controlled step size.
class Fehlberg : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Runge-Kutta-Fehlberg controlled stepper.
		typedef runge_kutta_fehlberg78< state_type > error_stepper_type;
		typedef controlled_runge_kutta< error_stepper_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::fe; }
};


/// Bulirsch-Stoer method with controlled step size.
class BulirschStoer : public IntegratorStepper
{
public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Bulirsch-Stoer controlled stepper.
		typedef bulirsch_stoer< state_type > controlled_stepper_type;
		static controlled_stepper_type stepper; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( stepper, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::bs; }
};




/// Adams-Bashforth-Moulton multistep method with adjustable order and adaptive step size.
class AdamsBashforthMoulton : public IntegratorStepper
{

/// \FIXME Doesn't work properly, something with the step size?

public:

	void invokeMethod( Integrator* fmuint, state_type& states,
			   fmiReal time, fmiReal step_size, fmiReal dt )
	{
		// Adams-Bashforth-Moulton stepper, first argument is the order of the method.
		adams_bashforth_moulton< 5, state_type > abm; // Static: initialize only once.

		// Integrator function with adaptive step size.
		integrate_adaptive( abm, *fmuint, states, time, time+step_size, dt, *fmuint );
	}

	virtual IntegratorType type() const { return IntegratorType::abm; }
};



IntegratorStepper* IntegratorStepper::createStepper( IntegratorType type )
{
	switch ( type ) {
	case IntegratorType::eu: return new Euler;
	case IntegratorType::rk: return new RungeKutta;
	case IntegratorType::ck: return new CashKarp;
	case IntegratorType::dp: return new DormandPrince;
	case IntegratorType::fe: return new Fehlberg;
	case IntegratorType::bs: return new BulirschStoer;
	case IntegratorType::abm: return new AdamsBashforthMoulton;
	}

	return 0;
}
