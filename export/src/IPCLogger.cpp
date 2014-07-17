/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file IPCLogger.cpp

#include "export/include/IPCLogger.h"
#include "export/include/FMIComponentFrontEndBase.h"


IPCMasterLogger::IPCMasterLogger( FMIComponentFrontEndBase* fe ) : frontend_( fe ) {}


void
IPCMasterLogger::logger( fmiStatus status, const std::string& category, const std::string& msg )
{
	if ( 0 != frontend_ ) frontend_->logger( status, category, msg );
}



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
IPCSlaveLogger::logger( fmiStatus status, const std::string& category, const std::string& msg )
{
	// Only open an output file in case there is something to report.
	if ( 0 == out_ ) out_ = new std::ofstream( fileName_.c_str(), std::ios::out | std::ios::trunc );

	// Write to output file.
	*out_ << "STATUS: " << status << " - CATEGORY: " << category << " - MESSAGE: " << msg << std::endl;
}
