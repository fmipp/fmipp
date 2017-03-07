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


boost::scoped_ptr<SimpleLogger> SimpleLogger::logger( new SimpleLogger() );


SimpleLogger& SimpleLogger::getStaticLogger()
{	
	return *SimpleLogger::logger;
}


std::ostream& SimpleLogger::log( LogLevel l )
{
	if( ( log_level >= 0 && l >= log_level ) || l == -log_level )
	{
#ifdef _WIN32
		time_t rawtime;
		time ( &rawtime );
		struct tm t;
		localtime_s(&t,&rawtime );
		std::stringstream ss;
		ss << t.tm_year + 1900 << "-"
		   << std::setfill('0') << std::setw(2) << t.tm_mon + 1 << "-"
		   << std::setfill('0') << std::setw(2) << t.tm_mday << " "
		   << std::setfill('0') << std::setw(2) << t.tm_hour << ":"
		   << std::setfill('0') << std::setw(2) << t.tm_min << ":"
		   << std::setfill('0') << std::setw(2) << t.tm_sec << "\t";
#endif
		if( fs.is_open() && fs.good() )
		{
			fs << ss.str();
			return fs;
		}
		else
		{
			std::cout << ss.str();
			return std::cout;
		}
	}
	else
	{
		rubbish = std::stringstream();
		return rubbish;
	}
}


SimpleLogger::SimpleLogger() : log_level( DBG ) {}


SimpleLogger::SimpleLogger( LogLevel log_level_param ) : log_level( log_level_param ) {}


SimpleLogger::SimpleLogger( LogLevel log_level_param,const std::string &fileName ) :
	log_level(log_level_param),
	fs(fileName.c_str(),std::ios::out)
{}


SimpleLogger::~SimpleLogger()
{
	if( fs.is_open() )
	{
		fs.flush();
		fs.close();
	}
}


void SimpleLogger::logToFile( const std::string &fileName )
{
	if(fs.is_open())
	{
		fs.flush();
		fs.close();
	}
	fs.open( fileName.c_str(), std::ios::out );
}


void SimpleLogger::setLogLevel( LogLevel log_level_param )
{
	log_level=log_level_param;
}
