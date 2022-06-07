// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMU_COSIMULATION_H
#define _FMIPP_FMU_COSIMULATION_H

#include <cstdio>
#include <map>
#include <stdexcept>

#include "import/base/include/BareFMU.h"
#include "import/base/include/FMUCoSimulationBase.h"

class ModelDescription;

/**
 * \file FMUCoSimulation_v1.h 
 *
 * \class FMUCoSimulation FMUCoSimulation_v1.h 
 * Implementation of abstract base class FMUCoSimulationBase (FMI CS Version 1.0).
 *  
 * The FMI standard requires to define the macro MODEL_IDENTIFIER for each entity of FMU CS
 * seperately. This is not done here, because this class links dynamically during run-time.
 */

namespace fmi_1_0 {

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
	FMUCoSimulation( const fmippString& fmuDirUri,
		const fmippString& modelIdentifier,
		const fmippBoolean loggingOn = fmippFalse,
		const fmippTime timeDiffResolution = 1e-9 );

	/**
	 * Constructor. Requires the FMU to be already loaded (via the model manager).
	 *
	 * @param[in]  modelIdentifier       FMI model identifier.
	 * @param[in]  loggingOn             if true, tell the FMU to log all calls to the fmiXXX functons
	 * @param[in]  timeDiffResolution    resolution for comparing the master time with the slave time.
	 */
	FMUCoSimulation( const fmippString& modelIdentifier,
		const fmippBoolean loggingOn = fmippFalse,
		const fmippTime timeDiffResolution = 1e-9 );

	/// Copy constructor.
	FMUCoSimulation( const FMUCoSimulation& fmu );

	/// Destructor.
	~FMUCoSimulation();

	/// Terminate the FMU explicitely.
	virtual void terminate();

	/// \copydoc FMUCoSimulationBase::instantiate
	virtual fmippStatus instantiate( const fmippString& instanceName,
		const fmippTime timeout,
		const fmippBoolean visible,
		const fmippBoolean interactive );

	/// \copydoc FMUCoSimulationBase::initialize
	virtual fmippStatus initialize( const fmippReal startTime,
		const fmippBoolean stopTimeDefined,
		const fmippReal stopTime );

	/// \copydoc FMUCoSimulationBase::doStep
	virtual fmippStatus doStep( fmippTime currentCommunicationPoint,
		fmippTime communicationStepSize,
		fmippBoolean newStep );

	/// \copydoc FMUBase::getTime()
	virtual fmippReal getTime() const;

	/// \copydoc FMUBase::setValue( fmippValueReference valref, const fmippReal& val ) 
	virtual fmippStatus setValue( fmippValueReference valref, const fmippReal& val );

	/// \copydoc FMUBase::setValue( fmippValueReference valref, const fmippInteger& val ) 
	virtual fmippStatus setValue( fmippValueReference valref, const fmippInteger& val );

	/// \copydoc FMUBase::setValue( fmippValueReference valref, const fmippBoolean& val ) 
	virtual fmippStatus setValue( fmippValueReference valref, const fmippBoolean& val );

	/// \copydoc FMUBase::setValue( fmippValueReference valref, const fmippString& val ) 
	virtual fmippStatus setValue( fmippValueReference valref, const fmippString& val );

