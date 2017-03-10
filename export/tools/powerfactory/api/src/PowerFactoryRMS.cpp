/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file PowerFactoryRMS.cpp
/// \authors Roman Schwalbe, Matthias Stifter, Edmund Widl

#ifndef _WIN32_WINDOWS // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS _WIN32_WINNT_WIN7 // Target Windows 7 or later.
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif

// Boost includes.
#include <map>
#include <boost/thread.hpp>

// PF API.
#include "v1/Api.hpp"

// Project includes.
#include "PowerFactoryRMS.h"
#include "RmsSimEventQueue.h"
#include "SimpleLogger.h"
#include "Utils.h"


using namespace pf_api;


PowerFactoryRMS::PowerFactoryRMS( PowerFactory *pf ) :
	pf_( pf ),
	inc_( 0 ),
	sim_( 0 ),
	isActive_( false )
{}


int
PowerFactoryRMS::rmsInc( double startTime, double timeStep, double realTimeFactor )
{	
	int result = 0;
	if ( 0 == inc_ ) {
		result = pf_->getActiveStudyCaseObject( "ComInc", "", true, inc_ );
		if( PowerFactory::Ok != result ) return result;
	}

	if ( startTime > -1. ) {
		result = pf_->setAttributeDouble( inc_, "tstart", startTime );
		if( PowerFactory::Ok != result ) return result;
	}

	if ( timeStep > 0. ) {
		result = pf_->setAttributeDouble( inc_, "dtout", timeStep );
		if( PowerFactory::Ok != result ) return result;

		result = pf_->setAttributeDouble( inc_, "dtgrd", timeStep );
		if( PowerFactory::Ok != result ) return result;

		result = pf_->setAttributeDouble( inc_, "rt_inter", timeStep );
		if( PowerFactory::Ok != result ) return result;
	}

	if ( realTimeFactor <= 0. ) {
		result = pf_->setAttributeDouble( inc_, "iopt_real", 0 );
		if( PowerFactory::Ok != result ) return result;
	} else {	
		result = pf_->setAttributeDouble( inc_, "iopt_real", 2 );
		if( PowerFactory::Ok != result ) return result;

		result = pf_->setAttributeDouble( inc_, "rt_factor", realTimeFactor );
		// result = pf_->setAttributeDouble( inc_, "rt_factor", realTimeFactor*100 );
		if( PowerFactory::Ok != result ) return result;
	}

	//result = pf_->executeRCOMcommand( "inc" );
	result = pf_->execute( "inc" );
	if( 0 != result ) {
		LOG_WARNING << "PowerFactoryRMS::rmsInc() could not calculate initial conditions ("
		            << result << ")" << std::endl;
		return result;
	}

	PowerFactory::VecVariant results;
	result = pf_->executeDPL( "DPLrmsValid", PowerFactory::VecVariant(), results );

	if( pf_->NoSuchObject == result ) {
		LOG_WARNING << "PowerFactoryRMS::rmsInc() DPL-Script DPLrmsValid not found - can't guaranty valid RMS!!!" << std::endl;
	} else if ( pf_->PowerFactory::Ok != result ) {
		LOG_WARNING << "PowerFactoryRMS::rmsInc() Error while executing DPLrmsValid" << std::endl;
		return result;
	} else {
		const int* ip = 0;
		if( results.size() != 1 || ( ip = boost::get<int>( &results[0] ) ) == 0 ) {
			LOG_WARNING << "PowerFactoryRMS::rmsInc() Error while executing DPLrmsValid." << std::endl;
			return pf_->UndefinedError;
		}
		
		if( *ip != 1 ) {
			LOG_WARNING << "PowerFactoryRMS::rmsInc() Calculation of Initial Conditions failed. See output messages!" << std::endl;
			return pf_->LDFnotValid;
		}
	}

	return pf_->PowerFactory::Ok;
}


int
PowerFactoryRMS::rmsSim( double stopTime, bool blocking )
{	
	int result = 0;

	if( 0 == sim_ ) {
		result = pf_->getActiveStudyCaseObject( "ComSim", "", true, sim_ );
		if ( PowerFactory::Ok != result ) return result;
	}
	
	if( stopTime > 0. ) {
		result = pf_->setAttributeDouble( sim_, "tstop", stopTime );
		if ( PowerFactory::Ok != result ) return result;
	}

	if( false == setSimActive() ) {
		LOG_DEBUG << "PowerFactoryRMS::rmsSim: Can't start simulation, because it is already running" << std::endl;
		return PowerFactory::LastCommandNotFinished;
	}

	if( true == blocking ) {	
		result = pf_->execute( sim_, "Execute" );
		isActive_ = false;
	} else {
		boost::thread* newSimThread =
			new boost::thread( boost::bind( &PowerFactoryRMS::rmsSimRun, this ) );
		rmsSimThread_ = boost::shared_ptr<boost::thread>( newSimThread );
	}

	return PowerFactory::Ok;
}


