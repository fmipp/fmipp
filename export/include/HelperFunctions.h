// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file HelperFunctions.h


#ifndef _FMIPP_HELPERFUNCTIONS_H
#define _FMIPP_HELPERFUNCTIONS_H

#include <vector>
#include <string>

#include <boost/property_tree/ptree.hpp>

#ifdef WIN32
#include "windows.h"
#endif


/// Helper functions for string manipulation.
namespace HelperFunctions
{
	/// Split a string according to a given seperator and trim the resulting
	/// sub-strings (i.e., remove leading or trainling white spaces).
	void splitAndTrim( const std::string& input,
		std::vector<std::string>& result,
		const std::string& seperators );


	/// Trim a string (i.e., remove leading or trainling white spaces).
	void trim( const std::string& input,
		std::string& result );

#ifdef WIN32
	/// Copy a string to a new TCHAR pointer (caller takes ownership).
	TCHAR* copyStringToTCHAR( const std::string& str,
		size_t extra_length = 0 );
#endif

	/// Convert an URL to an OS-specific path.
	bool getPathFromUrl( const std::string& inputFileUrl, std::string& outputFilePath );


	/// Retrieve data from file. The file is expected to have one entry per line, comment
	/// lines start with a semicolon (;). Return value is 'false' in case the specified
	/// file cannot be found/opened. In case the file can be found/opened, the result vector
	/// is cleared (all existing elements are removed) and filled with the data provided
	/// from the file.
	bool readDataFromFile( const std::string& file_name,
		std::vector<std::string>& result );


	bool copyFile( const boost::property_tree::ptree& fileAttributes,
		const std::string& fmuLocation,
		std::string& err );


	/// A file URI may start with "fmu://". In that case the
	/// FMU's location has to be prepended to the URI accordingly.
	void processURI( std::string& uri, const std::string& fmuLocation );


	/// Copy the contents of a vector of strings to a property tree
	void addVectorToTree( boost::property_tree::ptree& tree,
		const std::vector< std::string >& vector,
		const std::string& childName );

}


#endif // _FMIPP_HELPERFUNCTIONS_H
