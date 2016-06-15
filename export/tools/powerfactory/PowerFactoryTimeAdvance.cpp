/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file PowerFactoryTimeAdvance.cpp

// Check for compilation with Visual Studio 2010 (required).
#if ( _MSC_VER == 1800 )
#include "windows.h"
#else
#error This project requires Visual Studio 2013.
#endif

// Boost library includes.
#include <boost/foreach.hpp>
#include <boost/thread.hpp>

// Project file includes.
#include "PowerFactoryFrontEnd.h"
#include "import/base/include/ModelDescription.h"

// PFSim project includes (advanced PowerFactory wrapper)
#include "Types.h"
#include "PowerFactory.h"

#include "PowerFactoryTimeAdvance.h"


using namespace std;


TriggerTimeAdvance::TriggerTimeAdvance( PowerFactoryFrontEnd* fe,
					PowerFactory* pf ) :
	PowerFactoryTimeAdvance( fe, pf )
{}


TriggerTimeAdvance::~TriggerTimeAdvance() {}


fmiStatus
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
			fe_->logger( fmiFatal, "FATAL", "attribute 'name' is missing in trigger description" );
			return fmiFatal;
		}

		// Check if the XML attribute 'scale' is available.
		if ( false == hasChild( attributes, "scale" ) ) {
			fe_->logger( fmiFatal, "FATAL", "attribute 'scale' is missing in trigger description" );
			return fmiFatal;
		}

		string name = attributes.get<string>( "name" );
		fmiReal scale = attributes.get<fmiReal>( "scale" );

		api::v1::DataObject* trigger;

		// Search for trigger object by class name (SetTrigger) and object name.
		if ( pf_->Ok !=
		     pf_->getActiveStudyCaseObject( "SetTrigger", name, true, trigger ) )
		{
			string err = "[PowerFactoryFrontEnd] trigger not found: ";
			err += name;
			fe_->logger( fmiWarning, "WARNING", err );
			return fmiWarning;
		}

		// Activate trigger.
		if ( pf_->Ok != pf_->setAttributeDouble( trigger, "outserv", 0 ) )
		{
			string err = "[PowerFactoryFrontEnd] failed activating the trigger: ";
			err += name;
			fe_->logger( fmiWarning, "WARNING", err );
			return fmiWarning;
		}

		// Add trigger to internal list of all available triggers.
		triggers_.push_back( make_pair( trigger, scale ) );
	}

	if ( true == triggers_.empty() ) {
		fe_->logger( fmiFatal, "FATAL", "no valid triggers defined" );
		return fmiFatal;
	}

	return fmiOK;
}


fmiStatus
TriggerTimeAdvance::initialize( fmiReal tStart, fmiBoolean stopTimeDefined, fmiReal tStop )
{
	// Iterate through all available triggers.
	TriggerCollection::iterator itTrigger;
	for ( itTrigger = triggers_.begin(); itTrigger != triggers_.end(); ++itTrigger )
	{
		// The trigger value will be set to the start time (scaled).
		fmiReal value =  tStart / itTrigger->second;

		// Set the trigger value.
		if ( pf_->Ok != pf_->setAttributeDouble( itTrigger->first, "ftrigger", value ) )
		{
			fe_->logger( fmiFatal, "ABORT", "only variables of type 'fmiReal' supported" );
			return fmiFatal;
		}
	}

	// Set the start time as the first communication point.
	lastComPoint_ = tStart;

	return fmiOK;
}


fmiStatus
TriggerTimeAdvance::advanceTime( fmiReal comPoint, fmiReal stepSize )
{
	// Sanity check for step size.
	if ( stepSize < 0. ) {
		fe_->logger( fmiDiscard, "DISCARD", "step size has to be greater equal zero" );
		return fmiDiscard;
	}

	// Sanity check for the current communication point.
	if ( fabs( comPoint - lastComPoint_ ) > 1e-9 ) {
		fe_->logger( fmiDiscard, "DISCARD", "wrong communication point" );
		return fmiDiscard;
	}

	// The internal simulation time is set to the communication point plus the step size.
	fmiReal time = comPoint + stepSize;

	// Iterate through all available triggers.
	TriggerCollection::iterator itTrigger;
	for ( itTrigger = triggers_.begin(); itTrigger != triggers_.end(); ++itTrigger )
	{
		// The trigger value will be set to the current simulation time (scaled).
		fmiReal value =  time/itTrigger->second;

		// Set the trigger value.
		if ( pf_->Ok != pf_->setAttributeDouble( itTrigger->first, "ftrigger", value ) ) {
			fe_->logger( fmiFatal, "ABORT", "could not set trigger value" );
			return fmiFatal;
		}
	}

	// Set the current simulation time as the next communication point.
	lastComPoint_ = time;

	return fmiOK;
}



