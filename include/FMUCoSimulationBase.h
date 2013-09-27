/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


/**
 * \file FMUCoSimulationBase.h
 * \class FMUCoSimulationBase FMUCoSimulationBase.h  Abstract base class for wrappers handling FMUs.
 *
 * This class includes the main functions that need to be implemented by any class used for numerical integration of FMUs.
 * Typical operations are instantiation and initialization of FMUs, processing of intermidiate results,
 * numerical integration and event handlings
 *
 * \todo Do we really need all these virtual functions within FMUCoSimulationBase? Already some functions present in FMU are not listed here
 **/


#ifndef _FMIPP_FMUCOSIMULATIONBASE_H
#define _FMIPP_FMUCOSIMULATIONBASE_H

#include <string>

#include "FMIPPConfig.h"
#include "fmi_cs.h"



class __FMI_DLL FMUCoSimulationBase
{

public:


	/// Destructor.
        virtual ~FMUCoSimulationBase() {}

	/// Instantiate the FMU.
	virtual fmiStatus instantiate( const std::string& instanceName,
				       const fmiReal& timeout,
				       const fmiBoolean& visible,
				       const fmiBoolean& interactive,
				       const fmiBoolean& loggingOn ) = 0;

	/// Initialize the FMU.
	virtual fmiStatus initialize( const fmiReal& startTime,
				      const fmiBoolean& stopTimeDefined,
				      const fmiReal& stopTime ) = 0;

	/// Get current time.
	virtual fmiReal getTime() const = 0;

	/// Set single value of type fmiReal, using the value reference.
	virtual fmiStatus setValue( fmiValueReference valref, fmiReal& val ) = 0;

	/// Set single value of type fmiInteger, using the value reference.
	virtual fmiStatus setValue( fmiValueReference valref, fmiInteger& val ) = 0;

	/// Set single value of type fmiBoolean, using the value reference.
	virtual fmiStatus setValue( fmiValueReference valref, fmiBoolean& val ) = 0;

	/// Set single single string value (using internaly type fmiString), using the value reference.
	virtual fmiStatus setValue( fmiValueReference valref, std::string& val ) = 0;

	/// Set values of type fmiReal, using an array of value references. 
	virtual fmiStatus setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) = 0;

	/// Set values of type fmiInteger, using an array of value references. 
	virtual fmiStatus setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) = 0;

	/// Set values of type fmiBoolean, using an array of value references. 
	virtual fmiStatus setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) = 0;

	/// Set values of string (using internaly type fmiString), using an array of value references. 
	virtual fmiStatus setValue( fmiValueReference* valref, std::string* val, std::size_t ival ) = 0;

	/// Set single value of type fmiReal, using the variable name.
	virtual fmiStatus setValue( const std::string& name, fmiReal val ) = 0;

	/// Set single value of type fmiInteger, using the variable name.
	virtual fmiStatus setValue( const std::string& name, fmiInteger val ) = 0;

	/// Set single value of type fmiBoolean, using the variable name.
	virtual fmiStatus setValue( const std::string& name, fmiBoolean val ) = 0;

	/// Set single value of string (using internaly type fmiString), using the variable name.
	virtual fmiStatus setValue( const std::string& name, std::string val ) = 0;

	/// Get single value of type fmiReal, using the value reference.
	virtual fmiStatus getValue( fmiValueReference valref, fmiReal& val ) const = 0;

	/// Get single value of type fmiInteger, using the value reference.
	virtual fmiStatus getValue( fmiValueReference valref, fmiInteger& val ) const = 0;

	/// Get single value of type fmiBoolean, using the value reference.
	virtual fmiStatus getValue( fmiValueReference valref, fmiBoolean& val ) const = 0;

	/// Get single value of string (using internaly type fmiString), using the value reference.
	virtual fmiStatus getValue( fmiValueReference valref, std::string& val ) const = 0;

	/// Get values of type fmiReal, using an array of value references. 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const = 0;

	/// Get values of type fmiInteger, using an array of value references. 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) const = 0;

	/// Get values of type fmiBoolean, using an array of value references. 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) const = 0;

	/// Get values of strings (using internaly type fmiString), using an array of value references. 
	virtual fmiStatus getValue( fmiValueReference* valref, std::string* val, std::size_t ival ) const = 0;

	/// Get single value of type fmiReal, using the variable name.
	virtual fmiStatus getValue( const std::string& name, fmiReal& val ) const = 0;

	/// Get single value of type fmiInteger, using the variable name.
	virtual fmiStatus getValue( const std::string& name, fmiInteger& val ) const = 0;

	/// Get single value of type fmiBoolean, using the variable name.
	virtual fmiStatus getValue( const std::string& name, fmiBoolean& val ) const = 0;

	/// Get single value of string (using internaly type fmiString), using the variable name.
	virtual fmiStatus getValue( const std::string& name, std::string& val ) const = 0;


	/// Get value reference associated to variable name.
	virtual fmiValueReference getValueRef( const std::string& name ) const = 0;

	/// Call doStep(...) function of CS FMU.
	virtual fmiStatus doStep( fmiReal currentCommunicationPoint,
				  fmiReal communicationStepSize,
				  fmiBoolean newStep ) const = 0;

};


#endif // _FMIPP_FMUCOSIMULATIONBASE_H

/**
 *
 * \fn 	fmiStatus FMUCoSimulationBase::instantiate( const std::string& instanceName, const fmiReal& timeout, const fmiBoolean& visible, const fmiBoolean& interactive, const fmiBoolean& loggingOn )
 * @param[in]  instanceName  Name of the fmi instance.
 * @param[in]  timeout Communication timeout value in milli-seconds to allow inter-process communication to take place. A timeout value of 0 indicates an infinite wait period.
 * @param[in]  visible Indicates whether or not the simulator application window needed to execute a model should be visible.
 * @param[in]  interactive Indicates whether the simulator application must be manually started by the user.
 * @param[in]  loggingOn Set verbosity of logger.
 * @return the instantiation status
 */

/**
 * \fn 	virtual fmiStatus initialize( const fmiReal& startTime, const fmiBoolean& stopTimeDefined, const fmiReal& stopTime ) = 0;
 * Initialize the CS FMU model and informs the slave that the simulation run starts now.
 * @return initilization status.
 */
