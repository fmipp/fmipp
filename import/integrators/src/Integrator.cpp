/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file Integrator.cpp
 * The Integrator serves as an interface between the IntegratorSteppers and FMUModelExchange
 */ 

#include <cstdio>
#include <cassert>
#include <limits>

#include "common/fmi_v1.0/fmiModelTypes.h"

#include "import/base/include/FMUModelExchangeBase.h"

#include "import/integrators/include/Integrator.h"
#include "import/integrators/include/IntegratorStepper.h"

#include <iostream>

using namespace std;


Integrator::Integrator( FMUModelExchangeBase* fmu, IntegratorType type ) :
	fmu_( fmu ),
	stepper_( IntegratorStepper::createStepper( type, fmu ) ),
	states_( fmu_->nStates(), std::numeric_limits<fmiReal>::quiet_NaN() ),
	time_( std::numeric_limits<fmiReal>::quiet_NaN() ),
	is_copy_( false ),
	eventsind_( new fmiReal (fmu_->nEventInds())  )
{
	assert( 0 != stepper_ );
}


Integrator::Integrator( const Integrator& other ) :
	fmu_( other.fmu_ ),
	stepper_( other.stepper_ ),
	states_( other.states_ ),
	time_( other.time_ ),
	is_copy_( true ),
	eventsind_( other.eventsind_ )
{}


Integrator::~Integrator()
{
	// Internally, ODEINT makes copies of this class (when using
	// boost::fusion::for_each). This copies can however all use
	// the same stepper. Only when the destructor of the original
	// instance is called, the stepper is deleted.
	if ( false == is_copy_ ) delete stepper_;
}


IntegratorType Integrator::type() const
{
	return stepper_->type();
}



bool Integrator::integrate( fmiReal step_size, fmiReal dt, fmiReal eventSearchPrecision )
{
	// update the internal Event Indicators
	fmu_->getEventIndicators( eventsind_ );

	// Get current time.
	time_ = fmu_->getTime();

	// Get current continuous states.
	fmu_->getContinuousStates( &states_.front() );

	// Invoke integration method.
	stepper_->invokeMethod( this, states_, fmu_->getTime(), step_size, dt, eventSearchPrecision );

		// if no event happened, return
	if ( !eventHappened_ ){
		return false;
	} // else, use a binary search to locate the event upt to the eventSearchPrecision_
	else{
		/* an event happend. locate it using an event-search loop ( binary search )
		 *    * tLower_     last time where the stepper did not detect an event
		 *                  this variable gets written by invokeMethod
		 *    * tUpper_     first time where the stepper detected an event
		 *                  this variable gets written by invokeMethod
		 */
		while ( tUpper_ - tLower_ > eventSearchPrecision/2.0 ){
			// let the stepper integrate the left hgalf of the Interval [tLower_,tUpper_]
		  stepper_->invokeMethod( this, states_, tLower_, ( tUpper_ - tLower_ )/2.0,
					  dt, eventSearchPrecision );

			if ( ! eventHappened_ ){
				// set new horizon ( tUpper_ and tLower_ ) manually. otherwise this is done
				// internally by invokeMethod
				tLower_ = ( tUpper_ + tLower_ )/2.0;
				tUpper_ = tUpper_;
			} else{}
				// reset the stepper ( only relevant for steppers with internal states )
				//stepper_->reset();
		}
		// make sure the event is *strictly* inside the interval [tLower_, tUpper_]
		tUpper_+= eventSearchPrecision/8.0;
		return true;
	}

}


Integrator* Integrator::clone() const
{
	return new Integrator( this->fmu_, this->type() );
}

int Integrator::stepperOrder()
{
	return stepper_->getOrder();
}

// get time horizon for the event
void Integrator::getEventHorizon( time_type& tLower, time_type& tUpper ){
	tLower = tLower_;
	tUpper = tUpper_;
}


bool Integrator::checkStateEvent(){
	double* newEventsind = new double( fmu_->nEventInds() );
	fmu_->getEventIndicators( newEventsind );
	for ( unsigned int i = 0; i < fmu_->nEventInds(); i++ )
		if ( newEventsind[i] * eventsind_[i] < 0 )
			{
				delete newEventsind;
				return true;
			}
	delete newEventsind;
	return false;
}
