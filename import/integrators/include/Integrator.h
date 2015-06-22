/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_INTEGRATOR_H
#define _FMIPP_INTEGRATOR_H


#include <vector>

#include "common/FMIPPConfig.h"

#include "import/integrators/include/IntegratorType.h"

#include <string>
#include <limits>


class DynamicalSystem;
class IntegratorStepper;


/**
 * \file Integrator.h 
 * This integrator is compatible with all classes that properly implement FMUModelExchangeBase.
 * It provides a wrapper for Boost's ODEINT library and the CVode module from Sundials
 *
 * \class Integrator Integrator.h 
 * Integrator for classes implementing FMUModelExchangeBase.
 * 
 * This integrator is compatible with all classes that properly implement FMUModelExchangeBase.
 * It provides a wrapper for Boost's ODEINT library and the CVode module from Sundials
 */ 


class __FMI_DLL Integrator
{

public:

	/// \typedef std::vector<fmiReal> state_type 
	typedef std::vector<fmiReal> state_type;
	typedef double time_type;

	/**
	 * Constructor.
	 *
	 * @param[in]  fmu  an FMU ME to be integrated
	 */
	Integrator( DynamicalSystem* fmu );

	/// Copy constructor.
	Integrator( const Integrator& );

	/// Destructor.
	~Integrator();

	void initialize();

	void setType( IntegratorType type );

	struct Properties{
		IntegratorType type;
		std::string    name;
		int            order;
		double         abstol;
		double         reltol;
		Properties() : type( IntegratorType::dp ),
			name( "" ),
			order( 0 ),
			abstol( std::numeric_limits<double>::quiet_NaN() ),
			reltol( std::numeric_limits<double>::quiet_NaN() ){}
	};

	// information about events.
	struct EventInfo{
		bool   stepEvent;
		bool   stateEvent;
		double tLower;
		double tUpper;
	EventInfo() : stepEvent( 0 ), stateEvent( 0 ), tLower( 0 ), tUpper( 0 ){}
	};

	/// Integrate FMU ME state.
	bool integrate( fmiReal step_size, fmiReal dt, fmiReal eventSearchPrecision );

	/// Clone this instance of Integrator (not a copy).
	Integrator* clone() const;

	/// return upper and lower limits for state events
	void getEventHorizon( time_type& tLower, time_type& tUpper );

	/// create a new stepper with the specified properties
	void setProperties( Properties& properties );

	/// get the properties of the currently used stepper
	Properties getProperties() const;

private:

	Properties properties_;
	EventInfo  eventInfo_;

	DynamicalSystem* fmu_;    	///< Pointer to FMU ME.
	IntegratorStepper* stepper_;    ///< The stepper implements the actual integration method.
	state_type states_;		///< Internal states. Serve as backup if an intEvent occurs.
	fmiReal time_;			///< Internal time. Serves as backup if an intEvent occurs.

	bool is_copy_;                  ///< Is this just a copy of another instance of Integrator? -> See destructor.
};

#endif // _FMIPP_INTEGRATOR_H
