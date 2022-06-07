// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_IPMASTERCLOGGER_H
#define _FMIPP_IPMASTERCLOGGER_H

/**
 * \file IPCMasterLogger.h
 * Define logger to be used by the front end components responsible for IPC.
 */ 


#include "export/include/IPCLogger.h"

class FMIComponentFrontEndBase;


/**
 * \class IPCMasterLogger IPCMasterLogger.h
 * Logger to be used by the IPCMaster (and its components). Same output as calling FMU.
 */ 
class IPCMasterLogger : public IPCLogger
{

public:

	/// Constructor.
	IPCMasterLogger( FMIComponentFrontEndBase* fe );

	/// Destructor.
	virtual ~IPCMasterLogger();

	/// Call FMU logger.
	virtual void logger( fmippStatus status, const std::string& category, const std::string& msg );

private:

	/// Default constructor is private to prevent usage.
	IPCMasterLogger() : frontend_( 0 ) {}

	/// Pointer to the FMUs front end component.
	FMIComponentFrontEndBase* frontend_;

};



#endif // _FMIPP_IPCMASTERLOGGER_H
