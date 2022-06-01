// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file BareFMU.h
 * 
 * Definition of "bare" FMUs, i.e., structures that hold pointers
 * to the loaded shared library and the parsed XML model description. 
 * 
 */ 

#ifndef _FMIPP_BAREFMU_H
#define _FMIPP_BAREFMU_H

#include "common/FMIPPConfig.h"
#include "common/FMUType.h"
#include "common/fmi_v1.0/fmi_me.h"
#include "common/fmi_v1.0/fmi_cs.h"
#include "common/fmi_v2.0/fmi_2.h"

class ModelDescription;

/// FMI ME 1.0
struct BareFMUModelExchange
{
	/// Struct for FMI-compliant functions.
	me::FMUModelExchange_functions* functions;

	/// Parsed XML model description.
	ModelDescription* description;
	
	/// Destructor.
	~BareFMUModelExchange();
};

/// FMI CS 1.0
struct BareFMUCoSimulation
{
	/// Struct for FMI-compliant functions.
	cs::FMUCoSimulation_functions* functions;

	/// Parsed XML model description.
	ModelDescription* description;

	/// URI to (unzipped) FMU archive.
	std::string fmuLocation;
	
	/// Destructor.
	~BareFMUCoSimulation();
};

/// FMI 2.0, ME & CS
struct BareFMU2
{
	/// Struct for FMI-compliant functions.
	fmi2::FMU2_functions* functions;

	/// Parsed XML model description.
	ModelDescription* description;

	/// URI to FMU resources directory.
	std::string fmuResourceLocation;

	/// Destructor.
	~BareFMU2();
};

//
// Define smart pointers to bare FMUs.
//

#include <memory>

typedef std::shared_ptr<BareFMUModelExchange> BareFMUModelExchangePtr;
typedef std::shared_ptr<BareFMUCoSimulation> BareFMUCoSimulationPtr;
typedef std::shared_ptr<BareFMU2> BareFMU2Ptr;

#endif // _FMIPP_BAREFMU_H
