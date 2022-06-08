// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_INCREMENTALFMU_H
#define _FMIPP_INCREMENTALFMU_H

#include "common/FMIPPConfig.h"

#include "import/utility/include/History.h"
#include "import/integrators/include/Integrator.h"

class FMUModelExchangeBase;
class ModelDescription;

/**
 * \file IncrementalFMU.h 
 * \class IncrementalFMU IncrementalFMU.h 
 * The FMI++ library offers the possibility to combine the basic ability to integrate
 * the state of an FMU for ME with advanced event handling capabilities.
 *
 * This is especially useful when using FMUs within discrete event-based simulation environments,
 * where the time difference between updates is not constant. The class IncrementalFMU implements
 * a lookahead mechanism, where predictions of the FMU’s state are incrementally computed and stored.
 * In case an event occurs, these predictions are then used to interpolate and update the state of
 * the FMU. If no event occurs, the latest prediction can be directly used to update the FMU’s state.
 */ 

class __FMI_DLL IncrementalFMU
{

public:

	/**
	 * Constructor.
	 *
	 * @param[in]  fmuDirUri  path to unzipped FMU directory (as URI)
	 * @param[in]  modelIdentifier  FMI model identifier
	 * @param[in]  loggingOn  flag for logging
	 * @param[in]  timeDiffResolution  resolution for time comparison and event search during integration
	 * @param[in]  integratorType  integrator type
	 */
	IncrementalFMU( const fmippString& fmuDirUri,
		const fmippString& modelIdentifier,
		const fmippBoolean loggingOn = fmippFalse,
		const fmippTime timeDiffResolution = 1e-4,
#ifdef USE_SUNDIALS
		const IntegratorType integratorType = IntegratorType::bdf
#else
		const IntegratorType integratorType = IntegratorType::dp
#endif
	);

	/**
	 * Constructor which accesses a previously loaded FMU (via ModelManager)
	 *
	 * It is assumed that the model with the given identifier was successfully 
	 * loaded before. In case the model cannot be instantiates or it is not a 
	 * model exchange FMU, the Incremental FMU mill switch to error state.
	 * @param[in]  modelIdentifier  FMI model identifier of the previously loaded
	 * model
	 * @param[in]  loggingOn  flag for logging
	 * @param[in]  timeDiffResolution  resolution for time comparison and event 
	 * search during integration
	 * @param[in]  integratorType  integrator type
	 */
	IncrementalFMU( const fmippString& modelIdentifier,
		const fmippBoolean loggingOn = fmippFalse,
		const fmippReal timeDiffResolution = 1e-4,
#ifdef USE_SUNDIALS
		const IntegratorType integratorType = IntegratorType::bdf
#else
		const IntegratorType integratorType = IntegratorType::dp
#endif
	);

	virtual ~IncrementalFMU();

	int init( const fmippString& instanceName,
		const fmippString realVariableNames[],
		const fmippReal* realValues,
		const fmippSize nRealVars,
		const fmippTime startTime,
		const fmippTime lookAheadHorizon,
		const fmippTime lookAheadStepSize,
		const fmippTime integratorStepSize,
		const bool toleranceDefined = false,
		const double tolerance = 1e-5 )  ///< Initialize the FMU.
	{ 
		return init( instanceName, realVariableNames, realValues, nRealVars, 
			NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, 0, 
			startTime, lookAheadHorizon, lookAheadStepSize, 
			integratorStepSize, toleranceDefined, tolerance ); 
	}

	int init( const fmippString& instanceName,
		const fmippString realVariableNames[],
		const fmippReal* realValues,
		const fmippSize nRealVars,
		const fmippString integerVariableNames[],
		const fmippInteger* integerValues,
		const fmippSize nIntegerVars,
		const fmippString booleanVariableNames[],
		const fmippBoolean* booleanValues,
		const fmippSize nBooleanVars,
		const fmippString stringVariableNames[],
		const fmippString* stringValues,
		const fmippSize nStringVars,
		const fmippTime startTime,
		const fmippTime lookAheadHorizon,
		const fmippTime lookAheadStepSize,
		const fmippTime integratorStepSize,
		const bool toleranceDefined = false,
		const double tolerance = 1e-5 ); ///< Initialize the FMU.

