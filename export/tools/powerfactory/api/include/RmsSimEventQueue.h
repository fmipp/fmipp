#ifndef _PF_API_RMSSIMEVENTQUEUE_H
#define _PF_API_RMSSIMEVENTQUEUE_H

/**
 * \file RmsSimEventQueue.h 
 *
 * \class RmsSimEventQueue RmsSimEventQueue.h 
 * Event queue for comminucation with PowerFactory at 
 * run-time (for sending events during RMS simulation).
 *  
 * \authors Roman Schwalbe, Matthias Stifter, Edmund Widl
 */

#include <string>
 
#include "common/FMIPPConfig.h"


namespace pf_api {

class __FMI_DLL RmsSimEventQueue
{

private:

	static const size_t POWERFACTORY_MAX_LEN;

public:

	static const size_t getMaxLen();
	static std::string getNextEvent();
	static void addEvent( const std::string &eventString );
	static bool isEmpty();
	static size_t size();

};

} // namespace pf_api

#endif // _PF_API_RMSSIMEVENTQUEUE_H
