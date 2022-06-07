// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_IPCLOGGER_H
#define _FMIPP_IPCLOGGER_H

/**
 * \file IPCLogger.h
 * Define loggers to be used by the components responsible for IPC.
 */ 

#include "common/FMIPPConfig.h"

/**
 * \class IPCLogger IPCLogger.h
 * Abstract base class for loggers to be used by the components responsible for IPC.
 */ 
class IPCLogger
{

public:

	/// Destructor.
	virtual ~IPCLogger();

	/// Call logger.
	virtual void logger( fmippStatus status, const std::string& category, const std::string& msg ) = 0;

};


#endif // _FMIPP_IPCLOGGER_H
