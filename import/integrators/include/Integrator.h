/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_INTEGRATOR_H
#define _FMIPP_INTEGRATOR_H


#include <vector>

class FMUModelExchangeBase;
class IntegratorStepper;


/**
 * \file Integrator.h 
 *
 * \class Integrator Integrator.h 
 * Integrator for classes implementing FMUModelExchangeBase.
 * 
 * This integrator is compatible with all classes that properly implement FMUModelExchangeBase.
 * It provides a wrapper for Boost's ODEINT library.
 */ 


class Integrator
{

public:


	/// \enum IntegratorType available integration methods. 
        enum IntegratorType { eu, /// Forward Euler method. 
			      rk, /// 4th order Runge-Kutta method with constant step size.
			      dp, /// 5th order Runge-Kutta-Dormand-Prince method with controlled step size.
			      fe, /// 7th order Runge-Kutta-Fehlberg method with controlled step size. 
			      bs, /// Bulirsch-Stoer method with controlled step size.
			      abm /// abm: Adams-Bashforth-Moulton multistep method with adjustable order and adaptive step size.
	};

	/// \typedef std::vector<fmiReal> state_type 
	typedef std::vector<fmiReal> state_type;  

	/**
	 * Constructor.
	 *
	 * @param[in]  fmu  an FMU ME to be integrated 
	 * @param[in]  type  integerator method
	 */
	Integrator( FMUModelExchangeBase* fmu, IntegratorType type = dp );

	/// Copy constructor.
	Integrator( const Integrator& );

	/// Destructor.
	~Integrator();

	/// Return the integration algorithm type (i.e. the stepper type). 
	IntegratorType type() const;

	/// Integrate FMU ME state.
	void integrate( fmiReal step_size, fmiReal dt );

	/// Evaluates the right hand side of the ODE.
	void operator()( const state_type& x, state_type& dx, fmiReal time );

	/// ODEINT solvers call observer function with two parameters after each succesfull step
	void operator()( const state_type& state, fmiReal time );

	//// Clone this instance of Integrator (not a copy).
	Integrator* clone() const;

private:

	FMUModelExchangeBase* fmu_; 	///< Pointer to FMU ME.
	IntegratorStepper* stepper_; ///< The stepper implements the actual integration method.
	state_type states_;
	fmiReal time_;

	bool is_copy_; ///< Is this just a copy of another instance of Integrator? -> See destructor.

};


#endif // _FMIPP_INTEGRATOR_H
