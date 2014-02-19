/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


/** 
 * \file FMUBase.h
 * \class FMUBase FMUBase.h  Abstract base class for wrappers handling FMUs.
 * 
 * This class includes the main functions that need to be implemented by any class used for numerical integration of FMUs.
 * Typical operations are instantiation and initialization of FMUs, processing of intermidiate results, 
 * numerical integration and event handlings     
 * 
 * \todo Do we really need all these virtual functions within FMUBase? Already some functions present in FMU are not listed here 
 **/


#ifndef _FMIPP_FMUBASE_H
#define _FMIPP_FMUBASE_H

#include <string>

#include "FMIPPConfig.h"
#include "FMIType.h"
#include "fmi_me.h"



class __FMI_DLL FMUBase
{

public:

        
        virtual ~FMUBase() {} ///< Destructor.

	virtual FMIType getType( const std::string& variableName ) const = 0;

	virtual fmiStatus instantiate( const std::string& instanceName, fmiBoolean loggingOn ) = 0; ///< Instantiate the FMU.
	virtual fmiStatus initialize() = 0; ///< Initialize the FMU. 

	virtual fmiReal getTime() const = 0;                    ///< Get current time.
	virtual void    setTime( fmiReal time ) = 0;            ///< Set current time. 
	virtual void rewindTime( fmiReal deltaRewindTime ) = 0; ///< Rewind current time. 

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

	
	virtual fmiValueReference getValueRef( const std::string& name ) const = 0;  ///< Get value reference associated to variable name.

	virtual fmiStatus getContinuousStates( fmiReal* val ) const = 0;   ///< Get continuous states.
	virtual fmiStatus setContinuousStates( const fmiReal* val ) = 0;   ///< Set continuous states.
	virtual fmiStatus getDerivatives( fmiReal* val ) const = 0;        ///< Get derivatives.

	virtual fmiStatus getEventIndicators( fmiReal* eventsind ) const = 0; ///< Get event indicators.

	virtual fmiReal integrate( fmiReal tend, 
				   unsigned int nsteps ) = 0;   ///< Integrate internal state. 
	virtual fmiReal integrate( fmiReal tend, 
				   double deltaT ) = 0;         ///< Integrate internal state. 

	virtual void raiseEvent() = 0;                         ///< Raise an event.
	virtual fmiBoolean checkStateEvent() = 0;         ///< Check if a state event happened.
	virtual void handleEvents( fmiTime tstop ) = 0; ///< Handle events.

	virtual fmiStatus completedIntegratorStep() = 0; ///< Complete an integration step

	virtual fmiBoolean getEventFlag() = 0;   ///< Get event flag.

	virtual fmiBoolean getIntEvent() = 0; ///< The integrator needs to check for events that happened during the integration.

	virtual std::size_t nStates() const = 0;      ///< Get number of continuous states. 
	virtual std::size_t nEventInds() const = 0;   ///< Get number of event indicators.
	virtual std::size_t nValueRefs() const = 0;   ///< Get number of value references (equals the numer of variables).
};


#endif // _FMIPP_FMUBASE_H

/**
 * 
 * \fn 	fmiStatus FMUBase::instantiate( const std::string& instanceName, fmiBoolean loggingOn )
 * @param[in]  instanceName  name of the fmi instance 
 * @param[in]  loggingOn 
 * @return the instantation status 
 */
 
/**
 * \fn FMUBase::initialize
 * Initializing the FMU model at time t_0 (after model parameters and start values have been set)
 * @return initilization status.
 */

/** 
 * \fn FMUBase::setTime
 * Set current time affects only the value of the internal FMU time, not the internal state.
 * @param[in] time new time point to be set 
 */

/** 
 * \fn FMUBase::rewindTime
 * This affects only the value of the internal FMU time, not the internal state. 
 * @param[in] deltaRewindTime amount of time to be rewinded 
 */

/** 
 * \fn virtual fmiStatus FMUBase::setValue( fmiValueReference valref, fmiReal& val ) = 0;
 * @param[in]  valref   reference of real variable to be set 
 * @param[in]  val      value to be set
 * @return setting  value status  
 **/

/** 
 * \fn virtual fmiStatus FMUBase::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) = 0; 
 * @param[in] valref  value references of array of integer variable to be set 
 * @param[in] val     corresponding values to be set   
 * @param[in] ival    number of items in the array 
 * @return        ???   
 **/

 
/**  	 
 * \fn fmistatus FMUBase::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival )
 *
 * @param[in]  valref  value references of array of integer variable to be set 
 * @param[in]  val     corresponding values to be set   
 * @param[in]  ival    number of items in the array 
 * @return         ???
 */ 


/**
 * \fn virtual fmiStatus FMUBase::getValue( const std::string& name,  fmiReal& val ) const = 0;
 * @param[in] name name of the variable  
 * @param[in] current val of the variablet 
 * @return status of getting a value with the given name 
 */