	/**
	 * @brief Sets the integrator properties of the contained FMU
	 * @details The integrator properties may be set before the IncrementalFMU is
	 * initialized. In case the property structure contains some invalid values, 
	 * correct default values will be set instead. Hence, the given structure may
	 * be altered by the setter function. It is assumed that the IncrementalFMU 
	 * was successfully created before and that it is not in an erroneous state.
	 * @param prop A reference to the property structure
	 */
	void setIntegratorProperties( Integrator::Properties& prop );
	
	/**
	 * @brief Returns the currently used integrator properties.
	 * @details It is assumed that the IncrementalFMU was successfully created
	 * before.
	 */
	Integrator::Properties getIntegratorProperties() const;

	FMIPPVariableType getType( const fmippString& varName ) const;

	void defineRealInputs( const fmippString inputs[],
		const fmippSize nInputs );  ///<  Define inputs of the FMU.
	
	void defineIntegerInputs( const fmippString inputs[],
		const fmippSize nInputs );  ///<  Define inputs of the FMU.
	
	void defineBooleanInputs( const fmippString inputs[],
		const fmippSize nInputs );  ///<  Define inputs of the FMU.
	
	void defineStringInputs( const fmippString inputs[],
		const fmippSize nInputs );  ///<  Define inputs of the FMU.
	
	void defineRealOutputs( const fmippString outputs[],
		const fmippSize nOutputs ); ///< Define outputs of the FMU.
	
	void defineIntegerOutputs( const fmippString outputs[],
		const fmippSize nOutputs ); ///< Define outputs of the FMU.
	
	void defineBooleanOutputs( const fmippString outputs[],
		const fmippSize nOutputs ); ///< Define outputs of the FMU.
	
	void defineStringOutputs( const fmippString outputs[],
		const fmippSize nOutputs ); ///< Define outputs of the FMU.
	
	fmippReal* getCurrentState() const { return currentState_.state_; } ///< Get pointer to current state.

	fmippReal* getRealOutputs() const { return currentState_.realValues_; } ///< Get pointer to current outputs.

	fmippInteger* getIntegerOutputs() const { return currentState_.integerValues_; } ///< Get pointer to current outputs.

	fmippBoolean* getBooleanOutputs() const { return currentState_.booleanValues_; } ///< Get pointer to current outputs.

	fmippString* getStringOutputs() const { return currentState_.stringValues_; } ///< Get pointer to current outputs.

	fmippTime sync( fmippTime t0, fmippTime t1 ); ///< Simulate FMU from time t0 until t1.

	fmippTime sync( fmippTime t0, fmippTime t1, fmippReal* realInputs, fmippInteger* integerInputs = 0, fmippBoolean* booleanInputs = 0, fmippString* stringInputs = 0 ); ///< Simulate FMU from time t0 until t1. 

	/**
	 * \brief Update state at time t1, i.e. change the actual state using previous prediction(s). 
	 * \details In case of an event at t1, the FMU's output reflects the state before the 
	 * event occurred.
	 */
	fmippTime updateState( fmippTime t1 );

	/**
	 * \brief Updates the FMU's state to the predicted state at t1
	 * \details In case of a discontinuity at t1, the FMU's outputs will reflect 
	 * the limit from the right. The function may enhance time by the 
	 * timeDiffResulution set at construction time. The function has to be called
	 * after predictState() was called and may be used instead of updateState()
	 * which always sets the limit from the left.
	 * \return the FMU's time, if successful, INVALID_FMI_TIME otherwise
	 */
	fmippTime updateStateFromTheRight( fmippTime t1 );

