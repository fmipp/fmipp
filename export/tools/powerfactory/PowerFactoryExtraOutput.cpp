/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file PowerFactoryExtraOutput.cpp

// standard includes
#include <string>
#include <map>
#include <limits>

// boost includes
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/info_parser.hpp>

// PFSim project includes (advanced PowerFactory wrapper)
#include "Types.h"
#include "PowerFactory.h"

#include "PowerFactoryFrontEnd.h"
#include "PowerFactoryRealScalar.h"
#include "PowerFactoryExtraOutput.h"


using namespace std;


/// Destructor.
PowerFactoryExtraOutput::~PowerFactoryExtraOutput()
{
	// Delete the list of extra outputs.
	BOOST_FOREACH( ExtraOutputList::value_type& v, extraOutputs_ ) {
		delete v.first; // Delete output stream.
		BOOST_FOREACH( ExtraOutputSet::value_type& vv, *v.second )
			delete vv; // Delete elements of extra output scalar list.
		delete v.second; // Delete extra output scalar list.
	}

}

/// Initialize outputs streams and lists of scalar variables for extra output.
bool
PowerFactoryExtraOutput::initializeExtraOutput( PowerFactory* pf )
{
	using namespace boost::filesystem;
	using namespace boost::property_tree::info_parser;
	using namespace boost::property_tree;

	// String for logging.
	string log;

	directory_iterator itEnd; // Default ctor yields past-the-end.
	for( directory_iterator it( current_path() ); it != itEnd; ++it )
	{
		// Skip if not a file.
		if( !is_regular_file( it->status() ) ) continue;

		// Skip if no match.
		if( it->path().extension() != ".info" ) continue;

		// Name of file listing names of extra outputs.
		string infoFileName = it->path().filename().string();
		log = "read additional outputs from file '" + infoFileName + "'";
		logger( fmiOK, "DEBUG", log );

		// Name of file for writing extra outputs.
		string outStreamName = it->path().stem().string() + ".csv";
		log = "write additional simulations results to file '" + outStreamName + "'";
		logger( fmiOK, "DEBUG", log );

		// New output stream for extra outputs.
		ofstream* outStream = new ofstream( outStreamName, ios_base::trunc );
		*outStream << "# time"; // First entry of output file description line.
		string delimiter( ", " ); // Define delimiter for seperating data entries.

		// New set of scalar variables representing the extra output variables.
		ExtraOutputSet* outVariables = new ExtraOutputSet;

		// Property tree for parsing the file listing the extra outputs.
		ptree data;

		// Parse file.
		read_info( infoFileName, data );

		// Loop over extra outputs variable names.
		BOOST_FOREACH( const ptree::value_type &dataEntry, data )
		{
			PowerFactoryRealScalar* scalar = new PowerFactoryRealScalar;

			// Parse class name, object name and parameter name from description.
			bool parseStatus =
				PowerFactoryRealScalar::parseFMIVariableName( dataEntry.first,
									      scalar->className_,
									      scalar->objectName_,
									      scalar->parameterName_ );

			if ( false == parseStatus ) {
				ostringstream err;
				err << "bad variable name: " << dataEntry.first;
				logger( fmiWarning, "WARNING", err.str() );
				return false;
			}


			// Search for PowerFactory object by class name and object name.
			api::DataObject* dataObj = 0;
			int check = -1;
			check = pf->getCalcRelevantObject( scalar->className_, scalar->objectName_, dataObj );
			if ( check != pf->Ok )
			{
				ostringstream err;
				err << "unable to get object: " << scalar->objectName_
				    << " (type " << scalar->className_ << ")";
				logger( fmiWarning, "WARNING", err.str() );
				return false;
			} else if ( 0 != dataObj ) {
				scalar->apiDataObject_ = dataObj;
			}

			// Add scalar variable to list of extra outputs.
			outVariables->push_back( scalar );

			// Add name of variable to description line of output file.
			*outStream << delimiter << scalar->objectName_ << "." << scalar->parameterName_;

			log = "add extra output '" + dataEntry.first + "'";
			logger( fmiOK, "DEBUG", log );
		}

		// Add a carriage return to output file descrition line.
		*outStream << endl;

		// Add output stream and list of extra output scalar varaiables to list of extra outputs.
		extraOutputs_.push_back( make_pair( outStream, outVariables ) );
	}

	return true;
}


/// Initialize outputs streams and lists of scalar variables for extra output.
bool
PowerFactoryExtraOutput::writeExtraOutput( const fmiReal currentSyncPoint,
					   PowerFactory* pf )
{
	string delimiter( "," );

	BOOST_FOREACH( ExtraOutput& outputs, extraOutputs_ )
	{

		ExtraOutputSet* outVariables = outputs.second; // Get the list of outputs variables.

		// Write output to ostringstream first, then write the whole line to output stream at once.
		ostringstream outputLine;

		// Always write current simulation time as first element of output line (with maximal precision).
		outputLine << setprecision( std::numeric_limits<fmiReal>::max_digits10 ) << currentSyncPoint;

		// Reset numerical precision for remaining values to reasonable value.
		outputLine << setprecision( precision_ );
		
		fmiReal val;

		// Loop over output variables.
		BOOST_FOREACH( ExtraOutputSet::value_type& scalar, *outVariables )
		{
			// Extract data from PowerFactory object using the parameter name.
			if ( ( 0 == scalar->apiDataObject_ ) ||
			     ( pf->getAttributeDouble( scalar->apiDataObject_, scalar->parameterName_.c_str(), val ) != pf->Ok ) )
			{
				ostringstream err;
				err << "not able to read data of object: " << scalar->objectName_
				    << " (type " << scalar->className_ << ")";
				logger( fmiWarning, "WARNING", err.str() );
				return false;
			}

			// Write value to output.
			outputLine << delimiter << val;
		}

		// Get the output stream.
		ofstream* outStream = outputs.first;

		// Write data to output stream and add carriage return to end of output line.
		*outStream << outputLine.str() << endl;
	}

	return true;
}


void
PowerFactoryExtraOutput::logger( fmiStatus status,
				 const string& category,
				 const string& msg )
{
	if ( ( status == fmiOK ) && ( fmiFalse == loggingOn_ ) ) return;

	functions_->logger( static_cast<fmiComponent>( this ),
			    "PowerFactoryExtraOutput", status,
			    category.c_str(), msg.c_str() );
}
