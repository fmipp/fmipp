// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file PathFromUrl.cpp

#include "import/base/include/PathFromUrl.h"

#if defined( WIN32 )
#include "windows.h"
#include "shlwapi.h"
#include "tchar.h"
#endif

namespace PathFromUrl {

	bool
	getPathFromUrl( const fmippString& inputFileUrl, fmippString& outputFilePath )
	{
#ifdef WIN32
		LPCTSTR fileUrl = inputFileUrl.c_str();
		LPTSTR filePath = new TCHAR[MAX_PATH];
		DWORD filePathSize = inputFileUrl.size() + 1;
		DWORD tmp = 0;
		HRESULT res = PathCreateFromUrl( fileUrl, filePath, &filePathSize, tmp );
		outputFilePath = fmippString( filePath );
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
