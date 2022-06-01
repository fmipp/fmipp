// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file LogBuffer.cpp
 * Provide a global buffer instance for all FMU callback loggers.
 */

#include <cstdio>

#include "import/base/include/LogBuffer.h"

using namespace std;

LogBuffer* LogBuffer::logBuffer_ = 0;

LogBuffer::~LogBuffer()
{
	if ( false == buffer_.empty() )
	{
		fprintf( stderr, "%s", buffer_.c_str() );
		fflush( stderr );
	}
}

/// Default constructor. Private so that it can  not be called.
LogBuffer::LogBuffer() :
	isActivated_( false )
{}

/// Copy constructor. Private so that it can  not be called.
LogBuffer::LogBuffer( LogBuffer const& logBuffer ) :
	isActivated_( logBuffer.isActivated_ ),
	buffer_( logBuffer.buffer_ )
{}

/// Assignment operator. Private so that it can  not be called.
LogBuffer&
LogBuffer::operator=( LogBuffer const& )
{
	return *logBuffer_;
};

LogBuffer& 
LogBuffer::getLogBuffer()
{
	// Singleton instance
	static LogBuffer logBufferInstance;
	if ( 0 == logBuffer_ ) {
		logBuffer_ = &logBufferInstance;
	}
	return *logBuffer_;
}

void
LogBuffer::writeToBuffer( const fmippString& msg )
{
	logBuffer_->buffer_ += msg;
}

fmippString
LogBuffer::readFromBuffer()
{
	return logBuffer_->buffer_;
}

void
LogBuffer::clear()
{
	logBuffer_->buffer_.clear();
}

void
LogBuffer::activate()
{
	logBuffer_->isActivated_ = true;
}

void
LogBuffer::deactivate()
{
	logBuffer_->isActivated_ = false;
}

bool
LogBuffer::isActivated()
{
	return logBuffer_->isActivated_;
}
