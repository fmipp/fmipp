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


Integrator::Integrator( DynamicalSystem* fmu ) :
	fmu_( fmu ),
	stepper_( 0 ),
	is_copy_( false ),
	eventsind_( 0 )
{}


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
	if ( false == is_copy_ || stepper_ )
		delete stepper_;
	if ( 0 != eventsind_ )
		delete eventsind_;
}


Integrator* Integrator::clone() const
{
	Integrator* integrator = new Integrator( this->fmu_ );
	integrator->initialize();
	integrator->setType( this->type() );
	return integrator;
}


void Integrator::initialize(){
	states_      = state_type( fmu_->nStates(), std::numeric_limits<fmiReal>::quiet_NaN() );
	time_        = std::numeric_limits<fmiReal>::quiet_NaN();
	eventsind_   = new fmiReal [ fmu_->nEventInds() ];
}


void Integrator::setType( IntegratorType type )
{
	if ( 0 != stepper_ )
		delete stepper_;
	stepper_ = IntegratorStepper::createStepper( type, fmu_ );
	properties_.type  = type;
	properties_.order = stepper_->getOrder();
}


void Integrator::setProperties( Integrator::Properties& properties )
{
	// \TODO: run the factory method here to get a stepper according to the properties
	properties_ = properties;
}

Integrator::Properties Integrator::getProperties()
{
	return properties_;
}


IntegratorType Integrator::type() const
{
	return properties_.type;
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
		if ( tUpper_ > time_ + step_size ){
			// in case the stepper adapted the step size, make sure you only search
			// for an event within the integration limits

			fmiTime currentTime = fmu_->getTime();
			fmiTime stepSize =  time_ + step_size - fmu_->getTime();
			stepper_->do_step_const( this, states_, currentTime,
						 stepSize
						 );
			fmu_->setContinuousStates( &states_[0] );
			fmu_->setTime( time_ + step_size );
			eventHappened_ = checkStateEvent();
			if ( !eventHappened_ )
				return false;
			tUpper_ = time_ + step_size;
		}
		while ( tUpper_ - tLower_ > eventSearchPrecision/2.0 ){
			// create backup states
			state_type states_bak = states_;

			// let the stepper integrate the left half of the Interval [tLower_,tUpper_]
			//stepper_->reset();
			fmiTime dt = ( tUpper_ - tLower_ )/2.0;
			fmiTime time = tLower_;
			stepper_->do_step_const( this, states_, time, dt );

			// write the result in the model and check for int events
			fmu_->setContinuousStates( &states_[0] );
			fmu_->setTime( time );
			eventHappened_ = checkStateEvent();

			// update the event horizon according to the flag eventHappened_
			if ( ! eventHappened_ ){
				// update the event horizon
				tLower_ = ( tUpper_ + tLower_ )/2.0;
				tUpper_ = tUpper_;
			} else{
				// use the backup variables
				fmu_->setContinuousStates( &states_bak[0] );
				fmu_->setTime( tLower_ );
				states_ = states_bak;

				// reset the stepper
				stepper_->reset();

				// update the event horizon
				tUpper_ = ( tUpper_ + tLower_ )/2.0;
			}
		}
		// make sure the event is *strictly* inside the interval [tLower_, tUpper_]
		tUpper_ += eventSearchPrecision/8.0;
		time_    = tLower_;
		return true;
	}

}


int Integrator::stepperOrder()
{
	return properties_.order;
}

// get time horizon for the event
void Integrator::getEventHorizon( time_type& tLower, time_type& tUpper ){
	tLower = tLower_;
	tUpper = tUpper_;
}


bool Integrator::checkStateEvent(){
	double* newEventsind = new double[ fmu_->nEventInds() ];
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
