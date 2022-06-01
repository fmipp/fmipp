// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_INTEGRATOR_H
#define _FMIPP_INTEGRATOR_H

#include <limits>
#include <vector>

#include "common/FMIPPConfig.h"

#include "import/integrators/include/IntegratorType.h"

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

	/// StateType for odeint
	typedef std::vector<fmippReal> StateType;

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

	/**
	 * Allocate memory for the integrator.
	 *
	 * Call this after numberOfContinuousStates has been set
	 */
	void initialize();

	/// Change the stepper type
	void setType( IntegratorType type );

	/// give informations about some properties of a stepper
	struct __FMI_DLL Properties{
		IntegratorType type;     ///< the stepper type that is currently used
		std::string    name;     ///< basically the same as name but in more detail
		int            order;    ///< global trunounciation error of the stepper
		double         abstol;   ///< absolute tolerance. Inf for non adaptive steppers
		double         reltol;   ///< relative tolerance. Inf for non adaptive steppers
		Properties() : type( IntegratorType::dp ),
			name( "" ),
			order( 0 ),
			abstol( std::numeric_limits<double>::quiet_NaN() ),
			reltol( std::numeric_limits<double>::quiet_NaN() ){}

		/// Returns true iff all properties are equal
		bool operator==( const Properties& prop ) const;
		/// Returns true iff at least one property differs
		bool operator!=( const Properties& prop ) const 
		{ 
			return !operator==(prop); 
		}
	};

	/// Information about events.
	struct EventInfo{
		bool   stepEvent;      ///< Did a step Event happen?
		bool   stateEvent;     ///< Did an state event happen?
		double tLower;         ///< Lower limit for the state Event time.
		                       ///  Possibly undefined of stateEvent == false
		double tUpper;         ///< Upper limit for the state Event time.
		                       ///  Possibly undefined of stateEvent == false
	EventInfo() : stepEvent( 0 ), stateEvent( 0 ), tLower( 0 ), tUpper( 0 ){}
	};

	/// Integrate FMU ME state.
	EventInfo integrate( fmippTime step_size, fmippTime dt, fmippTime eventSearchPrecision );

	/// Clone this instance of Integrator (not a copy).
	Integrator* clone() const;

	/// return upper and lower limits for state events
	void getEventHorizon( fmippTime& tLower, fmippTime& tUpper );

	/**
	 * create a new stepper with the specified properties
	 *
	 * The factory method is called and tries to chreate a stepper with very similar properties
	 * as the specified ones.
	 * In the curerent implementation arguments that ate guaraneed to be applied are
	 *   * type
	 *
	 * and arguments that are guaranteed to be ignored are
	 *   * name  ( depends on type )
	 *   * order ( depends on type )
	 *
	 * in case of non adaptive steppers, reltol and abstol are set to infinity
	 *
	 * \retval properties    the properties of the stepper that has been built.
	 */
	void setProperties( Properties& properties );

	/// get the properties of the currently used stepper
	Properties getProperties() const;

private:

	Properties properties_;         ///< Internal copy of the stepper properties
	EventInfo  eventInfo_;          ///< last event info returned by the stepper
	                                ///  gets updated inside the eventSearch loop

	DynamicalSystem* fmu_;    	///< Pointer to FMU ME.
	IntegratorStepper* stepper_;    ///< The stepper implements the actual integration method.
	StateType states_;		///< Internal states. Serve as backup if an intEvent occurs.
	fmippTime time_;			///< Internal time. Serves as backup if an intEvent occurs.

	bool is_copy_;                  ///< Is this just a copy of another instance of Integrator? -> See destructor.
};

#endif // _FMIPP_INTEGRATOR_H
