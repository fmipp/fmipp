/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file HelperFunctions.cpp

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS // Turn of warnings concerning iterator bound checks.
#endif

#ifdef WIN32
#include "Shlwapi.h"
#endif

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>

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

	void trim( const string& input,
		   string& result )
	{
		result = boost::trim_copy( input );
	}


#ifdef WIN32
	TCHAR* copyStringToTCHAR( const std::string& str, size_t extra_length )
	{
		TCHAR *result = new TCHAR[str.size() + extra_length + 1];
		result[str.size()] = 0;
		copy( str.begin(), str.end(), result );
		return result;
	}
#endif

	bool
	getPathFromUrl( const std::string& inputFileUrl, std::string& outputFilePath )
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

}

