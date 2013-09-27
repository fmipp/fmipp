/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMUCOSIMULATION_H
#define _FMIPP_FMUCOSIMULATION_H


#include <map>

#include "FMUCoSimulationBase.h"


class FMUCoSimulationIntegrator;


/**
 *  \file FMUCoSimulation.h 
 *  \class FMUCoSimulation FMUCoSimulation.h 
 *   Main functionalities for self integration of FMUs. 
 *  
 *  The FMI standard requires to define the macro MODEL_IDENTIFIER for each
 *  type of FMU seperately. This is not done here, because this class links
 *  dynamically during run-time.
 */


class __FMI_DLL FMUCoSimulation : public FMUCoSimulationBase
{

public:

	FMUCoSimulation( const std::string& fmuPath,
			 const std::string& modelName );

	FMUCoSimulation( const FMUCoSimulation& fmu );

	~FMUCoSimulation();


	virtual fmiStatus instantiate( const std::string& instanceName,
				       const fmiReal& timeout,
				       const fmiBoolean& visible,
				       const fmiBoolean& interactive,
				       const fmiBoolean& loggingOn ); ///< Instantiate the FMU.

	virtual fmiStatus initialize( const fmiReal& startTime,
				      const fmiBoolean& stopTimeDefined,
				      const fmiReal& stopTime ); ///< \copydoc FMUCoSimulationBase::initialize

	virtual fmiReal getTime() const; ///< \copydoc FMUCoSimulationBase::getTime()

	/// \copydoc FMUCoSimulationBase::setValue( fmiValueReference valref, fmiReal& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, fmiReal& val );

	/// \copydoc FMUCoSimulationBase::setValue( fmiValueReference valref, fmiInteger& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, fmiInteger& val );

	/// \copydoc FMUCoSimulationBase::setValue( fmiValueReference valref, fmiBoolean& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, fmiBoolean& val );

	/// \copydoc FMUCoSimulationBase::setValue( fmiValueReference valref, std::string& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, std::string& val );

	/// \copydoc FMUCoSimulationBase::setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival );

	/// \copydoc FMUCoSimulationBase::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival );

	/// \copydoc FMUCoSimulationBase::setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival );

	/// \copydoc FMUCoSimulationBase::setValue( fmiValueReference* valref, std::string* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, std::string* val, std::size_t ival );

	///  \copydoc FMUCoSimulationBase::setValue( const std::string& name,  fmiReal val )
	virtual fmiStatus setValue( const std::string& name, fmiReal val );

	///  \copydoc FMUCoSimulationBase::setValue( const std::string& name,  fmiInteger val )
	virtual fmiStatus setValue( const std::string& name, fmiInteger val );

	///  \copydoc FMUCoSimulationBase::setValue( const std::string& name,  fmiBoolean val )
	virtual fmiStatus setValue( const std::string& name, fmiBoolean val );

	///  \copydoc FMUCoSimulationBase::setValue( const std::string& name,  std::string val )
	virtual fmiStatus setValue( const std::string& name, std::string val );

	/// \copydoc FMUCoSimulationBase::getValue( fmiValueReference valref, fmiReal& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, fmiReal& val ) const;

	/// \copydoc FMUCoSimulationBase::getValue( fmiValueReference valref, fmiInteger& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, fmiInteger& val ) const;

	/// \copydoc FMUCoSimulationBase::getValue( fmiValueReference valref, fmiBoolean& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, fmiBoolean& val ) const;

	/// \copydoc FMUCoSimulationBase::getValue( fmiValueReference valref, std::string& val ) const 
	virtual fmiStatus getValue( fmiValueReference valref, std::string& val ) const;

	/// \copydoc FMUCoSimulationBase::getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const;

	/// \copydoc FMUCoSimulationBase::getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) const;

	/// \copydoc FMUCoSimulationBase::getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) const;

	/// \copydoc FMUCoSimulationBase::getValue( fmiValueReference* valref, std::string* val, std::size_t ival ) const 
	virtual fmiStatus getValue( fmiValueReference* valref, std::string* val, std::size_t ival ) const;

	/// \copydoc FMUCoSimulationBase::getValue( const std::string& name,  fmiReal& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiReal& val ) const;

	/// \copydoc FMUCoSimulationBase::getValue( const std::string& name,  fmiInteger& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiInteger& val ) const;

	/// \copydoc FMUCoSimulationBase::getValue( const std::string& name,  fmiBoolean& val ) const 
	virtual fmiStatus getValue( const std::string& name, fmiBoolean& val ) const;

	/// \copydoc FMUCoSimulationBase::getValue( const std::string& name,  std::string& val ) const 
	virtual fmiStatus getValue( const std::string& name, std::string& val ) const;

	/// \copydoc FMUCoSimulationBase::getValueRef
	virtual fmiValueReference getValueRef( const std::string& name ) const;

	virtual fmiStatus doStep( fmiReal currentCommunicationPoint,
				  fmiReal communicationStepSize,
				  fmiBoolean newStep ) const;


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

	FMUCoSimulation_functions *fmuFun_; ///< Internal pointer to FMI CS functions.

	/** Maps variable names and value references. TODO: Map should
	 *  be handled via ModelManager, to avoid duplication of this
	 * (potentially large) map with every instance. */
	std::map<std::string,fmiValueReference> varMap_;

	fmiReal time_; 

	void readModelDescription();

};

#endif // _FMIPP_FMUCoSimulation_H
