/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_IPCSLAVELOGGER_H
#define _FMIPP_IPCSLAVELOGGER_H


#include <fstream>

#include "export/include/IPCLogger.h"


/**
 * \class IPCSlaveLogger IPCLogger.h
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
	virtual void logger( fmiStatus status, const std::string& category, const std::string& msg );

private:

	/// Default constructor is private to prevent usage.
	IPCSlaveLogger();

	const std::string fileName_;
	std::ofstream* out_;

};



#endif // _FMIPP_IPCSLAVELOGGER_H
