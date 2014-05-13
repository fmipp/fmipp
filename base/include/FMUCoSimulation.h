/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMU_COSIMULATION_H
#define _FMIPP_FMU_COSIMULATION_H


#include <map>

#include "base/include/FMUCoSimulationBase.h"


struct BareFMUCoSimulation;


/**
 * \file FMUCoSimulation.h 
 *
 * \class FMUCoSimulation FMUCoSimulation.h 
 * Implementation of abstract base class FMUCoSimulationBase.
 *  
 * The FMI standard requires to define the macro MODEL_IDENTIFIER for each entity of FMU CS
 * seperately. This is not done here, because this class links dynamically during run-time.
 */


class __FMI_DLL FMUCoSimulation : public FMUCoSimulationBase
{

public:

	/**
	 * Constructor.
	 *
	 * @param[in]  fmuPath  path to FMU (as URI)
	 * @param[in]  modelName  model name
	 */
	FMUCoSimulation( const std::string& fmuPath,
			 const std::string& modelName );

	/// Copy constructor.
	FMUCoSimulation( const FMUCoSimulation& fmu );

	/// Destructor.
	~FMUCoSimulation();


	/// \copydoc FMUCoSimulationBase::instantiate
	virtual fmiStatus instantiate( const std::string& instanceName,
				       const fmiReal& timeout,
				       const fmiBoolean& visible,
				       const fmiBoolean& interactive,
				       const fmiBoolean& loggingOn );

	/// \copydoc FMUCoSimulationBase::initialize
	virtual fmiStatus initialize( const fmiReal& startTime,
				      const fmiBoolean& stopTimeDefined,
				      const fmiReal& stopTime );

	/// \copydoc FMUCoSimulationBase::doStep
	virtual fmiStatus doStep( fmiReal currentCommunicationPoint,
				  fmiReal communicationStepSize,
				  fmiBoolean newStep );


	/// \copydoc FMUHandle::getTime()
	virtual fmiReal getTime() const;

	/// \copydoc FMUHandle::setValue( fmiValueReference valref, fmiReal& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, fmiReal& val );

	/// \copydoc FMUHandle::setValue( fmiValueReference valref, fmiInteger& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, fmiInteger& val );

	/// \copydoc FMUHandle::setValue( fmiValueReference valref, fmiBoolean& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, fmiBoolean& val );

	/// \copydoc FMUHandle::setValue( fmiValueReference valref, std::string& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, std::string& val );

	/// \copydoc FMUHandle::setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival );

	/// \copydoc FMUHandle::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival );

	/// \copydoc FMUHandle::setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival );

	/// \copydoc FMUHandle::setValue( fmiValueReference* valref, std::string* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, std::string* val, std::size_t ival );

	///  \copydoc FMUHandle::setValue( const std::string& name,  fmiReal val )
	virtual fmiStatus setValue( const std::string& name, fmiReal val );

	///  \copydoc FMUHandle::setValue( const std::string& name,  fmiInteger val )
	virtual fmiStatus setValue( const std::string& name, fmiInteger val );

	///  \copydoc FMUHandle::setValue( const std::string& name,  fmiBoolean val )
	virtual fmiStatus setValue( const std::string& name, fmiBoolean val );

	///  \copydoc FMUHandle::setValue( const std::string& name,  std::string val )
	virtual fmiStatus setValue( const std::string& name, std::string val );

	/// \copydoc FMUHandle::getValue( fmiValueReference valref, fmiReal& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, fmiReal& val ) const;

	/// \copydoc FMUHandle::getValue( fmiValueReference valref, fmiInteger& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, fmiInteger& val ) const;

	/// \copydoc FMUHandle::getValue( fmiValueReference valref, fmiBoolean& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, fmiBoolean& val ) const;

	/// \copydoc FMUHandle::getValue( fmiValueReference valref, std::string& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, std::string& val ) const;

	/// \copydoc FMUHandle::getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const;

	/// \copydoc FMUHandle::getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) const;

	/// \copydoc FMUHandle::getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) const;

	/// \copydoc FMUHandle::getValue( fmiValueReference* valref, std::string* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, std::string* val, std::size_t ival ) const;

	/// \copydoc FMUHandle::getValue( const std::string& name,  fmiReal& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiReal& val ) const;

	/// \copydoc FMUHandle::getValue( const std::string& name,  fmiInteger& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiInteger& val ) const;

	/// \copydoc FMUHandle::getValue( const std::string& name,  fmiBoolean& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiBoolean& val ) const;

	/// \copydoc FMUHandle::getValue( const std::string& name,  std::string& val ) const 
	virtual fmiStatus getValue( const std::string& name, std::string& val ) const;

	/// \copydoc FMUHandle::getValueRef
	virtual fmiValueReference getValueRef( const std::string& name ) const;

	/// \copydoc FMUHandle::nStates
	virtual std::size_t nStates() const;

	/// \copydoc FMUHandle::nEventInds
	virtual std::size_t nEventInds() const;

	/// \copydoc FMUHandle::nValueRefs
	virtual std::size_t nValueRefs() const;

	/// \copydoc FMUHandle::getType
	virtual FMIType getType( const std::string& variableName ) const;


	/// Send message to FMUCoSimulation logger.
	void logger( fmiStatus status, const std::string& msg ) const;
        /// Send message to FMUCoSimulation logger.	
	void logger( fmiStatus status, const char* msg ) const;
	/// Logger function handed to the internal FMUCoSimulation instance.
	static void logger( fmiComponent m, fmiString instanceName,
			    fmiStatus status, fmiString category,
			    fmiString message, ... );

private:


	FMUCoSimulation(); ///< Prevent calling the default constructor.

	std::string instanceName_;  ///< Name of the instantiated CS FMU.

	std::string fmuPath_; ///< Path to the FMU.

	fmiComponent instance_; ///< Internal FMUCoSimulation instance.

	BareFMUCoSimulation* fmu_; ///< Internal pointer to bare FMU ME functionalities and model description.

	/// \FIXME Maps should be handled via ModelManager, to avoid duplication 
	///        of this (potentially large) map with every instance.
	std::map<std::string,fmiValueReference> varMap_;  ///< Maps variable names and value references.
	std::map<std::string,FMIType> varTypeMap_; ///< Maps variable names and their types.

	fmiReal time_; 

	void readModelDescription();

};

#endif // _FMIPP_FMU_COSIMULATION_H
