/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file PowerFactoryTimeAdvance.cpp

// Check for compilation with Visual Studio 2010 (required).
#if ( _MSC_VER == 1600 )
#include "Windows.h"
#else
#error This project requires Visual Studio 2010.
#endif

// // Standard library includes.
// #include <sstream>
// #include <stdexcept>

// Boost library includes.
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
// #include <boost/algorithm/string.hpp>
// #include <boost/lexical_cast.hpp>

// Project file includes.
#include "PowerFactoryFrontEnd.h"
// #include "PowerFactoryRealScalar.h"
// #include "export/include/HelperFunctions.h"

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
		string name = attributes.get<string>( "name" );
		fmiReal scale = attributes.get<fmiReal>( "scale" );

		api::DataObject* trigger;

		// Search for trigger object by class name (SetTrigger) and object name.
		if ( pf_->Ok !=
		     pf_->getActiveStudyCaseObject( "SetTrigger", name, false, trigger ) )
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
	string dplScriptName_ = attributes.get<string>( "name" );
	fmiReal scale_ = attributes.get<fmiReal>( "scale" );
	fmiReal offset_ = attributes.get<fmiReal>( "offest" );

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
