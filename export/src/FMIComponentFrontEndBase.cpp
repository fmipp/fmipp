// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file FMIComponentFrontEndBase.cpp

#include <sstream>
#include <stdexcept>

#include <boost/foreach.hpp>

#include "export/include/FMIComponentFrontEndBase.h"
#include "export/include/HelperFunctions.h"
#include "import/base/include/ModelDescription.h"

using namespace std;

FMIComponentFrontEndBase::FMIComponentFrontEndBase() : fmiFunctions_( 0 ), fmi2Functions_( 0 ), loggingOn_( false ) {}

FMIComponentFrontEndBase::~FMIComponentFrontEndBase()
{
	if ( 0 != fmiFunctions_ ) delete fmiFunctions_;
	if ( 0 != fmi2Functions_ ) delete fmi2Functions_;
}

/// Set internal debug flag and pointer to callback functions (FMI 1.0 backward compatibility).
bool
FMIComponentFrontEndBase::setCallbackFunctions( cs::fmiCallbackFunctions* functions )
{
	if ( 0 == fmiFunctions_ ) fmiFunctions_ = new cs::fmiCallbackFunctions;

	if ( ( 0 == functions->logger ) || 
	     ( 0 == functions->allocateMemory ) || 
	     ( 0 == functions->freeMemory ) ) return false; // NB: stepFinished(...) is allowed to be 0!

	fmiFunctions_->logger = functions->logger;
	fmiFunctions_->allocateMemory = functions->allocateMemory;
	fmiFunctions_->freeMemory = functions->freeMemory;
	fmiFunctions_->stepFinished = functions->stepFinished;

	return true;
}

/// Set internal debug flag and pointer to callback functions.
bool
FMIComponentFrontEndBase::setCallbackFunctions( fmi2::fmi2CallbackFunctions* functions )
{
	if ( 0 == fmi2Functions_ ) fmi2Functions_ = new fmi2::fmi2CallbackFunctions;
	
	if ( ( 0 == functions->logger ) || 
	     ( 0 == functions->allocateMemory ) || 
	     ( 0 == functions->freeMemory ) ) return false; // NB: stepFinished(...) and is allowed to be 0!

	fmi2Functions_->logger = functions->logger;
	fmi2Functions_->allocateMemory = functions->allocateMemory;
	fmi2Functions_->freeMemory = functions->freeMemory;
	fmi2Functions_->stepFinished = functions->stepFinished;
	fmi2Functions_->componentEnvironment = functions->componentEnvironment;
	
	return true;
}

/// Set internal debug flag.
void
FMIComponentFrontEndBase::setDebugFlag( fmippBoolean loggingOn )
{
	loggingOn_ = loggingOn;
}

/// Call the user-supplied function "stepFinished(...)".
void
FMIComponentFrontEndBase::callStepFinished( fmippStatus status )
{
	if ( 0 != fmiFunctions_ && 0 != fmiFunctions_->stepFinished ) // FMI 1.0 backward compatibility.
		fmiFunctions_->stepFinished( static_cast<fmiComponent>( this ), static_cast<fmiStatus>( status ) );

	if ( 0 != fmi2Functions_ && 0 != fmi2Functions_->stepFinished )
		fmi2Functions_->stepFinished( fmi2Functions_->componentEnvironment, static_cast<fmi2Status>( status ) );
}

