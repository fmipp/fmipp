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

        
        virtual ~FMUCoSimulationBase() {} ///< Destructor.

	virtual fmiStatus instantiate( const std::string& instanceName,
				       fmiReal timeout,
				       fmiBoolean visible,
				       fmiBoolean interactive,
				       fmiBoolean loggingOn ) = 0; ///< Instantiate the FMU.

	virtual fmiStatus initialize( fmiReal tStart,
				      fmiBoolean stopTimeDefined,
				      fmiReal tStop ) = 0; ///< Initialize the FMU. 

	virtual fmiReal getTime() const = 0;                    ///< Get current time.

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

	virtual fmiStatus doStep( fmiReal currentCommunicationPoint,
				  fmiReal communicationStepSize,
				  fmiBoolean newStep ) const = 0;

};


#endif // _FMIPP_FMUCOSIMULATIONBASE_H

/**
 * 
 * \fn 	fmiStatus FMUCoSimulationBase::instantiate( const std::string& instanceName, fmiBoolean loggingOn )
 * @param[in]  instanceName  name of the fmi instance 
 * @param[in]  loggingOn 
 * @return the instantation status 
 */
 
/**
 * \fn FMUCoSimulationBase::initialize
 * Initializing the FMU model at time t_0 (after model parameters and start values have been set)
 * @return initilization status.
 */

/** 
 * \fn virtual fmiStatus FMUCoSimulationBase::setValue( fmiValueReference valref, fmiReal& val ) = 0;
 * @param[in]  valref   reference of real variable to be set 
 * @param[in]  val      value to be set
 * @return setting  value status  
 **/

/** 
 * \fn virtual fmiStatus FMUCoSimulationBase::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) = 0; 
 * @param[in] valref  value references of array of integer variable to be set 
 * @param[in] val     corresponding values to be set   
 * @param[in] ival    number of items in the array 
 * @return        ???   
 **/

 
/**  	 
 * \fn fmistatus FMUCoSimulationBase::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival )
 *
 * @param[in]  valref  value references of array of integer variable to be set 
 * @param[in]  val     corresponding values to be set   
 * @param[in]  ival    number of items in the array 
 * @return         ???
 */ 


/**
 * \fn virtual fmiStatus FMUCoSimulationBase::getValue( const std::string& name,  fmiReal& val ) const = 0;
 * @param[in] name name of the variable  
 * @param[in] current val of the variablet 
 * @return status of getting a value with the given name 
 */