	/// \copydoc FMUBase::setValue( fmippValueReference* valref, const fmippReal* val, fmippSize ival )
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippReal* val, fmippSize ival );

	/// \copydoc FMUBase::setValue( fmippValueReference* valref, const fmippInteger* val, fmippSize ival )
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippInteger* val, fmippSize ival );

	/// \copydoc FMUBase::setValue( fmippValueReference* valref, const fmippBoolean* val, fmippSize ival )
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippBoolean* val, fmippSize ival );

	/// \copydoc FMUBase::setValue( fmippValueReference* valref, const fmippString* val, fmippSize ival )
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippString* val, fmippSize ival );

	///  \copydoc FMUBase::setValue( const fmippString& name, const fmippReal& val )
	virtual fmippStatus setValue( const fmippString& name, const fmippReal& val );

	///  \copydoc FMUBase::setValue( const fmippString& name, const fmippInteger& val )
	virtual fmippStatus setValue( const fmippString& name, const fmippInteger& val );

	///  \copydoc FMUBase::setValue( const fmippString& name, const fmippBoolean& val )
	virtual fmippStatus setValue( const fmippString& name, const fmippBoolean& val );

	///  \copydoc FMUBase::setValue( const fmippString& name, const fmippString& val )
	virtual fmippStatus setValue( const fmippString& name, const fmippString& val );

	/// \copydoc FMUBase::getValue( fmippValueReference valref, fmippReal& val )
	virtual fmippStatus getValue( fmippValueReference valref, fmippReal& val );

	/// \copydoc FMUBase::getValue( fmippValueReference valref, fmippInteger& val )
	virtual fmippStatus getValue( fmippValueReference valref, fmippInteger& val );

	/// \copydoc FMUBase::getValue( fmippValueReference valref, fmippBoolean& val )
	virtual fmippStatus getValue( fmippValueReference valref, fmippBoolean& val );

	/// \copydoc FMUBase::getValue( fmippValueReference valref, fmippString& val )
	virtual fmippStatus getValue( fmippValueReference valref, fmippString& val );

	/// \copydoc FMUBase::getValue( fmippValueReference* valref, fmippReal* val, fmippSize ival )
	virtual fmippStatus getValue( fmippValueReference* valref, fmippReal* val, fmippSize ival );

	/// \copydoc FMUBase::getValue( fmippValueReference* valref, fmippInteger* val, fmippSize ival )
	virtual fmippStatus getValue( fmippValueReference* valref, fmippInteger* val, fmippSize ival );

	/// \copydoc FMUBase::getValue( fmippValueReference* valref, fmippBoolean* val, fmippSize ival )
	virtual fmippStatus getValue( fmippValueReference* valref, fmippBoolean* val, fmippSize ival );

	/// \copydoc FMUBase::getValue( fmippValueReference* valref, fmippString* val, fmippSize ival )
	virtual fmippStatus getValue( fmippValueReference* valref, fmippString* val, fmippSize ival );

	/// \copydoc FMUBase::getValue( const fmippString& name,  fmippReal& val )
	virtual fmippStatus getValue( const fmippString& name, fmippReal& val );

	/// \copydoc FMUBase::getValue( const fmippString& name,  fmippInteger& val )
	virtual fmippStatus getValue( const fmippString& name, fmippInteger& val );

	/// \copydoc FMUBase::getValue( const fmippString& name,  fmippBoolean& val )
	virtual fmippStatus getValue( const fmippString& name, fmippBoolean& val );

	/// \copydoc FMUBase::getValue( const fmippString& name,  fmippString& val )
	virtual fmippStatus getValue( const fmippString& name, fmippString& val );

	/// \copydoc FMUBase::getRealValue( const fmippString& name )
	virtual fmippReal getRealValue( const fmippString& name );

	/// \copydoc FMUBase::getIntegerValue( const fmippString& name )
	virtual fmippInteger getIntegerValue( const fmippString& name );

	/// \copydoc FMUBase::getBooleanValue( const fmippString& name )
	virtual fmippBoolean getBooleanValue( const fmippString& name );

	/// \copydoc FMUBase::getStringValue( const fmippString& name )
	virtual fmippString getStringValue( const fmippString& name );

	/// \copydoc FMUBase::getLastStatus
	virtual fmippStatus getLastStatus() const;

	/// \copydoc FMUBase::getValueRef
	virtual fmippValueReference getValueRef( const fmippString& name ) const;

	/// \copydoc FMUBase::nStates
	virtual fmippSize nStates() const;

	/// \copydoc FMUBase::nEventInds
	virtual fmippSize nEventInds() const;

	/// \copydoc FMUBase::nValueRefs
	virtual fmippSize nValueRefs() const;

	/// \copydoc FMUBase::getModelDescription
	virtual const ModelDescription* getModelDescription() const;

	/// \copydoc FMUBase::getType
	virtual FMIPPVariableType getType( const fmippString& variableName ) const;

	/// \copydoc FMUBase::canHandleVariableCommunicationStepSize
	virtual fmippBoolean canHandleVariableCommunicationStepSize() const;

	/// \copydoc FMUBase::canHandleEvents
	virtual fmippBoolean canHandleEvents() const;

	/// \copydoc FMUBase::canRejectSteps
	virtual fmippBoolean canRejectSteps() const;

	/// \copydoc FMUBase::canInterpolateInputs
	virtual fmippBoolean canInterpolateInputs() const;

	/// \copydoc FMUBase::maxOutputDerivativeOrder
	virtual fmippSize maxOutputDerivativeOrder() const;

	/// \copydoc FMUBase::canRunAsynchronuously
	virtual fmippBoolean canRunAsynchronuously() const;

	/// \copydoc FMUBase::canSignalEvents
	virtual fmippBoolean canSignalEvents() const;

	/// \copydoc FMUBase::canBeInstantiatedOnlyOncePerProcess
	virtual fmippBoolean canBeInstantiatedOnlyOncePerProcess() const;

	/// \copydoc FMUBase::canNotUseMemoryManagementFunctions
	virtual fmippBoolean canNotUseMemoryManagementFunctions() const;

	/// Call logger to issue a debug message.
	virtual void sendDebugMessage( const fmippString& msg ) const;

	/// Send message to FMUCoSimulation logger.
	void logger( fmiStatus status, const fmippString& category, const fmippString& msg ) const;

    /// Send message to FMUCoSimulation logger.	
	void logger( fmiStatus status, const fmippChar* category, const fmippChar* msg ) const;

	/************ Unique functions for FMI ME 1.0 ************/

	/// Set callback functions.
	virtual fmippStatus setCallbacks( cs::fmiCallbackLogger logger,
		cs::fmiCallbackAllocateMemory allocateMemory,
		cs::fmiCallbackFreeMemory freeMemory,
		cs::fmiStepFinished stepFinished );	

private:

	/// Internal helper function to retrieve attributes from model description.
	template<typename Type>
	Type getCoSimToolCapabilities( const fmippString& attributeName ) const;
	
	FMUCoSimulation(); ///< Prevent calling the default constructor.

	fmippString instanceName_;  ///< Name of the instantiated CS FMU.

	fmiComponent instance_; ///< Internal FMUCoSimulation instance.

	BareFMUCoSimulationPtr fmu_; ///< Internal pointer to bare FMU ME functionalities and model description.

	cs::fmiCallbackFunctions callbacks_; ///< Internal struct to callback functions.

	/// \FIXME Maps should be handled via ModelManager, to avoid duplication 
	///        of this (potentially large) map with every instance.
	std::map<fmippString,fmippValueReference> varMap_;  ///< Maps variable names and value references.
	std::map<fmippString,FMIPPVariableType> varTypeMap_; ///< Maps variable names and their types.

	fmippTime time_; ///< Internal time.
	const fmippTime timeDiffResolution_; ///< Internal time resolution.

	fmiStatus lastStatus_; ///< Last status returned by the FMU.

	void readModelDescription(); ///< Read the model description.

};

} // namespace fmi_1_0

#endif // _FMIPP_FMU_COSIMULATION_H
