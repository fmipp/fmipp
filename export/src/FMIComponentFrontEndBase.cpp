/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file FMIComponentFrontEndBase.cpp

#include <sstream>
#include <stdexcept>

// Bug fix related to C++11 and boost::filesystem::copy_file (linking error).
/// \FIXME This bug fix might become irrelevant for future BOOST releases.
#if !defined(_MSC_VER) || _MSC_VER < 1700
	#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif

#include <boost/filesystem.hpp>
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
FMIComponentFrontEndBase::setDebugFlag( fmi2Boolean loggingOn )
{
	loggingOn_ = loggingOn;
}


/// Call the user-supplied function "stepFinished(...)".
void
FMIComponentFrontEndBase::callStepFinished( fmi2Status status )
{
	if ( 0 != fmiFunctions_ && 0 != fmiFunctions_->stepFinished ) // FMI 1.0 backward compatibility.
		fmiFunctions_->stepFinished( static_cast<fmiComponent>( this ), static_cast<fmiStatus>( status ) );

	if ( 0 != fmi2Functions_ && 0 != fmi2Functions_->stepFinished )
		fmi2Functions_->stepFinished( fmi2Functions_->componentEnvironment, status );
}


// A file URI may start with "fmu://". In that case the
// FMU's location has to be prepended to the URI accordingly.
void
FMIComponentFrontEndBase::processURI( string& uri,
				      const string& fmuLocation ) const
{
	if ( uri.substr( 0, 6 ) == string( "fmu://" ) ) {
		// Check if the FMU's location has a trailing '/'.
		if ( fmuLocation.at( fmuLocation.size() - 1 ) == '/' )
		{
			uri = fmuLocation + uri.substr( 6 );
		} else {
			uri = fmuLocation + uri.substr( 5 );
		}
	}
}


// Check for additional command line arguments (as part of optional vendor
// annotations). Get command line arguments that are supposed to come
// between the applications name and the main input file (entry point).
// Get command line arguments that are supposed to come after the main
// input file (entry point). A main argument can be specified, which should
// then be used instead of just the filename as main command line argument
// when starting the external application.
void
FMIComponentFrontEndBase::parseAdditionalArguments( const ModelDescription* description,
						    string& preArguments,
							string& mainArguments,
						    string& postArguments,
						    string& executableURI,
							string& entryPointURI ) const
{
	using namespace ModelDescriptionUtilities;

	if ( description->hasVendorAnnotations() )
	{
		string applicationName( "unknown_application" );

		if ( 1 == description->getVersion() ) {
			applicationName = description->getMIMEType().substr( 14 );
		}
		else if ( 2 == description->getVersion() ) // Try to use "generationTool" from model description.
		{
			const Properties& attributes = description->getModelAttributes();
			applicationName = attributes.get<string>( "generationTool" );
		}

		const Properties& vendorAnnotations = description->getVendorAnnotations();
		if ( hasChild( vendorAnnotations, applicationName ) )
		{
			const Properties& annotations = getChildAttributes( vendorAnnotations, applicationName );

			// Command line arguments after the application name but before the
			// main input file (entry point).
			preArguments = hasChild( annotations, "preArguments" ) ?
				annotations.get<string>( "preArguments" ) : string();

			// Command line arguments after the the main input file (entry point).
			mainArguments = hasChild( annotations, "arguments" ) ?
				annotations.get<string>( "arguments" ) : string();

				// Command line arguments after the the main input file (entry point).
			postArguments = hasChild( annotations, "postArguments" ) ?
				annotations.get<string>( "postArguments" ) : string();

			// Command line arguments after the the main input file (entry point).
			executableURI = hasChild( annotations, "executableURI" ) ?
				annotations.get<string>( "executableURI" ) : string();

			// Command line arguments after the the main input file (entry point).
			entryPointURI = hasChild( annotations, "entryPointURI" ) ?
				annotations.get<string>( "entryPointURI" ) : string();
		}
	}
}


// Copy additional input files (specified in XML description elements
// of type  "Implementation.CoSimulation_Tool.Model.File").
bool
FMIComponentFrontEndBase::copyAdditionalInputFiles( const ModelDescription* modelDescription,
						    const string& fmuLocation )
{
	using namespace ModelDescriptionUtilities;
	using namespace boost::filesystem;

	// In case the model description defines some input files, copy them to the current working directory.
	if ( modelDescription->hasImplementation() == true ) {

		const Properties& implementation = modelDescription->getImplementation();
		if ( hasChild( implementation, "CoSimulation_Tool.Model" ) ) {

			// Iterate through XML elements of description "CoSimulation_Tool.Model" and
			// check if any additional files are specified.
			const Properties& csModel = implementation.get_child( "CoSimulation_Tool.Model" );
			BOOST_FOREACH( const Properties::value_type &v, csModel )
			{
				if ( v.first == "File" ) {
					// Get file URI.
					const Properties& attributes = getAttributes( v.second );
					string fileName = attributes.get<string>( "file" );
					// A file URI may start with "fmu://". In that case the
					// FMU's location has to be prepended to the URI accordingly.
					processURI( fileName, fmuLocation );

					string strFilePath;
					if ( false == HelperFunctions::getPathFromUrl( fileName, strFilePath ) ) {
						string err( "invalid input URL for additional input file" );
						logger( fmi2Fatal, "ABORT", err );
						return false;
					}


					// Use Boost tools for file manipulation.
					path filePath( strFilePath );
					if ( is_regular_file( filePath ) ) { // Check if regular file.
						// Copy to working directory.
						path copyToPath = current_path() /= filePath.filename();
						// Copy file.
						copy_file( filePath, copyToPath,
							   copy_option::overwrite_if_exists );
					} else {
						stringstream err;
						err << "File not found: " << filePath;
						logger( fmi2Fatal, "ABORT", err.str() );
						return false;
					}
				}
			}
		}
	}

	return true;
}
