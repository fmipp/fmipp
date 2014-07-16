/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_IPCLOGGER_H
#define _FMIPP_IPCLOGGER_H

/**
 * \file IPCLogger.h
 * Define loggers to be used by the components responsible for IPC.
 */ 


#include <string>
#include <fstream>

#include "common/fmi_v1.0/fmiModelTypes.h"

class FMIComponentFrontEndBase;


/**
 * \class IPCLogger IPCLogger.h
 * Abstract base class for loggers to be used by the components responsible for IPC.
 */ 
class IPCLogger
{

public:

	/// Call logger.
	virtual void logger( fmiStatus status, const std::string& category, const std::string& msg ) = 0;

};


/**
 * \class IPCMasterLogger IPCLogger.h
 * Logger to be used by the IPCMaster (and its components). Same output as calling FMU.
 */ 
class IPCMasterLogger : public IPCLogger
{

public:

	/// Constructor.
	IPCMasterLogger( FMIComponentFrontEndBase* fe );

	/// Call FMU logger.
	virtual void logger( fmiStatus status, const std::string& category, const std::string& msg );

private:

	/// Default constructor is private to prevent usage.
	IPCMasterLogger() : frontend_( 0 ) {}

	/// Pointer to the FMUs front end component.
	FMIComponentFrontEndBase* frontend_;

};


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
	~IPCSlaveLogger();

	/// Call logger.
	virtual void logger( fmiStatus status, const std::string& category, const std::string& msg );

private:

	/// Default constructor is private to prevent usage.
	IPCSlaveLogger();

	const std::string fileName_;
	std::ofstream* out_;

};



#endif // _FMIPP_IPCLOGGER_H