	/** 
	 * \brief Sets the given inputs at the FMU and fetches the updated current state
	 * \details The function assumes that updateState() or 
	 * updateStateFromTheRight() was called before. Even if the current state 
	 * doesn't seem to be fixed beyond the current instant of time, any 
	 * predictions may have altered the current state of the FMU. Hence, an 
	 * update function must be called beforehand to fix the state and to bring 
	 * the FMU into a defined state.
	 */
	// FIXME: Consider making the function private. A sync function which 
	//        doesn't start the predictions but calls update, if necessary, may 
	//        replace the syncState function.
	void syncState( fmippTime t1, fmippReal* realInputs, fmippInteger* integerInputs, fmippBoolean* booleanInputs, fmippString* stringInputs );

	/** Compute state predictions. **/
	fmippTime predictState( fmippTime t1 );


	/** Get the status of the last operation on the FMU. **/
	fmippStatus getLastStatus() const;

	/**
	 * \brief Returns a pointer to the model description of the managed FMU
	 * \details The pointer must be valid as soon as the FMU is successfully 
	 * loaded. It may be null in case getLastStatus() indicates a non-successful 
	 * operation on constructing the FMU. The returned pointer must be valid 
	 * until the object is deleted. In order to avoid inconsistent behavior, the 
	 * model description pointer is set to constant and the model description 
	 * must not be modified.
	 */
	const ModelDescription* getModelDescription() const;

	/**
	 * Returns the initially set temporal resolution parameter
	 * @return The initially set temporal resolution parameter
	 */
	fmippTime getTimeDiffResolution() const { return timeDiffResolution_;  }

protected:

	History::History predictions_; ///< Vector of state predictions.

	/// Check the latest prediction if an event has occured. If so, update the latest prediction accordingly.
	virtual bool checkForEvent( const HistoryEntry& newestPrediction );

	/** Called in case checkForEvent() returns true. **/
	virtual void handleEvent();

	/** Set initial values for integration (i.e. for each look-ahead). **/
	virtual void initializeIntegration( HistoryEntry& initialPrediction );

	/** 
	 * @brief Define the initial inputs of the FMU
	 * @details The input states before initialization will be set. The function 
	 * will abort, if it fails on setting a value.
	 * @return The status of the operation
	 */
	fmippStatus setInitialInputs( const fmippString realVariableNames[],
		const fmippReal* realValues,
		fmippSize nRealVars,
		const fmippString integerVariableNames[],
		const fmippInteger* integerValues,
		fmippSize nIntegerVars,
		const fmippString booleanVariableNames[],
		const fmippBoolean* booleanValues,
		fmippSize nBooleanVars,
		const fmippString stringVariableNames[],
		const fmippString* stringValues,
		fmippSize nStringVars );

	/** Get the continuous state of the FMU. **/
	void getContinuousStates( fmippReal* state ) const;

	/** Set the real inputs of the FMU. **/
	fmippStatus setInputs(fmippReal* inputs) const;

	/** Set the integer inputs of the FMU. **/
	fmippStatus setInputs(fmippInteger* inputs) const;

	/** Set the boolean inputs of the FMU. **/
	fmippStatus setInputs(fmippBoolean* inputs) const;

	/** Set the string inputs of the FMU. **/
	fmippStatus setInputs(fmippString* inputs) const;

	/** Get the real outputs of the FMU. **/
	void getOutputs( fmippReal* outputs ) const;

	/** Get integer the outputs of the FMU. **/
	void getOutputs( fmippInteger* outputs ) const;

	/** Get the boolean outputs of the FMU. **/
	void getOutputs( fmippBoolean* outputs ) const;

	/** Get the string outputs of the FMU. **/
	void getOutputs( fmippString* outputs ) const;

