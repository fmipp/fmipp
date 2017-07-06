/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file SimpleLogger.cpp
/// \authors Roman Schwalbe, Matthias Stifter, Edmund Widl

#include <iostream>
#include <iomanip>
#include <ctime>

#include "SimpleLogger.h"


using namespace pf_api;


boost::scoped_ptr<SimpleLogger> SimpleLogger::logger_( new SimpleLogger() );


SimpleLogger& SimpleLogger::getStaticLogger()
{	
	return *SimpleLogger::logger_;
}


std::ostream& SimpleLogger::log( LogLevel l )
{
	if( ( log_level_ >= 0 && l >= log_level_ ) || l == -log_level_ )
	{
#ifdef _WIN32
		time_t rawtime;
		time ( &rawtime );
		struct tm t;
		localtime_s( &t, &rawtime );
		std::stringstream ss;
		ss << t.tm_year + 1900 << "-"
		   << std::setfill('0') << std::setw(2) << t.tm_mon + 1 << "-"
		   << std::setfill('0') << std::setw(2) << t.tm_mday << " "
		   << std::setfill('0') << std::setw(2) << t.tm_hour << ":"
		   << std::setfill('0') << std::setw(2) << t.tm_min << ":"
		   << std::setfill('0') << std::setw(2) << t.tm_sec << " ";
#endif
		if( fs_.is_open() && fs_.good() )
		{
			fs_ << ss.str();
			return fs_;
		}
		else
		{
			std::cout << ss.str();
			return std::cout;
		}
	}
	else
	{
		default_ = std::stringstream();
		return default_;
	}
}


SimpleLogger::SimpleLogger() : log_level_( INF ) {}


SimpleLogger::SimpleLogger( LogLevel log_level_param ) : log_level_( log_level_param ) {}


SimpleLogger::SimpleLogger( LogLevel log_level_param, const std::string &fileName ) :
	log_level_( log_level_param ),
	fs_( fileName.c_str(), std::ios::out )
{}


SimpleLogger::~SimpleLogger()
{
	if( fs_.is_open() )
	{
		fs_.flush();
		fs_.close();
	}
}


void SimpleLogger::logToFile( const std::string &fileName )
{
	if( fs_.is_open() )
	{
		fs_.flush();
		fs_.close();
	}
	fs_.open( fileName.c_str(), std::ios::out );
}


void SimpleLogger::setLogLevel( LogLevel log_level_param )
{
	log_level_ = log_level_param;
}
