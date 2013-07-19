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
 **/


#ifndef _FMIPP_FMUBASE_H
#define _FMIPP_FMUBASE_H

#include <string>

#include "FMIPPConfig.h"
#include "fmi_me.h"



class __FMI_DLL FMUBase
{

public:

        
        virtual ~FMUBase() {} ///< Destructor.

	virtual fmiStatus instantiate( const std::string& instanceName, fmiBoolean loggingOn ) = 0; ///< Instantiate the FMU.
	virtual fmiStatus initialize() = 0; ///< Initialize the FMU. 

	virtual fmiReal getTime() const = 0;                    ///< Get current time.
	virtual void    setTime( fmiReal time ) = 0;            ///< Set current time. 
	virtual void rewindTime( fmiReal deltaRewindTime ) = 0; ///< Rewind current time. 

	virtual fmiStatus setValue( fmiValueReference valref, 
				    fmiReal& val ) = 0;         ///< Set single value of type fmiReal, using the value reference.
	virtual fmiStatus setValue( fmiValueReference valref, 
				    fmiInteger& val ) = 0;      ///< Set single value of type fmiInteger, using the value reference.
	virtual fmiStatus setValue( fmiValueReference* 
				    valref, fmiReal* val, 
				    std::size_t ival ) = 0;     ///< Set values of type fmiReal, using an array of value references. 
	virtual fmiStatus setValue( fmiValueReference* valref, 
				    fmiInteger* val, 
				    std::size_t ival ) = 0;     ///< Set values of type fmiInteger, using an array of value references.

	virtual fmiStatus setValue( const std::string& name,  
				    fmiReal val ) = 0;          ///< Set single value of type fmiReal, using the variable name.
	virtual fmiStatus setValue( const std::string& name,  
				    fmiInteger val ) = 0;       ///< Set single value of type fmiInteger, using the variable name.

	virtual fmiStatus getValue( fmiValueReference valref, 
				    fmiReal& val ) const = 0;   ///< Get single value of type fmiReal, using the value reference.
	virtual fmiStatus getValue( fmiValueReference valref, 
				    fmiInteger& val ) const = 0;  ///< Get single value of type fmiInteger, using the value reference.
	virtual fmiStatus getValue( fmiValueReference* valref, 
				    fmiReal* val, 
				    std::size_t ival ) const = 0; ///< Get values of type fmiReal, using an array of value references.
	virtual fmiStatus getValue( fmiValueReference* valref, 
				    fmiInteger* val, 
				    std::size_t ival ) const = 0; ///< Get values of type fmiInteger, using an array of value references.
	
	virtual fmiStatus getValue( const std::string& name,  
				    fmiReal& val ) const = 0;     ///< Get single value of type fmiReal, using the variable name.
	virtual fmiStatus getValue( const std::string& name,  
				    fmiInteger& val ) const = 0;  ///< Get single value of type fmiInteger, using the variable name.
	
	virtual fmiValueReference getValueRef( const std::string& name ) const = 0;  ///< Get value reference associated to variable name.

	virtual fmiStatus getContinuousStates( fmiReal* val ) const = 0;   ///< Get continuous states.
	virtual fmiStatus setContinuousStates( const fmiReal* val ) = 0;   ///< Set continuous states.
	virtual fmiStatus getDerivatives( fmiReal* val ) const = 0;        ///< Get derivatives.

	virtual fmiStatus getEventIndicators( fmiReal* eventsind ) const = 0; ///< Get event indicators.

	virtual fmiReal integrate( fmiReal tend, 
				   unsigned int nsteps ) = 0;   ///< Integrate internal state. 
	virtual fmiReal integrate( fmiReal tend, 
				   double deltaT ) = 0;         ///< Integrate internal state. 

	virtual void raiseEvent() = 0;                         ///< Raise an event. **/
	virtual void handleEvents( fmiTime tstop, 
				   bool completedIntegratorStep ) = 0; ///< Handle events.

	virtual fmiBoolean getStateEventFlag() = 0;   ///< Get state event flag.

	virtual std::size_t nStates() const = 0;      ///< Get number of continuous states. 
	virtual std::size_t nEventInds() const = 0;   ///< Get number of event indicators.
	virtual std::size_t nValueRefs() const = 0;   ///< Get number of value references (equals the numer of variables).
};


#endif // _FMIPP_FMUBASE_H

/**
 * 
 * \fn 	fmiStatus FMUBase::instantiate( const std::string& instanceName, fmiBoolean loggingOn )
 * @param  instanceName  name of the fmi instance 
 * @param  loggingOn 
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
 * @param time new time point to be set 
 */

/** 
 * \fn FMUBase::rewindTime
 * This affects only the value of the internal FMU time, not the internal state. 
 * @param deltaRewindTime amount of time to be rewinded 
 */

/** 
 * \fn virtual fmiStatus FMUBase::setValue( fmiValueReference valref, fmiReal& val ) = 0;
 * @param  valref   reference of real variable to be set 
 * @param  val      value to be set
 * @return setting  value status  
 **/

/** 
 * \fn virtual fmiStatus FMUBase::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) = 0; 
 * @param valref  value references of array of integer variable to be set 
 * @param val     corresponding values to be set   
 * @param ival    number of items in the array 
 * @return        ???   
 **/

 
/**  	 
 * \fn fmistatus FMUBase::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival )
 *
 * @param  valref  value references of array of integer variable to be set 
 * @param  val     corresponding values to be set   
 * @param  ival    number of items in the array 
 * @return         ???
 */ 


/**
 * \fn virtual fmiStatus FMUBase::getValue( const std::string& name,  fmiReal& val ) const = 0;
 * @param name name of the variable  
 * @param current val of the variablet 
 * @return status of getting a value with the given name 
 */
