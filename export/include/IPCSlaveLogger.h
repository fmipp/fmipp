// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_IPCSLAVELOGGER_H
#define _FMIPP_IPCSLAVELOGGER_H


/**
 * \file IPCSlaveLogger.h
 * Define logger to be used by the back end components responsible for IPC.
 */

#include <fstream>

#include "export/include/IPCLogger.h"


/**
 * \class IPCSlaveLogger IPSlaveCLogger.h
 * Logger to be used by the IPCSlave (and its components). Writes log messages to file.
 */ 
class IPCSlaveLogger : public IPCLogger
{

public:

	/// Constructor.
	IPCSlaveLogger( const std::string& fileName );

	/// Destructor.
	virtual ~IPCSlaveLogger();

	/// Call logger.
	virtual void logger( fmippStatus status, const std::string& category, const std::string& msg );

	/// Get full path of log messages file.
	std::string getLogFileName() const;
	
private:

	/// Default constructor is private to prevent usage.
	IPCSlaveLogger();

	const std::string fileName_;
	std::ofstream* out_;

};



#endif // _FMIPP_IPCSLAVELOGGER_H