DPLScriptTimeAdvance::DPLScriptTimeAdvance( PowerFactoryFrontEnd* fe,
					    PowerFactory* pf ) :
	PowerFactoryTimeAdvance( fe, pf )
{}


DPLScriptTimeAdvance::~DPLScriptTimeAdvance() {}


fmiStatus
DPLScriptTimeAdvance::instantiate( const ModelDescription::Properties& vendorAnnotations )
{
	using namespace ModelDescriptionUtilities;

	// Check if XML element describes a DPL script.
	unsigned int numNodes = vendorAnnotations.count( "DPLScript" );
	if ( numNodes != 1 ) {
		stringstream err;
		err << "expect exactly 1 node called 'DPLScript', but got " << numNodes;
		fe_->logger( fmiFatal, "FATAL", err.str() );
		return fmiFatal;
	}

	// Extract object name, offest and time scale for DPL script.
	const Properties& attributes = getChildAttributes( vendorAnnotations, "DPLScript" );

	// Check if the XML attribute 'name' is available.
	if ( false == hasChild( attributes, "name" ) ) {
		fe_->logger( fmiFatal, "FATAL", "attribute 'name' is missing in DPL script description" );
		return fmiFatal;
	}

	// Check if the XML attribute 'scale' is available.
	if ( false == hasChild( attributes, "scale" ) ) {
		fe_->logger( fmiFatal, "FATAL", "attribute 'scale' is missing in DPL script description" );
		return fmiFatal;
	} 

	// Check if the XML attribute 'offset' is available.
	if ( false == hasChild( attributes, "offset" ) ) {
		fe_->logger( fmiFatal, "FATAL", "attribute 'offset' is missing in DPL script description" );
		return fmiFatal;
	}

	dplScriptName_ = attributes.get<string>( "name" );
	scale_ = attributes.get<fmiReal>( "scale" );
	offset_ = attributes.get<fmiReal>( "offset" );

	return fmiOK;
}


fmiStatus
DPLScriptTimeAdvance::initialize( fmiReal tStart, fmiBoolean stopTimeDefined, fmiReal tStop )
{
	// DPL script input (and output) arguments.
	VecVariant input, output;
	input.push_back( boost::variant<fmiReal>( offset_ + tStart/scale_ ) );

	// Execute DPL script.
	if ( pf_->Ok != pf_->executeDPL( dplScriptName_, input, output ) ) {
		fe_->logger( fmiWarning, "WARNING", "time advance failed" );
		return fmiWarning;
	}

	// Set the start time as the first communication point.
	lastComPoint_ = tStart;

	return fmiOK;
}


fmiStatus
DPLScriptTimeAdvance::advanceTime( fmiReal comPoint, fmiReal stepSize )
{
	// Sanity check for step size.
	if ( stepSize < 0. ) {
		fe_->logger( fmiDiscard, "DISCARD", "step size has to be greater equal zero" );
		return fmiDiscard;
	}

	// Sanity check for the current communication point.
	if ( fabs( comPoint - lastComPoint_ ) > 1e-9 ) {
		fe_->logger( fmiDiscard, "DISCARD", "wrong communication point" );
		return fmiDiscard;
	}

	// The internal simulation time is set to the communication point plus the step size.
	fmiReal time = comPoint + stepSize;

	// DPL script input (and output) arguments.
	VecVariant input, output;
	input.push_back( boost::variant<fmiReal>( offset_ + time/scale_ ) );

	// Execute DPL script.
	if ( pf_->Ok != pf_->executeDPL( dplScriptName_, input, output ) ) {
		fe_->logger( fmiWarning, "WARNING", "time advance failed" );
		return fmiWarning;
	}

	// Set the current simulation time as the next communication point.
	lastComPoint_ = time;

	return fmiOK;
}
