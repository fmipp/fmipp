/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _PF_API_POWERFACTORYRMS_H
#define _PF_API_POWERFACTORYRMS_H

/**
 * \file PowerFactoryRMS.h 
 *
 * \class PowerFactoryRMS PowerFactoryRMS.h 
 * High-level interface for PowerFactory RMS simulation.
 *  
 * \authors Roman Schwalbe, Matthias Stifter, Edmund Widl
 */

// Boost includes.
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// Project includes.
#include "api/include/PowerFactory.h"


namespace pf_api {

class PowerFactoryRMS
{

public:

	typedef PowerFactory::MapStrStr MapStrStr;

	/// Constructor.
	PowerFactoryRMS( PowerFactory *pf );

	/// Compute initial conditions.
	int rmsInc( double startTime, double timeStep, double realTimeFactor );
	
	/// Carry out an RMS simulation step.
	int rmsSim( double stopTime, bool blocking );
	
	/// Stop RMS simulation.
	int rmsStop( bool blocking );
	
	/// Send an event to PowerFactory.
	int rmsSendEvent( const std::string& name,
		const std::string& type,
		const std::string& target,
		const std::string& event,
		bool blocking,
		bool& isDuplicate );

	/// Check if RMS simulation is running.
	bool rmsIsActive();
	
	/// Reset RMS simulation.
	int clearCachedObjects();

private:

	/// Pointer to PowerFactorh high-level interface.
	PowerFactory* pf_;

	/// PF object related to calculating initial conditions.
	DataObject* inc_;

	/// PF object related to running simulations.
	DataObject* sim_;

	/// Flag indicationg the simulations status (running/not running).
	bool isActive_;

	/// List of previous commands sent to PowerFactory.
	std::map<std::string,MapStrStr> lastCommands_;
	
	boost::shared_ptr<boost::thread> rmsSimThread_;
	boost::mutex rmsSimStart_;

	void rmsSimRun();
	bool sameAsLastEvent(const std::string eventString);
	bool setSimActive();

};

} // namespace pf_api

#endif _PF_API_POWERFACTORYRMS_H