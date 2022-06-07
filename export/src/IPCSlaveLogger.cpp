// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file IPCSlaveLogger.cpp

#include <boost/filesystem.hpp>

#include "export/include/IPCSlaveLogger.h"


IPCSlaveLogger::IPCSlaveLogger() :
	fileName_( "debug.log" ),
	out_( 0 )
{}


IPCSlaveLogger::IPCSlaveLogger( const std::string& fileName ) :
	fileName_( fileName ),
	out_( 0 )
{}


IPCSlaveLogger::~IPCSlaveLogger()
{
	// Only delete file stream in case it was really used.
	if ( 0 != out_ ) delete out_;
}


void
IPCSlaveLogger::logger( fmippStatus status, const std::string& category, const std::string& msg )
{
	// Only open an output file in case there is something to report.
	if ( 0 == out_ ) out_ = new std::ofstream( fileName_.c_str(), std::ios::out | std::ios::trunc );

	// Write to output file.
	*out_ << "STATUS: " << status << " - CATEGORY: " << category << " - MESSAGE: " << msg << std::endl;
}


std::string
IPCSlaveLogger::getLogFileName() const
{
	using namespace boost::filesystem;

	// Use Boost tools for file manipulation.
	path logFile( fileName_ );
	if ( is_regular_file( logFile ) ) { // Check if regular file.
		// Copy to working directory.
		path fullLogFileName = current_path() /= logFile.filename();
		return fullLogFileName.string();
	}
	
	std::string err = std::string( "ERROR - no log file with this name has been found: " ) + fileName_;
	return err;
}