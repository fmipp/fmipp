/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file HelperFunctions.h


#ifndef _FMIPP_HELPERFUNCTIONS_H
#define _FMIPP_HELPERFUNCTIONS_H

#include <vector>
#include <string>


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

}


#endif // _FMIPP_HELPERFUNCTIONS_H
