#ifndef _PF_API_POWERFACTORYLOGGERBASE_H
#define _PF_API_POWERFACTORYLOGGERBASE_H

/**
 * \file PowerFactoryLoggerBase.h 
 *
 * \class PowerFactoryLoggerBase PowerFactoryLoggerBase.h 
 * Simple logger base class for PowerFactory high-level API.
 *  
 * \authors Edmund Widl
 */

#include <string>


namespace pf_api {


class PowerFactoryLoggerBase
{	

public:

	enum LogLevel {
		OK,
		Warning,
		Error
	};

	virtual void logger( const LogLevel& l, const std::string& category, const std::string& msg ) = 0;
	
};

} // namespace pf_api

#endif // _PF_API_POWERFACTORYLOGGERBASE_H
