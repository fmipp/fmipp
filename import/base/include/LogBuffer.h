// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file LogBuffer.h
 * Provide a global buffer instance for all FMU callback loggers.
 */

#include <string>

#include "common/FMIPPConfig.h"


class __FMI_DLL LogBuffer
{

public:

	/// Destructor.        
	~LogBuffer();

	/// Get singleton instance of log buffer.
	static LogBuffer& getLogBuffer();

	/// Write a log message to the buffer.
	void writeToBuffer( const fmippString& msg );

	/// Read the buffer's content.
	fmippString readFromBuffer();

	/// Clear the buffer's content.
	void clear();

	/// Activate global buffering.
	void activate();

	/// Deactivate global buffering.
	void deactivate();

	/// Check if global buffering is activated.
	bool isActivated();

private:

	/// Default constructor. Private so that it can  not be called.
	LogBuffer();

	/// Copy constructor. Private so that it can  not be called.
	LogBuffer( LogBuffer const& );

	/// Assignment operator. Private so that it can  not be called.
	LogBuffer& operator=( LogBuffer const& logBuffer );

	/// Singleton instance of the log buffer.
	static LogBuffer* logBuffer_;

	/// Flag indicationg if buffering is on/off.
	bool isActivated_;

	/// The string for buffering log messages.
	fmippString buffer_;
};
