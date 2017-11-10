// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMUCOSIMULATIONBASE_H
#define _FMIPP_FMUCOSIMULATIONBASE_H


#include "import/base/include/FMUBase.h"
#include "common/fmi_v1.0/fmi_cs.h"


/**
 * \file FMUCoSimulationBase.h
 *
 * \class FMUCoSimulationBase FMUCoSimulationBase.h  Abstract base class for wrappers handling FMUs for CS.
 *
 * This class defines the main functions that need to be implemented by any class used for
 * handling FMUs for CS. This includes instantiation, initialization and stepping of FMUs.

 **/


class __FMI_DLL FMUCoSimulationBase : public FMUBase
{

public:


	/// Constructor.
	FMUCoSimulationBase( fmiBoolean loggingOn ) : loggingOn_( loggingOn ) {}

	/// Destructor.
	virtual ~FMUCoSimulationBase() {}

	/// Terminate the FMU explicitely.
	virtual void terminate() = 0;

	/**
	 * Instantiate the FMU. This function has to be called successfully (i.e., with return
	 * status fmiOK) before any other function is called.
	 *
	 * @param[in]  instanceName  name of the FMI instance.
	 * @param[in]  timeout  communication timeout value in milli-seconds to allow inter-process
	 *             communication to take place. A timeout value of 0 indicates an infinite wait period
	 * @param[in]  visible  indicates whether or not the simulator application window needed to
	 *             execute a model should be visible
	 * @param[in]  interactive  indicates whether the simulator application must be manually started
	 *             by the user.
	 * @param[in]  loggingOn Set verbosity of logger
	 * @return the instantiation status
	 */
	virtual fmiStatus instantiate( const std::string& instanceName,
		const fmiReal timeout,
		const fmiBoolean visible,
		const fmiBoolean interactive ) = 0;

	/**
	 * Initialize the FMU CS model and inform the slave that the simulation run starts now.
	 *
	 * @param[in]  startTime  start time for the model
	 * @param[in]  stopTimeDefined  is true (fmiTrue) if a stop time is defined
	 * @param[in]  stopTime  stop time for model
	 * @return initilization status.
	 */
	virtual fmiStatus initialize( const fmiReal startTime,
		const fmiBoolean stopTimeDefined,
		const fmiReal stopTime ) = 0;

	
	/**
	 * Call doStep(...) function of CS FMU.
	 *
	 * @param[in]  currentCommunicationPoint  current communication point of the master
	 * @param[in]  communicationStepSize  communication step size, if the master carries out an
	 *             event iteration the parameter is zero
	 * @param[in]  newStep  is true (fmiTrue) if the last communication step is accepted by the
	 *             master and a new communication step is started
	 * @return simulation step status.
	 */
	virtual fmiStatus doStep( fmiReal currentCommunicationPoint,
		fmiReal communicationStepSize,
		fmiBoolean newStep ) = 0;

	/**
	 * Provide basic information about FMU implementation from model description.
	 */
	virtual bool canHandleVariableCommunicationStepSize() const = 0;

	/**
	 * Provide basic information about FMU implementation from model description.
	 */
	virtual bool canHandleEvents() const = 0;

	/**
	 * Provide basic information about FMU implementation from model description.
	 */
	virtual bool canRejectSteps() const = 0;

	/**
	 * Provide basic information about FMU implementation from model description.
	 */
	virtual bool canInterpolateInputs() const = 0;

	/**
	 * Provide basic information about FMU implementation from model description.
	 */
	virtual size_t maxOutputDerivativeOrder() const = 0;

	/**
	 * Provide basic information about FMU implementation from model description.
	 */
	virtual bool canRunAsynchronuously() const = 0;

	/**
	 * Provide basic information about FMU implementation from model description.
	 */
	virtual bool canSignalEvents() const = 0;

	/**
	 * Provide basic information about FMU implementation from model description.
	 */
	virtual bool canBeInstantiatedOnlyOncePerProcess() const = 0;

	/**
	 * Provide basic information about FMU implementation from model description.
	 */
	virtual bool canNotUseMemoryManagementFunctions() const = 0;
	
protected:

	const fmiBoolean loggingOn_;
};


#endif // _FMIPP_FMUCOSIMULATIONBASE_H
