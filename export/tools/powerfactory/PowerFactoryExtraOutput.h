/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _POWER_FACTORY_EXTRA_OUTPUT_H
#define _POWER_FACTORY_EXTRA_OUTPUT_H


#include <vector>
#include <utility>
#include <fstream>

#include "common/fmi_v1.0/fmi_cs.h"


namespace pf_api { class PowerFactory; }
class PowerFactoryRealScalar;


/**
 * \file PowerFactoryExtraOutput.h
 * \class PowerFactoryExtraOutput PowerFactoryExtraOutput.h
 * This class allows to write additional simulation data to csv files.
 */ 


class PowerFactoryExtraOutput
{

public:

	typedef std::vector<const PowerFactoryRealScalar*> ExtraOutputSet;
	typedef std::pair<std::ofstream*, ExtraOutputSet*> ExtraOutput;
	typedef std::vector<ExtraOutput> ExtraOutputList;

	/// Constructor (FMI 1.0 backward compatibility).
	PowerFactoryExtraOutput( cs::fmiCallbackFunctions* functions ) :
		fmiFunctions_( functions ), fmi2Functions_( 0 ) {}

	/// Constructor (FMI 2.0).
	PowerFactoryExtraOutput( fmi2::fmi2CallbackFunctions* functions ) :
		fmiFunctions_( 0 ), fmi2Functions_( functions ) {}

		/// Destructor.
	~PowerFactoryExtraOutput();

	/// Initialize outputs streams and lists of scalar variables for extra output.
	bool initializeExtraOutput( pf_api::PowerFactory* pf );

	/// Initialize outputs streams and lists of scalar variables for extra output.
	bool writeExtraOutput( const fmi2Real currentSyncPoint,
			       pf_api::PowerFactory* pf );

private:

	/// Map with output streams and scalar variables for saving extra simulation results at each step.
	ExtraOutputList extraOutputs_;

	/// Internal pointer to callback functions (FMI 1.0, backward compatibility).
	cs::fmiCallbackFunctions* fmiFunctions_;

	/// Internal pointer to callback functions (FMI 2.0).
	fmi2::fmi2CallbackFunctions* fmi2Functions_;

	/// Flag indicating that debug logging is enabled.
	fmi2Boolean loggingOn_;
	
	/// Standard precision for writing values to the CSV file.
	static const unsigned int precision_ = 6;

	/// Send a message to FMU logger.
	void logger( fmi2Status status,
		     const std::string& category,
		     const std::string& msg );

};


#endif //_POWER_FACTORY_EXTRA_OUTPUT_H
