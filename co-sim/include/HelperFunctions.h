/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <vector>
#include <string>


#ifdef WIN32
#include "Windows.h"
#endif


namespace HelperFunctions
{

	void splitAndTrim( const std::string& input,
			   std::vector<std::string>& result,
			   const std::string& seperators );


#ifdef WIN32
	TCHAR* copyStringToTCHAR( const std::string& str,
				  size_t extra_length = 0 );
#endif

}


#endif // HELPER_FUNCTIONS_H
