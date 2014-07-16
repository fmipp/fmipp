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
#include "Windows.h"
#endif


/// Helper functions for string manipulation.
namespace HelperFunctions
{

	void splitAndTrim( const std::string& input,
			   std::vector<std::string>& result,
			   const std::string& seperators );


#ifdef WIN32
	TCHAR* copyStringToTCHAR( const std::string& str,
				  size_t extra_length = 0 );
#endif

	std::string getPathFromUrl( const std::string& inputFileUrl );

}


#endif // _FMIPP_HELPERFUNCTIONS_H