	/** In case no look-ahead prediction is given for time t, this function is responsible to provide
	 *  an estimate for the corresponding state. For convenience, a REVERSE iterator pointing to the
	 *  next prediction available AFTER time t is handed over to the function.
	 **/
	void interpolateState(fmippTime t, History::const_reverse_iterator& historyEntry, HistoryEntry& state);

	/** Helper function: linear value interpolation. **/
	double interpolateValue( fmippReal x, fmippReal x0, fmippReal y0, fmippReal x1, fmippReal y1 ) const;

private:

	/** Interface to the FMU ME. **/
	FMUModelExchangeBase* fmu_;

	/** The current state. **/
	HistoryEntry currentState_;

	/** Value references of the real inputs. **/
	fmippValueReference* realInputRefs_;

	/** Value references of the integer inputs. **/
	fmippValueReference* integerInputRefs_;

	/** Value references of the boolean inputs. **/
	fmippValueReference* booleanInputRefs_;

	/** Value references of the string inputs. **/
	fmippValueReference* stringInputRefs_;

	/** Number of real inputs. **/
	fmippSize nRealInputs_;

	/** Number of integer inputs. **/
	fmippSize nIntegerInputs_;

	/** Number of boolean inputs. **/
	fmippSize nBooleanInputs_;

	/** Number of string inputs. **/
	fmippSize nStringInputs_;

	/** Value references of real outputs. **/
	fmippValueReference* realOutputRefs_;

	/** Value references of integer outputs. **/
	fmippValueReference* integerOutputRefs_;

	/** Value references of boolean outputs. **/
	fmippValueReference* booleanOutputRefs_;

	/** Value references of string outputs. **/
	fmippValueReference* stringOutputRefs_;

	/** Number of real outputs. **/
	fmippSize nRealOutputs_;

	/** Number integer of outputs. **/
	fmippSize nIntegerOutputs_;

	/** Number of boolean outputs. **/
	fmippSize nBooleanOutputs_;

	/** Number of string outputs. **/
	fmippSize nStringOutputs_;

	/** Look-ahead horizon. **/
	fmippTime lookAheadHorizon_;

	/** Look-ahead step size. **/
	fmippTime lookaheadStepSize_;

	/** Intergrator step size. **/
	fmippTime integratorStepSize_;

	/** Time the last event occurred **/
	fmippTime lastEventTime_;

	/** Resolution for internal time comparison. **/
	fmippTime timeDiffResolution_;

	/** Flag indicating logging on/off **/
	fmippBoolean loggingOn_;

	/** Protect default constructor. **/
	IncrementalFMU() {}

	/**
	 * @brief Instantiates the previously loaded FMU. 
	 * @details The FMU which is registered at the ModelManager with the 
	 * modelIdentifier must support the given modelType. It is assumed that no 
	 * other model was loaded and that the reference to the FMU is null. In case
	 * of an error, the return value is set appropriately. The function does not
	 * check the status of the loaded FMU. The FMU may still be in an error 
	 * state although the function returned successfully. The function will not 
	 * set any internal error state.
	 * @param modelIdentifier The ID of the previously loaded model
	 * @param modelType The type of the referenced model
	 * @param loggingOn Logging flag which is passed on to the FMU
	 * @param timeDiffResolution The event search precision which is passed on to
	 * the FMU
	 * @param integratorType The integrator to use. (Passed on to the FMU.)
	 */
	fmippStatus instantiateModelExchangeFMU(const fmippString& modelIdentifier,
		FMUType modelType,
		const fmippBoolean loggingOn,
		const fmippReal timeDiffResolution,
		const IntegratorType integratorType);

	/** Compute state at time t from previous state predictions. **/
	void getState(fmippTime t, HistoryEntry& state);

	/** Retrieve values after each integration step from FMU. **/
	void retrieveFMUState( fmippReal* result, fmippReal* realValues, fmippInteger* integerValues, fmippBoolean* booleanValues, fmippString* stringValues ) const;

};

#endif // _FMIPP_INCREMENTALFMU_H
