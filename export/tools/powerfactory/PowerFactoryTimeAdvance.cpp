/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file PowerFactoryTimeAdvance.cpp

// Boost library includes.
#include <boost/foreach.hpp>
#include <boost/thread.hpp>

// Check for compilation with Visual Studio 2010 (required).
#if ( _MSC_VER == 1800 )
#include "windows.h"
#else
#error This project requires Visual Studio 2013.
#endif

// Project file includes.
#include "PowerFactoryFrontEnd.h"
#include "PowerFactoryTimeAdvance.h"
#include "import/base/include/ModelDescription.h"


using namespace std;
using namespace pf_api;


TriggerTimeAdvance::TriggerTimeAdvance( PowerFactoryFrontEnd* fe,
					PowerFactory* pf ) :
	PowerFactoryTimeAdvance( fe, pf )
{}


TriggerTimeAdvance::~TriggerTimeAdvance() {}


fmi2Status
TriggerTimeAdvance::instantiate( const ModelDescription::Properties& vendorAnnotations )
{
	using namespace ModelDescriptionUtilities;

	BOOST_FOREACH( const Properties::value_type &v, vendorAnnotations )
	{
		// Check if XML element describes a trigger.
		if ( v.first != "Trigger" ) continue;

		// Extract object name and time scale for trigger.
		const Properties& attributes = getAttributes( v.second );

		// Check if the XML attribute 'name' is available.
		if ( false == hasChild( attributes, "name" ) ) {
			fe_->logger( fmi2Fatal, "FATAL", "attribute 'name' is missing in trigger description" );
			return fmi2Fatal;
		}

		// Check if the XML attribute 'scale' is available.
		if ( false == hasChild( attributes, "scale" ) ) {
			fe_->logger( fmi2Fatal, "FATAL", "attribute 'scale' is missing in trigger description" );
			return fmi2Fatal;
		}

		string name = attributes.get<string>( "name" );
		fmi2Real scale = attributes.get<fmi2Real>( "scale" );

		api::v1::DataObject* trigger;

		// Search for trigger object by class name (SetTrigger) and object name.
		if ( pf_->Ok !=
		     pf_->getActiveStudyCaseObject( "SetTrigger", name, true, trigger ) )
		{
			string err = "[PowerFactoryFrontEnd] trigger not found: ";
			err += name;
			fe_->logger( fmi2Warning, "WARNING", err );
			return fmi2Warning;
		}

		// Activate trigger.
		if ( pf_->Ok != pf_->setAttributeDouble( trigger, "outserv", 0 ) )
		{
			string err = "[PowerFactoryFrontEnd] failed activating the trigger: ";
			err += name;
			fe_->logger( fmi2Warning, "WARNING", err );
			return fmi2Warning;
		}

		// Add trigger to internal list of all available triggers.
		triggers_.push_back( make_pair( trigger, scale ) );
	}

	if ( true == triggers_.empty() ) {
		fe_->logger( fmi2Fatal, "FATAL", "no valid triggers defined" );
		return fmi2Fatal;
	}

	return fmi2OK;
}


fmi2Status
TriggerTimeAdvance::initialize( fmi2Real tStart, fmi2Boolean stopTimeDefined, fmi2Real tStop )
{
	// Iterate through all available triggers.
	TriggerCollection::iterator itTrigger;
	for ( itTrigger = triggers_.begin(); itTrigger != triggers_.end(); ++itTrigger )
	{
		// The trigger value will be set to the start time (scaled).
		fmi2Real value =  tStart / itTrigger->second;

		// Set the trigger value.
		if ( pf_->Ok != pf_->setAttributeDouble( itTrigger->first, "ftrigger", value ) )
		{
			fe_->logger( fmi2Fatal, "ABORT", "only variables of type 'fmi2Real' supported" );
			return fmi2Fatal;
		}
	}

	// Set the start time as the first communication point.
	lastComPoint_ = tStart;

	return fmi2OK;
}


