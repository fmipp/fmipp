/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


/**
 * \file BareFMU.h
 * 
 * Definition of "bare" FMUs, i.e., structures that hold pointers
 * to the loaded shared library and callback functions as well as 
 * the parsed XML model description. 
 * 
 */ 

#ifndef _FMIPP_BAREFMU_H
#define _FMIPP_BAREFMU_H

#include "common/FMIPPConfig.h"
#include "common/fmi_v1.0/fmi_me.h"
#include "common/fmi_v1.0/fmi_cs.h"
#include "common/fmi_v2.0/fmi_2.h"


class ModelDescription;


struct BareFMUModelExchange { /// FMI ME 1.0
	me::FMUModelExchange_functions* functions;
	me::fmiCallbackFunctions* callbacks;
	ModelDescription* description;
	
	/// Destructor.
	~BareFMUModelExchange();
};


struct BareFMUCoSimulation { /// FMI CS 1.0
	cs::FMUCoSimulation_functions* functions;
	cs::fmiCallbackFunctions* callbacks;
	ModelDescription* description;
	
	/// Destructor.
	~BareFMUCoSimulation();
};


struct BareFMU2 { /// FMI 2.0, ME & CS
	fmi2::FMU2_functions* functions;
	fmi2::fmi2CallbackFunctions* callbacks;
	ModelDescription* description;

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