// Check for additional command line arguments (as part of optional vendor
// annotations). Get command line arguments that are supposed to come
// between the applications name and the main input file (entry point).
// Get command line arguments that are supposed to come after the main
// input file (entry point). A main argument can be specified, which should
// then be used instead of just the filename as main command line argument
// when starting the external application.
bool
FMIComponentFrontEndBase::parseAdditionalArguments(
	const ModelDescription* description,
	string& preArguments,
	string& mainArguments,
	string& postArguments,
	string& executableURI,
	string& entryPointURI ) const
{
	using namespace ModelDescriptionUtilities;

	Properties additionalArguments;
	
	string toolXmlTag;
	if ( 1 == description->getVersion() ) toolXmlTag = description->getMIMEType().substr( 14 );
	if ( 2 == description->getVersion() ) toolXmlTag = string( "FMI++Export" );

	if ( true == description->hasVendorAnnotationsTool() )
	{
		const Properties& vendorAnnotations = description->getVendorAnnotations();
		BOOST_FOREACH( const Properties::value_type &v, vendorAnnotations ) // Iterate vendor annotations.
		{
			if ( v.first == "Tool" ) // Check if node describes a "Tool".
			{
				const Properties& toolAttributes = getAttributes( v.second );
				string toolName = toolAttributes.get<string>( "name" );
				if ( string::npos != toolName.find( toolXmlTag ) )
				{
					if ( hasChild( v.second, "Executable" ) ) {
						additionalArguments = getChildAttributes( v.second, "Executable" );
						break;
					} else {
						return false;
					}
				}
			}
		}
	} else {
		return false;
	}

	// Command line arguments after the application name but before the main argument.
	preArguments = hasChild( additionalArguments, "preArguments" ) ?
		additionalArguments.get<string>( "preArguments" ) : string();

	// Command line main argument. If empty, main input file (entry point) will be used.
	mainArguments = hasChild( additionalArguments, "arguments" ) ?
		additionalArguments.get<string>( "arguments" ) : string();

	// Command line arguments after the the main argument.
	postArguments = hasChild( additionalArguments, "postArguments" ) ?
		additionalArguments.get<string>( "postArguments" ) : string();

	// URI of the executable.
	executableURI = hasChild( additionalArguments, "executableURI" ) ?
		additionalArguments.get<string>( "executableURI" ) : string();

	// URI of the main input file (entry point).
	entryPointURI = hasChild( additionalArguments, "entryPointURI" ) ?
		additionalArguments.get<string>( "entryPointURI" ) : string();

	return true;
}

// Copy additional input files (specified in XML description elements
// of type  "Implementation.CoSimulation_Tool.Model.File").
bool
FMIComponentFrontEndBase::copyAdditionalInputFiles( const ModelDescription* modelDescription,
	const string& fmuLocation )
{
	using namespace ModelDescriptionUtilities;

	// In case the model description defines some input files, copy them to the current working directory.
	if ( 1 == modelDescription->getVersion() && modelDescription->hasImplementation() == true ) // FMI 1.0
	{
		const Properties& implementation = modelDescription->getImplementation();
		if ( hasChild( implementation, "CoSimulation_Tool.Model" ) )
		{
			// Iterate through XML elements of description "CoSimulation_Tool.Model" and
			// check if any additional files are specified.
			const Properties& csModel = implementation.get_child( "CoSimulation_Tool.Model" );
			BOOST_FOREACH( const Properties::value_type &v, csModel )
			{
				if ( v.first == "File" )
				{
					// Retrieve file attributes and copy file.
					const Properties& fileAttributes = getAttributes( v.second );
					string err;
					if ( false == HelperFunctions::copyFile( fileAttributes, fmuLocation, err ) )
					{
						logger( fmippFatal, "ABORT", err );
						return false;
					}
				}
			}
		}
	}
	else if ( 2 == modelDescription->getVersion() && modelDescription->hasVendorAnnotationsTool() ) // FMI 2.0
	{
		const Properties& vendorAnnotations = modelDescription->getVendorAnnotations();
		BOOST_FOREACH( const Properties::value_type &v, vendorAnnotations ) // Iterate vendor annotations.
		{
			if ( v.first == "Tool" ) // Check if node describes a "Tool".
			{
				const Properties& toolAttributes = getAttributes( v.second );
				string toolName = toolAttributes.get<string>( "name" );

				if ( string::npos != toolName.find( "FMI++Export" ) ) // Check if tool description is for "FMI++Export".
				{
					BOOST_FOREACH( const Properties::value_type &vv, v.second ) // Iterate tool description.
					{
						if ( vv.first == "File" ) // Check if node describes a "File".
						{
							// Retrieve file attributes and copy file.
							const Properties& fileAttributes = getAttributes( vv.second );
							string err;
							if ( false == HelperFunctions::copyFile( fileAttributes, fmuLocation, err ) )
							{
								logger( fmippFatal, "ABORT", err );
								return false;
							}
						}
					}
				}
			}
		}		
	}

	return true;
}