fmi2Status
TriggerTimeAdvance::advanceTime( fmi2Real comPoint, fmi2Real stepSize )
{
	// Sanity check for step size.
	if ( stepSize < 0. ) {
		fe_->logger( fmi2Discard, "DISCARD", "step size has to be greater equal zero" );
		return fmi2Discard;
	}

	// Sanity check for the current communication point.
	if ( fabs( comPoint - lastComPoint_ ) > 1e-9 ) {
		fe_->logger( fmi2Discard, "DISCARD", "wrong communication point" );
		return fmi2Discard;
	}

	// The internal simulation time is set to the communication point plus the step size.
	fmi2Real time = comPoint + stepSize;

	// Iterate through all available triggers.
	TriggerCollection::iterator itTrigger;
	for ( itTrigger = triggers_.begin(); itTrigger != triggers_.end(); ++itTrigger )
	{
		// The trigger value will be set to the current simulation time (scaled).
		fmi2Real value =  time/itTrigger->second;

		// Set the trigger value.
		if ( pf_->Ok != pf_->setAttributeDouble( itTrigger->first, "ftrigger", value ) ) {
			fe_->logger( fmi2Fatal, "ABORT", "could not set trigger value" );
			return fmi2Fatal;
		}
	}

	// Set the current simulation time as the next communication point.
	lastComPoint_ = time;

	return fmi2OK;
}



DPLScriptTimeAdvance::DPLScriptTimeAdvance( PowerFactoryFrontEnd* fe,
					    PowerFactory* pf ) :
	PowerFactoryTimeAdvance( fe, pf )
{}


DPLScriptTimeAdvance::~DPLScriptTimeAdvance() {}


fmi2Status
DPLScriptTimeAdvance::instantiate( const ModelDescription::Properties& vendorAnnotations )
{
	using namespace ModelDescriptionUtilities;

	// Check if XML element describes a DPL script.
	unsigned int numNodes = vendorAnnotations.count( "DPLScript" );
	if ( numNodes != 1 ) {
		stringstream err;
		err << "expect exactly 1 node called 'DPLScript', but got " << numNodes;
		fe_->logger( fmi2Fatal, "FATAL", err.str() );
		return fmi2Fatal;
	}

	// Extract object name, offest and time scale for DPL script.
	const Properties& attributes = getChildAttributes( vendorAnnotations, "DPLScript" );

	// Check if the XML attribute 'name' is available.
	if ( false == hasChild( attributes, "name" ) ) {
		fe_->logger( fmi2Fatal, "FATAL", "attribute 'name' is missing in DPL script description" );
		return fmi2Fatal;
	}

	// Check if the XML attribute 'scale' is available.
	if ( false == hasChild( attributes, "scale" ) ) {
		fe_->logger( fmi2Fatal, "FATAL", "attribute 'scale' is missing in DPL script description" );
		return fmi2Fatal;
	} 

	// Check if the XML attribute 'offset' is available.
	if ( false == hasChild( attributes, "offset" ) ) {
		fe_->logger( fmi2Fatal, "FATAL", "attribute 'offset' is missing in DPL script description" );
		return fmi2Fatal;
	}

	dplScriptName_ = attributes.get<string>( "name" );
	scale_ = attributes.get<fmi2Real>( "scale" );
	offset_ = attributes.get<fmi2Real>( "offset" );

	return fmi2OK;
}


fmi2Status
DPLScriptTimeAdvance::initialize( fmi2Real tStart, fmi2Boolean stopTimeDefined, fmi2Real tStop )
{
	// DPL script input (and output) arguments.
	PowerFactory::VecVariant input, output;
	input.push_back( boost::variant<fmi2Real>( offset_ + tStart/scale_ ) );

	// Execute DPL script.
	if ( pf_->Ok != pf_->executeDPL( dplScriptName_, input, output ) ) {
		fe_->logger( fmi2Warning, "WARNING", "time advance failed" );
		return fmi2Warning;
	}

	// Set the start time as the first communication point.
	lastComPoint_ = tStart;

	return fmi2OK;
}


fmi2Status
DPLScriptTimeAdvance::advanceTime( fmi2Real comPoint, fmi2Real stepSize )
{
	// Sanity check for step size.
	if ( stepSize < 0. ) {
		fe_->logger( fmi2Discard, "DISCARD", "step size has to be greater equal zero" );
		return fmi2Discard;
	}

	// Sanity check for the current communication point.
	if ( fabs( comPoint - lastComPoint_ ) > 1e-9 ) {
		fe_->logger( fmi2Discard, "DISCARD", "wrong communication point" );
		return fmi2Discard;
	}

	// The internal simulation time is set to the communication point plus the step size.
	fmi2Real time = comPoint + stepSize;

	// DPL script input (and output) arguments.
	PowerFactory::VecVariant input, output;
	input.push_back( boost::variant<fmi2Real>( offset_ + time/scale_ ) );

	// Execute DPL script.
	if ( pf_->Ok != pf_->executeDPL( dplScriptName_, input, output ) ) {
		fe_->logger( fmi2Warning, "WARNING", "time advance failed" );
		return fmi2Warning;
	}

	// Set the current simulation time as the next communication point.
	lastComPoint_ = time;

	return fmi2OK;
}
