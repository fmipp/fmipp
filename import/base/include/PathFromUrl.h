// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file PathFromUrl.h

#ifndef _FMIPP_PATHFROMURL_H
#define _FMIPP_PATHFROMURL_H

#include "common/FMIPPConfig.h"

/// Namespace contains helper function to convert URLs to system paths.
namespace PathFromUrl
{

	/// Helper function for transforming URLs to a system path.
	fmippBoolean getPathFromUrl( const fmippString& inputFileUrl, fmippString& outputFilePath );

}

#endif // _FMIPP_PATHFROMURL_H
