/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <vector>
#include <string>

#include "Windows.h"


namespace HelperFunctions
{

	void splitAndTrim( const std::string& input,
			   std::vector<std::string>& result,
			   const std::string& seperators );


	TCHAR* copyStringToTCHAR( const std::string& str,
				  size_t extra_length = 0 );
}


#endif // HELPER_FUNCTIONS_H
