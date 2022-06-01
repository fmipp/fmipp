// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file BareFMU.cpp
 */

#include "import/base/include/BareFMU.h"
#include "import/base/include/ModelDescription.h"

// Helper function for deleting bare FMUs.
template<typename BareFMUType> void deleteBareFMUContent( BareFMUType* bareFMU )
{
		if ( 0 != bareFMU->functions->dllHandle ) {
#if defined(MINGW)
			FreeLibrary( static_cast<HMODULE>( bareFMU->functions->dllHandle ) );
#elif defined(_MSC_VER)
			FreeLibrary( static_cast<HMODULE>( bareFMU->functions->dllHandle ) );
#else
			dlclose( bareFMU->functions->dllHandle );
#endif
		}

		if ( 0 != bareFMU->functions ) delete bareFMU->functions;
		if ( 0 != bareFMU->description ) delete bareFMU->description;
}

BareFMUModelExchange::~BareFMUModelExchange()
{
	deleteBareFMUContent( this );
}

BareFMUCoSimulation::~BareFMUCoSimulation()
{
	deleteBareFMUContent( this );
}

BareFMU2::~BareFMU2()
{
	deleteBareFMUContent( this );
}
