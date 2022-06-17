// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file HelperFunctions.cpp

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS // Turn of warnings concerning iterator bound checks.
#endif

#ifdef WIN32
#include "shlwapi.h"
#endif

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/info_parser.hpp>

// Bug fix related to C++11 and boost::filesystem::copy_file (linking error).
/// \FIXME This bug fix might become irrelevant for future BOOST releases.
#if !defined(_MSC_VER) || _MSC_VER < 1700
	#define BOOST_NO_CXX11_SCOPED_ENUMS
#endif
#include <boost/filesystem.hpp>

#include "export/include/HelperFunctions.h"

using namespace std;

namespace HelperFunctions {

	void splitAndTrim( const string& input,
		vector<string>& result,
		const string& seperators )
	{
		boost::split( result, input, boost::is_any_of( seperators ) );
		for_each( result.begin(), result.end(), boost::bind( &boost::trim<string>, _1, locale() ) );
	}

	void trim( const string& input, string& result )
	{
		result = boost::trim_copy( input );
	}

#ifdef WIN32
	TCHAR* copyStringToTCHAR( const string& str, size_t extra_length )
	{
		TCHAR *result = new TCHAR[str.size() + extra_length + 1];
		result[str.size()] = 0;
		copy( str.begin(), str.end(), result );
		return result;
	}
#endif

	bool
	getPathFromUrl( const string& inputFileUrl, string& outputFilePath )
	{
#ifdef WIN32
		LPCTSTR fileUrl = HelperFunctions::copyStringToTCHAR( inputFileUrl );
		LPTSTR filePath = new TCHAR[MAX_PATH];
		DWORD filePathSize = MAX_PATH;
		DWORD tmp = 0;
		HRESULT res = PathCreateFromUrl( fileUrl, filePath, &filePathSize, tmp );
		outputFilePath = string( filePath );
		delete fileUrl;
		delete filePath;
		return ( S_OK == res );
#else
		// Reject URLs that do not correspond to (local) files.
		if ( inputFileUrl.substr( 0, 7 ) != "file://" ) return false;

		outputFilePath = inputFileUrl.substr( 7, inputFileUrl.size() );
		return true;
#endif
	}

	/// Retrieve data from file. The file is expected to have one entry per line, comment
	/// lines start with a semicolon (;). Return value is 'false' in case the specified
	/// file cannot be found/opened. In case the file can be found/opened, the result vector
	/// is cleared (all existing elements are removed) and filled with the data provided
	/// from the file.
	bool readDataFromFile( const string& file_name,
		vector<string>& result )
	{
		using namespace boost::property_tree::info_parser;
		using namespace boost::property_tree;

		// Check if file exists.
		boost::filesystem::path p( file_name );
		if ( false == boost::filesystem::exists( p ) ) return false;
		if ( false == boost::filesystem::is_regular_file( p ) ) return false;

		// Property tree for parsing the file listing the extra outputs.
		ptree data;

		try {
			// Parse file.
			read_info( file_name, data );
		} catch ( const info_parser_error& err ) {
			// Return false if the file cannot be read, doesn't contain valid INFO,
			// or a conversion fails.
			return false;
		}

		// Clear results vector (remove all existing elements).
		result.clear();

		// Loop over extra outputs variable names.
		BOOST_FOREACH( const ptree::value_type &dataEntry, data )
		{
			result.push_back( dataEntry.first );
		}

		return true;
	}

	bool copyFile( const boost::property_tree::ptree& fileAttributes,
		const string& fmuLocation, string& err )
	{
		using namespace boost::filesystem;
		
		string fileName = fileAttributes.get<string>( "file" );
		// A file URI may start with "fmu://". In that case the
		// FMU's location has to be prepended to the URI accordingly.
		processURI( fileName, fmuLocation );
	
		string strFilePath;
		if ( false == HelperFunctions::getPathFromUrl( fileName, strFilePath ) ) {
			err = string ( "invalid input URL for additional input file" );
			return false;
		}
	
		// Use Boost tools for file manipulation.
		path filePath( strFilePath );
		if ( is_regular_file( filePath ) ) { // Check if regular file.
			// Copy to working directory.
			path copyToPath = current_path() /= filePath.filename();
			// Copy file.
			copy_file( filePath, copyToPath, copy_option::overwrite_if_exists );
		} else {
			stringstream sserr;
			sserr << "File not found: " << filePath;
			err = sserr.str();
			return false;
		}
		
		return true;
	}

	// A file URI may start with "fmu://". In that case the
	// FMU's location has to be prepended to the URI accordingly.
	void processURI( string& uri,
		const string& fmuLocation )
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

	void addVectorToTree( boost::property_tree::ptree& tree,
		const vector< string >& vector,
		const string& childName )
	{
		boost::property_tree::ptree treeForVector;
		boost::property_tree::ptree vectorElement;

		BOOST_FOREACH( string s, vector ) {
			vectorElement.put_value( s );
			treeForVector.push_back( make_pair( "", vectorElement ) );
		}

		tree.add_child( childName, treeForVector );
	}
}
