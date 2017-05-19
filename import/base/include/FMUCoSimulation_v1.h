/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_FMU_COSIMULATION_H
#define _FMIPP_FMU_COSIMULATION_H


#include <cstdio>
#include <map>

#include <boost/shared_ptr.hpp>

#include <stdexcept>


#include "import/base/include/FMUCoSimulationBase.h"

struct BareFMUCoSimulation;
class ModelDescription;


/**
 * \file FMUCoSimulation.h 
 *
 * \class FMUCoSimulation FMUCoSimulation.h 
 * Implementation of abstract base class FMUCoSimulationBase.
 *  
 * The FMI standard requires to define the macro MODEL_IDENTIFIER for each entity of FMU CS
 * seperately. This is not done here, because this class links dynamically during run-time.
 */

namespace fmi_1_0 {

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
			 const std::string& modelName,
			 const fmiBoolean loggingOn = fmiFalse,
			 const fmiReal timeDiffResolution = 1e-9 );

	/// Copy constructor.
	FMUCoSimulation( const FMUCoSimulation& fmu );

	/// Destructor.
	~FMUCoSimulation();


	/// \copydoc FMUCoSimulationBase::instantiate
	virtual fmiStatus instantiate( const std::string& instanceName,
				       const fmiReal timeout,
				       const fmiBoolean visible,
				       const fmiBoolean interactive );

	/// \copydoc FMUCoSimulationBase::initialize
	virtual fmiStatus initialize( const fmiReal startTime,
				      const fmiBoolean stopTimeDefined,
				      const fmiReal stopTime );

	/// \copydoc FMUCoSimulationBase::doStep
	virtual fmiStatus doStep( fmiReal currentCommunicationPoint,
				  fmiReal communicationStepSize,
				  fmiBoolean newStep );

	/// \copydoc FMUCoSimulationBase::setCallbacks
	virtual fmiStatus setCallbacks( cs::fmiCallbackLogger logger,
					cs::fmiCallbackAllocateMemory allocateMemory,
					cs::fmiCallbackFreeMemory freeMemory,
					cs::fmiStepFinished stepFinished );

	/// \copydoc FMUBase::getTime()
	virtual fmiReal getTime() const;

	/// \copydoc FMUBase::setValue( fmiValueReference valref, fmiReal& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, fmiReal& val );

	/// \copydoc FMUBase::setValue( fmiValueReference valref, fmiInteger& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, fmiInteger& val );

	/// \copydoc FMUBase::setValue( fmiValueReference valref, fmiBoolean& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, fmiBoolean& val );

	/// \copydoc FMUBase::setValue( fmiValueReference valref, std::string& val ) 
	virtual fmiStatus setValue( fmiValueReference valref, std::string& val );

	/// \copydoc FMUBase::setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival );

	/// \copydoc FMUBase::setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival );

	/// \copydoc FMUBase::setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival );

	/// \copydoc FMUBase::setValue( fmiValueReference* valref, std::string* val, std::size_t ival )
	virtual fmiStatus setValue( fmiValueReference* valref, std::string* val, std::size_t ival );

	///  \copydoc FMUBase::setValue( const std::string& name,  fmiReal val )
	virtual fmiStatus setValue( const std::string& name, fmiReal val );

	///  \copydoc FMUBase::setValue( const std::string& name,  fmiInteger val )
	virtual fmiStatus setValue( const std::string& name, fmiInteger val );

	///  \copydoc FMUBase::setValue( const std::string& name,  fmiBoolean val )
	virtual fmiStatus setValue( const std::string& name, fmiBoolean val );

	///  \copydoc FMUBase::setValue( const std::string& name,  std::string val )
	virtual fmiStatus setValue( const std::string& name, std::string val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiReal& val )
	virtual fmiStatus getValue( fmiValueReference valref, fmiReal& val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiInteger& val )
	virtual fmiStatus getValue( fmiValueReference valref, fmiInteger& val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, fmiBoolean& val )
	virtual fmiStatus getValue( fmiValueReference valref, fmiBoolean& val );

	/// \copydoc FMUBase::getValue( fmiValueReference valref, std::string& val )
	virtual fmiStatus getValue( fmiValueReference valref, std::string& val );

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival )
	virtual fmiStatus getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival );

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival )
	virtual fmiStatus getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival );

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival )
	virtual fmiStatus getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival );

	/// \copydoc FMUBase::getValue( fmiValueReference* valref, std::string* val, std::size_t ival )
	virtual fmiStatus getValue( fmiValueReference* valref, std::string* val, std::size_t ival );

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiReal& val )
	virtual fmiStatus getValue( const std::string& name, fmiReal& val );

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiInteger& val )
	virtual fmiStatus getValue( const std::string& name, fmiInteger& val );

	/// \copydoc FMUBase::getValue( const std::string& name,  fmiBoolean& val )
	virtual fmiStatus getValue( const std::string& name, fmiBoolean& val );

	/// \copydoc FMUBase::getValue( const std::string& name,  std::string& val )
	virtual fmiStatus getValue( const std::string& name, std::string& val );

	/// \copydoc FMUBase::getRealValue( const std::string& name )
	virtual fmiReal getRealValue( const std::string& name );

	/// \copydoc FMUBase::getIntegerValue( const std::string& name )
	virtual fmiInteger getIntegerValue( const std::string& name );

	/// \copydoc FMUBase::getBooleanValue( const std::string& name )
	virtual fmiBoolean getBooleanValue( const std::string& name );

	/// \copydoc FMUBase::getStringValue( const std::string& name )
	virtual fmiString getStringValue( const std::string& name );

	/// \copydoc FMUBase::getLastStatus
	virtual fmiStatus getLastStatus() const;

	/// \copydoc FMUBase::getValueRef
	virtual fmiValueReference getValueRef( const std::string& name ) const;

	/// \copydoc FMUBase::nStates
	virtual std::size_t nStates() const;

	/// \copydoc FMUBase::nEventInds
	virtual std::size_t nEventInds() const;

	/// \copydoc FMUBase::nValueRefs
	virtual std::size_t nValueRefs() const;

	/// \copydoc FMUBase::getType
	virtual FMIType getType( const std::string& variableName ) const;

	/// \copydoc FMUBase::canHandleVariableCommunicationStepSize
	virtual bool canHandleVariableCommunicationStepSize() const;

	/// \copydoc FMUBase::canHandleEvents
	virtual bool canHandleEvents() const;

	/// \copydoc FMUBase::canRejectSteps
	virtual bool canRejectSteps() const;

	/// \copydoc FMUBase::canInterpolateInputs
	virtual bool canInterpolateInputs() const;

	/// \copydoc FMUBase::maxOutputDerivativeOrder
	virtual size_t maxOutputDerivativeOrder() const;

	/// \copydoc FMUBase::canRunAsynchronuously
	virtual bool canRunAsynchronuously() const;

	/// \copydoc FMUBase::canSignalEvents
	virtual bool canSignalEvents() const;

	/// \copydoc FMUBase::canBeInstantiatedOnlyOncePerProcess
	virtual bool canBeInstantiatedOnlyOncePerProcess() const;

	/// \copydoc FMUBase::canNotUseMemoryManagementFunctions
	virtual bool canNotUseMemoryManagementFunctions() const;

	/// Call logger to issue a debug message.
	virtual void sendDebugMessage( const std::string& msg ) const;

	/// Send message to FMUCoSimulation logger.
	void logger( fmiStatus status, const std::string& category, const std::string& msg ) const;

    /// Send message to FMUCoSimulation logger.	
	void logger( fmiStatus status, const char* category, const char* msg ) const;