int
PowerFactoryRMS::rmsStop( bool blocking )
{	
	if ( false == isActive_ ) return PowerFactory::Ok;

	//pf_->executeRCOMcommand( "stop" );
	pf_->execute( "stop" );

	if ( blocking ) { rmsSimThread_->join(); } // Wait for the thread to finish.
	// else { Sleep( 500 ); } /// \FIXME This delay should be adjustable.

	return PowerFactory::Ok;
}


int
PowerFactoryRMS::rmsSendEvent( const char *eventString, bool blocking )
{
	std::string eventStr( eventString );

	if ( eventStr.size() > RmsSimEventQueue::getMaxLen() ) // Check length of event string.
		return PowerFactory::UndefinedError;

	if ( sameAsLastEvent( eventStr ) ) { // No need to execute same event again.
		LOG_INFO << "PowerFactoryRMS::rmsSendEvent(" << eventStr
		         << ") has not been executed (duplicate)!" << std::endl;
		return PowerFactory::Ok;
	}

	if ( false == RmsSimEventQueue::isEmpty() )
		LOG_WARNING << "PowerFactoryRMS::rmsSendEvent(" << eventStr
	                << "): WARNING: Last command not finished when sending new command to PowerFactory!"
		            << std::endl;

	// Put event string into queue for further processing (done in PowerFactory).
	RmsSimEventQueue::addEvent( eventStr );
	LOG_DEBUG << "PowerFactoryRMS::rmsSendEvent(): \'" << eventStr << "\' was sent." << std::endl;

	/// \FIXME The delay in the following line should be adjustable.
	while ( blocking && ( false == RmsSimEventQueue::isEmpty() ) ) Sleep(1); // Wait until event queue is empty (blocking call).
	
	return PowerFactory::Ok;
}


bool
PowerFactoryRMS::rmsIsActive()
{	
	return isActive_;
}


int
PowerFactoryRMS::clearCachedObjects()
{	
	inc_ = 0;
	sim_ = 0;
	lastCommands_.clear();
	return PowerFactory::Ok;
}


void
PowerFactoryRMS::rmsSimRun()
{	
	//pf_->executeRCOMcommand("sim");
	pf_->execute( "sim" );
	isActive_ = false;
	LOG_DEBUG << "RMS Simulation finished." << std::endl;
}


bool
PowerFactoryRMS::sameAsLastEvent( const std::string eventString )
{
	// Split event string into sub-strings.
	std::vector<std::string> paramStr;
	utils::split( eventString, ' ', paramStr );
	
	// Parse individual sub-strings to retrieve target, variable and value.
	std::string target;
	std::string variable;
	std::string value;
	std::vector<std::string> param;
	for ( size_t i = 0; i < paramStr.size(); ++i ) {
		utils::split( paramStr[i], '=', param );

		if( param.size() < 2 ) return false; // Syntax error.

		if ( param[0].compare( "target" ) == 0 ) {
			target = param[1]; // Parse target attribute.
		} else if ( param[0].compare( "variable" ) == 0 ) {
			variable = param[1]; // Parse variable attribute.
		} else if ( param[0].compare( "value" ) == 0 ) {
			value = param[1]; // Parse value attribute.
		}
	}

	// No variable or value specified.
	if ( variable.empty() || value.empty() ) return false;

	// Retrieve previous value for this combination of target/value.
	std::string lastValue = lastCommands_[target][variable];
	// Set current value for this combination of target/value.
	lastCommands_[target][variable] = value;
	
	// Compare previous and current value.
	return ( lastValue.compare(value) == 0 ) ? true : false;
}


bool
PowerFactoryRMS::setSimActive()
{	
	bool ret = true;
	rmsSimStart_.lock();
	if ( isActive_ ) {
		ret = false;
	} else {
		isActive_ = true;
	}
	rmsSimStart_.unlock();
	return ret;
}
