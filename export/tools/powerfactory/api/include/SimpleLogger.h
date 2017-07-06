#ifndef _PF_API_SIMPLELOGGER_H
#define _PF_API_SIMPLELOGGER_H

/**
 * \file SimpleLogger.h 
 *
 * \class SimpleLogger SimpleLogger.h 
 * Simple logger for PowerFactory high-level API.
 *  
 * \authors Roman Schwalbe, Matthias Stifter, Edmund Widl
 */

#include <sstream>
#include <fstream>
#include <boost/scoped_ptr.hpp>


namespace pf_api {


class SimpleLogger
{	

public:

	enum LogLevel {
		ERR_ONLY = -5,
		WRN_ONLY = -4,
		INF_ONLY = -3,
		DBG_ONLY = -2,
		TRC_ONLY = -1,
		ALL = 0,
		TRC = 1,
		DBG = 2,
		INF = 3,
		WRN = 4,
		ERR = 5,
		FTL = 6,
		OFF = 7
		};

	std::ostream& log( LogLevel l );

	SimpleLogger();
	SimpleLogger( LogLevel log_level_param );
	SimpleLogger( LogLevel log_level_param,const std::string &fileName );
	~SimpleLogger();

	void logToFile( const std::string &fileName );
	void setLogLevel( LogLevel log_level_param );
	static SimpleLogger& getStaticLogger();

protected:

	static boost::scoped_ptr<SimpleLogger> logger_;
	LogLevel log_level_;
	std::ofstream fs_;
	std::stringstream default_;
};


#define LOG_TRACE SimpleLogger::getStaticLogger().log( SimpleLogger::TRC )
#define LOG_DEBUG SimpleLogger::getStaticLogger().log( SimpleLogger::DBG )
#define LOG_INFO SimpleLogger::getStaticLogger().log( SimpleLogger::INF )
#define LOG_WARNING SimpleLogger::getStaticLogger().log( SimpleLogger::WRN )
#define LOG_ERROR SimpleLogger::getStaticLogger().log( SimpleLogger::ERR )
#define LOG_FATAL SimpleLogger::getStaticLogger().log( SimpleLogger::FTL )

} // namespace pf_api

#endif // _PF_API_SIMPLELOGGER_H

