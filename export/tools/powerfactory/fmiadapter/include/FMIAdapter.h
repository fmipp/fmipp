#ifndef _POWER_FACTORY_FMIADAPTER_H
#define _POWER_FACTORY_FMIADAPTER_H

// Project includes.
#include "fmiadapter/include/digusermodel.h"
#include "fmiadapter/include/GetNextEventStatus.h"

// DLL info for DSL model definition
#define DESCRIPTION "FMIAdapter"
#define VERSION "0.1"
#define CREATED "11.07.2017"
#define AUTHOR "Edmund Widl"
#define COMPANY "AIT Austrian Institute of Technology GmbH"
#define COPYRIGHT "AIT Austrian Institute of Technology GmbH"
#define CHECKSUM "3A3F-8C8F-C2E5-5EEF"

// Define size of character arrays used for retrieving events from the queue.
#define LENNAME 30
#define LENTYPE 10
#define LENTARGET 30
#define LENEVT 100

// Helper macros for handling the output signal.
#define ___trigger 0
#define trigger *(pInstance->m_outputSigs[0].m_value)
#define trigger___init(val) *(pInstance->m_outputSigs[0].m_value)=val

#endif // _POWER_FACTORY_FMIADAPTER_H