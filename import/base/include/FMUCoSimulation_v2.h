// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMU2_COSIMULATION_H
#define _FMIPP_FMU2_COSIMULATION_H


#include <cstdio>
#include <map>
#include <stdexcept>

#include "import/base/include/BareFMU.h"
#include "import/base/include/FMUCoSimulationBase.h"

class ModelDescription;


/**
 * \file FMUCoSimulation_v2.h 
 *
 * \class FMUCoSimulation FMUCoSimulation_v2.h 
 * Implementation of abstract base class FMUCoSimulationBase (FMI CS Version 2.0).
 */

namespace fmi_2_0 {

class __FMI_DLL FMUCoSimulation : public FMUCoSimulationBase
{

public:

	/**
	 * Constructor. Loads the FMU via the model manager (if needed).
	 *
	 * @param[in]  fmuDirUri             path to unzipped FMU directory (as URI)
	 * @param[in]  modelIdentifier       FMI model identifier
	 * @param[in]  loggingOn             if true, tell the FMU to log all calls to the fmiXXX functons
	 * @param[in]  timeDiffResolution    resolution for comparing the master time with the slave time.
	 */
	FMUCoSimulation( const std::string& fmuDirUri,
		const std::string& modelIdentifier,
		const fmiBoolean loggingOn = fmiFalse,
		const fmiReal timeDiffResolution = 1e-9 );

	/**
	 * Constructor. Requires the FMU to be already loaded (via the model manager).
	 *
	 * @param[in]  modelIdentifier       FMI model identifier.
	 * @param[in]  loggingOn             if true, tell the FMU to log all calls to the fmiXXX functons
	 * @param[in]  timeDiffResolution    resolution for comparing the master time with the slave time.
	 */
	FMUCoSimulation( const std::string& modelIdentifier,
		const fmiBoolean loggingOn = fmiFalse,
		const fmiReal timeDiffResolution = 1e-9 );


	/// Copy constructor.
	FMUCoSimulation( const FMUCoSimulation& fmu );

	/// Destructor.
	~FMUCoSimulation();

	/// Terminate the FMU explicitely.
	virtual void terminate();

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

	/// \copydoc FMUBase::getModelDescription
	virtual const ModelDescription* getModelDescription() const;

	/// \copydoc FMUBase::getType
	virtual FMIVariableType getType( const std::string& variableName ) const;

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

	/************ Unique functions for FMI 2.0 ************/

	/// Set callback functions.
	virtual fmiStatus setCallbacks( fmi2::fmi2CallbackLogger logger,
		fmi2::fmi2CallbackAllocateMemory allocateMemory,
		fmi2::fmi2CallbackFreeMemory freeMemory,
		fmi2::fmi2StepFinished stepFinished );	

	/// Set component environment.
	virtual void setComponentEnvironment( fmi2ComponentEnvironment env ) {
		callbacks_.componentEnvironment = env;
	}

private:

	/// Internal helper function to retrieve attributes from model description.
	template<typename Type>
	Type getCoSimToolCapabilities( const std::string& attributeName ) const;
	
	FMUCoSimulation(); ///< Prevent calling the default constructor.

	std::string instanceName_;  ///< Name of the instantiated CS FMU.

	fmi2Component instance_; ///< Internal FMUCoSimulation instance.

	BareFMU2Ptr fmu_; ///< Internal pointer to bare FMU ME functionalities and model description.

	fmi2::fmi2CallbackFunctions callbacks_; ///< Internal struct to callback functions.

	/// \FIXME Maps should be handled via ModelManager, to avoid duplication 
	///        of this (potentially large) map with every instance.
	std::map<std::string,fmi2ValueReference> varMap_;  ///< Maps variable names and value references.
	std::map<std::string,FMIVariableType> varTypeMap_; ///< Maps variable names and their types.

	fmi2Real time_; ///< Internal time.
	const fmi2Real timeDiffResolution_; ///< Internal time resolution.

	fmi2Status lastStatus_; ///< Last status returned by the FMU.

	void readModelDescription(); ///< Read the model description.

};

} // namespace fmi_2_0

#endif // _FMIPP_FMU2_COSIMULATION_H
