// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/// \file IPCMasterLogger.cpp

#include "export/include/IPCMasterLogger.h"
#include "export/include/FMIComponentFrontEndBase.h"


IPCMasterLogger::IPCMasterLogger( FMIComponentFrontEndBase* fe ) : frontend_( fe ) {}


IPCMasterLogger::~IPCMasterLogger() {}


void
IPCMasterLogger::logger( fmippStatus status, const std::string& category, const std::string& msg )
{
	if ( 0 != frontend_ ) frontend_->logger( status, category, msg );
}