private:

	/// Internal helper function to retrieve attributes from model description.
	template<typename Type>
	Type getCoSimToolCapabilities( const std::string& attributeName ) const;

	/// Get pointer to model description.
	const ModelDescription* getModelDescription() const;
	
	FMUCoSimulation(); ///< Prevent calling the default constructor.

	std::string instanceName_;  ///< Name of the instantiated CS FMU.

	fmiComponent instance_; ///< Internal FMUCoSimulation instance.

	boost::shared_ptr<BareFMUCoSimulation> fmu_; ///< Internal pointer to bare FMU ME functionalities and model description.

	std::string fmuPath_; ///< Path to the FMU.

	/// \FIXME Maps should be handled via ModelManager, to avoid duplication 
	///        of this (potentially large) map with every instance.
	std::map<std::string,fmiValueReference> varMap_;  ///< Maps variable names and value references.
	std::map<std::string,FMIType> varTypeMap_; ///< Maps variable names and their types.

	fmiReal time_; ///< Internal time.
	const fmiReal timeDiffResolution_; ///< Internal time resolution.

	fmiStatus lastStatus_; ///< Last status returned by the FMU.

	void readModelDescription(); ///< Read the model description.

};

} // namespace fmi_1_0

#endif // _FMIPP_FMU_COSIMULATION_H