/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_INTEGRATOR_H
#define _FMIPP_INTEGRATOR_H


#include <vector>

#include "import/integrators/include/IntegratorType.h"


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

	/// \typedef std::vector<fmiReal> state_type 
	typedef std::vector<fmiReal> state_type;  

	/**
	 * Constructor.
	 *
	 * @param[in]  fmu  an FMU ME to be integrated 
	 * @param[in]  type  integerator method
	 */
	Integrator( FMUModelExchangeBase* fmu, IntegratorType type = IntegratorType::dp );

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

	// RHS for CVODE
	void rhs(const state_type& x, state_type& dx, fmiReal time );

	/// GetIntEvent for CVODE
	bool getIntEvent(fmiReal time, state_type state);

	/// ODEINT solvers call observer function with two parameters after each succesfull step
	void operator()( const state_type& state, fmiReal time );

	//// Clone this instance of Integrator (not a copy).
	Integrator* clone() const;

private:

	FMUModelExchangeBase* fmu_; 	///< Pointer to FMU ME.
	IntegratorStepper* stepper_;    ///< The stepper implements the actual integration method.
	state_type states_;		///< Internal states. Serve as backup if an intEvent occurs.
	fmiReal time_;			///< Internal time. Serves as backup if an intEvent occurs.

	bool is_copy_;                  ///< Is this just a copy of another instance of Integrator? -> See destructor.

};


#endif // _FMIPP_INTEGRATOR_H
