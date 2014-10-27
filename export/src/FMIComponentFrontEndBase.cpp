/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file FMIComponentFrontEndBase.cpp

#include <sstream>
#include <stdexcept>

// Bug fix related to C++11 and boost::filesystem::copy_file (linking error).
/// \FIXME This bug fix might become irrelevant for future BOOST releases.
#define BOOST_NO_CXX11_SCOPED_ENUMS

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include "export/include/FMIComponentFrontEndBase.h"
#include "export/include/HelperFunctions.h"


using namespace std;
using namespace cs;


FMIComponentFrontEndBase::FMIComponentFrontEndBase() : functions_( 0 ), loggingOn_( false ) {}


FMIComponentFrontEndBase::~FMIComponentFrontEndBase()
{
	if ( 0 != functions_ ) delete functions_;
}


/// Set internal debug flag and pointer to callback functions.
bool
FMIComponentFrontEndBase::setCallbackFunctions( fmiCallbackFunctions* functions )
{
	if ( 0 == functions_ ) functions_ = new fmiCallbackFunctions;

	if ( ( 0 == functions->logger ) || 
	     ( 0 == functions->allocateMemory ) || 
	     ( 0 == functions->freeMemory ) ) return false; // NB: stepFinished(...) is allowed to be 0!

	functions_->logger = functions->logger;
	functions_->allocateMemory = functions->allocateMemory;
	functions_->freeMemory = functions->freeMemory;
	functions_->stepFinished = functions->stepFinished;

	return true;
}


/// Set internal debug flag.
void
FMIComponentFrontEndBase::setDebugFlag( fmiBoolean loggingOn )
{
	loggingOn_ = loggingOn;
}


/// Call the user-supplied function "stepFinished(...)".
void
FMIComponentFrontEndBase::callStepFinished( fmiStatus status )
{
	if ( 0 != functions_->stepFinished )
		functions_->stepFinished( static_cast<fmiComponent>( this ), status );
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
// input file (entry point).
void
FMIComponentFrontEndBase::parseAdditionalArguments( const ModelDescription& description,
						    string& preArguments,
						    string& postArguments,
						    std::string& executableURI ) const
{
	using namespace ModelDescriptionUtilities;

	if ( description.hasVendorAnnotations() )
	{
		string applicationName = description.getMIMEType().substr( 14 );
		const Properties& vendorAnnotations = description.getVendorAnnotations();
		if ( hasChild( vendorAnnotations, applicationName ) )
		{
			const Properties& annotations = getChildAttributes( vendorAnnotations, applicationName );

			// Command line arguments after the application name but before the
			// main input file (entry point).
			preArguments = hasChild( annotations, "preArguments" ) ?
				annotations.get<string>( "preArguments" ) : string();

			// Command line arguments after the the main input file (entry point).
			postArguments = hasChild( annotations, "postArguments" ) ?
				annotations.get<string>( "postArguments" ) : string();

			// Command line arguments after the the main input file (entry point).
			executableURI = hasChild( annotations, "executableURI" ) ?
				annotations.get<string>( "executableURI" ) : string();
		}
	}
}


// Copy additional input files (specified in XML description elements
// of type  "Implementation.CoSimulation_Tool.Model.File").
bool
FMIComponentFrontEndBase::copyAdditionalInputFiles( const ModelDescription& modelDescription,
						    const string& fmuLocation )
{
	using namespace ModelDescriptionUtilities;
	using namespace boost::filesystem;

	// In case the model description defines some input files, copy them to the current working directory.
	if ( modelDescription.hasImplementation() == true ) {

		const Properties& implementation = modelDescription.getImplementation();
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
						logger( fmiFatal, "ABORT", err );
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
						logger( fmiFatal, "ABORT", err.str() );
						return false;
					}
				}
			}
		}
	}

	return true;
}
