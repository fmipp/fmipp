// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_INCREMENTALFMU_H
#define _FMIPP_INCREMENTALFMU_H


#include <cstdio>
#include <string>

#include "common/FMIPPConfig.h"
#include "common/FMIVariableType.h"
#include "common/FMUType.h"

#include "common/fmi_v1.0/fmiModelTypes.h"

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
	IncrementalFMU( const std::string& fmuDirUri,
			const std::string& modelIdentifier,
			const fmiBoolean loggingOn = fmiFalse,
			const fmiReal timeDiffResolution = 1e-4,
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
	IncrementalFMU( const std::string& modelIdentifier,
			const fmiBoolean loggingOn = fmiFalse,
			const fmiReal timeDiffResolution = 1e-4,
#ifdef USE_SUNDIALS
			const IntegratorType integratorType = IntegratorType::bdf
#else
			const IntegratorType integratorType = IntegratorType::dp
#endif
			  );


	~IncrementalFMU();

	int init( const std::string& instanceName,
		  const std::string realVariableNames[],
		  const fmiReal* realValues,
		  const std::size_t nRealVars,
		  const fmiTime startTime,
		  const fmiTime lookAheadHorizon,
		  const fmiTime lookAheadStepSize,
		  const fmiTime integratorStepSize,
		  const bool toleranceDefined = false,
		  const double tolerance = 1e-5 )  ///< Initialize the FMU.
	{ return init( instanceName, realVariableNames, realValues, nRealVars, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, 0, startTime, lookAheadHorizon, lookAheadStepSize, integratorStepSize, toleranceDefined, tolerance ); }

	int init( const std::string& instanceName,
		  const std::string realVariableNames[],
		  const fmiReal* realValues,
		  const std::size_t nRealVars,
		  const std::string integerVariableNames[],
		  const fmiInteger* integerValues,
		  const std::size_t nIntegerVars,
		  const std::string booleanVariableNames[],
		  const fmiBoolean* booleanValues,
		  const std::size_t nBooleanVars,
		  const std::string stringVariableNames[],
		  const std::string* stringValues,
		  const std::size_t nStringVars,
		  const fmiTime startTime,
		  const fmiTime lookAheadHorizon,
		  const fmiTime lookAheadStepSize,
		  const fmiTime integratorStepSize,
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

	FMIVariableType getType( const std::string& varName ) const;

	void defineRealInputs( const std::string inputs[],
			   const std::size_t nInputs );  ///<  Define inputs of the FMU.
	
	void defineIntegerInputs( const std::string inputs[],
			   const std::size_t nInputs );  ///<  Define inputs of the FMU.
	
	void defineBooleanInputs( const std::string inputs[],
			   const std::size_t nInputs );  ///<  Define inputs of the FMU.
	
	void defineStringInputs( const std::string inputs[],
			   const std::size_t nInputs );  ///<  Define inputs of the FMU.
	
	void defineRealOutputs( const std::string outputs[],
			    const std::size_t nOutputs ); ///< Define outputs of the FMU.
	
	void defineIntegerOutputs( const std::string outputs[],
			    const std::size_t nOutputs ); ///< Define outputs of the FMU.
	
	void defineBooleanOutputs( const std::string outputs[],
			    const std::size_t nOutputs ); ///< Define outputs of the FMU.
	
	void defineStringOutputs( const std::string outputs[],
			    const std::size_t nOutputs ); ///< Define outputs of the FMU.
	
	fmiReal* getCurrentState() const { return currentState_.state_; } ///< Get pointer to current state.

	fmiReal* getRealOutputs() const { return currentState_.realValues_; } ///< Get pointer to current outputs.

	fmiInteger* getIntegerOutputs() const { return currentState_.integerValues_; } ///< Get pointer to current outputs.

	fmiBoolean* getBooleanOutputs() const { return currentState_.booleanValues_; } ///< Get pointer to current outputs.

	std::string* getStringOutputs() const { return currentState_.stringValues_; } ///< Get pointer to current outputs.

	fmiTime sync( fmiTime t0, fmiTime t1 ); ///< Simulate FMU from time t0 until t1.

	fmiTime sync( fmiTime t0, fmiTime t1, fmiReal* realInputs, fmiInteger* integerInputs = 0, fmiBoolean* booleanInputs = 0, std::string* stringInputs = 0 ); ///< Simulate FMU from time t0 until t1. 

	/**
	 * \brief Update state at time t1, i.e. change the actual state using previous prediction(s). 
	 * \details In case of an event at t1, the FMU's output reflects the state before the 
	 * event occurred.
	 */
	fmiTime updateState( fmiTime t1 );

	/**
	 * \brief Updates the FMU's state to the predicted state at t1
	 * \details In case of a discontinuity at t1, the FMU's outputs will reflect 
	 * the limit from the right. The function may enhance time by the 
	 * timeDiffResulution set at construction time. The function has to be called
	 * after predictState() was called and may be used instead of updateState()
	 * which always sets the limit from the left.
	 * \return the FMU's time, if successful, INVALID_FMI_TIME otherwise
	 */
	fmiTime updateStateFromTheRight( fmiTime t1 );

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
	void syncState( fmiTime t1, fmiReal* realInputs, fmiInteger* integerInputs, fmiBoolean* booleanInputs, std::string* stringInputs );

	/** Compute state predictions. **/
	fmiTime predictState( fmiTime t1 );


	/** Get the status of the last operation on the FMU. **/
	fmiStatus getLastStatus() const;

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
	fmiTime getTimeDiffResolution() const { return timeDiffResolution_;  }

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
	fmiStatus setInitialInputs( const std::string realVariableNames[],
						   const fmiReal* realValues,
						   std::size_t nRealVars,
						   const std::string integerVariableNames[],
						   const fmiInteger* integerValues,
						   std::size_t nIntegerVars,
						   const std::string booleanVariableNames[],
						   const fmiBoolean* booleanValues,
						   std::size_t nBooleanVars,
						   const std::string stringVariableNames[],
						   const std::string* stringValues,
						   std::size_t nStringVars );

	/** Get the continuous state of the FMU. **/
	void getContinuousStates( fmiReal* state ) const;

	/** Set the real inputs of the FMU. **/
	fmiStatus setInputs(fmiReal* inputs) const;

	/** Set the integer inputs of the FMU. **/
	fmiStatus setInputs(fmiInteger* inputs) const;

	/** Set the boolean inputs of the FMU. **/
	fmiStatus setInputs(fmiBoolean* inputs) const;

	/** Set the string inputs of the FMU. **/
	fmiStatus setInputs(std::string* inputs) const;

	/** Get the real outputs of the FMU. **/
	void getOutputs( fmiReal* outputs ) const;

	/** Get integer the outputs of the FMU. **/
	void getOutputs( fmiInteger* outputs ) const;

	/** Get the boolean outputs of the FMU. **/
	void getOutputs( fmiBoolean* outputs ) const;

	/** Get the string outputs of the FMU. **/
	void getOutputs( std::string* outputs ) const;

	/** In case no look-ahead prediction is given for time t, this function is responsible to provide
	 *  an estimate for the corresponding state. For convenience, a REVERSE iterator pointing to the
	 *  next prediction available AFTER time t is handed over to the function.
	 **/
	void interpolateState(fmiTime t, History::const_reverse_iterator& historyEntry, HistoryEntry& state);

	/** Helper function: linear value interpolation. **/
	double interpolateValue( fmiReal x, fmiReal x0, fmiReal y0, fmiReal x1, fmiReal y1 ) const;

private:

	/** Interface to the FMU ME. **/
	FMUModelExchangeBase* fmu_;

	/** The current state. **/
	HistoryEntry currentState_;

	/** Value references of the real inputs. **/
	fmiValueReference* realInputRefs_;

	/** Value references of the integer inputs. **/
	fmiValueReference* integerInputRefs_;

	/** Value references of the boolean inputs. **/
	fmiValueReference* booleanInputRefs_;

	/** Value references of the string inputs. **/
	fmiValueReference* stringInputRefs_;

	/** Number of real inputs. **/
	std::size_t nRealInputs_;

	/** Number of integer inputs. **/
	std::size_t nIntegerInputs_;

	/** Number of boolean inputs. **/
	std::size_t nBooleanInputs_;

	/** Number of string inputs. **/
	std::size_t nStringInputs_;

	/** Value references of real outputs. **/
	fmiValueReference* realOutputRefs_;

	/** Value references of integer outputs. **/
	fmiValueReference* integerOutputRefs_;

	/** Value references of boolean outputs. **/
	fmiValueReference* booleanOutputRefs_;

	/** Value references of string outputs. **/
	fmiValueReference* stringOutputRefs_;

	/** Number of real outputs. **/
	std::size_t nRealOutputs_;

	/** Number integer of outputs. **/
	std::size_t nIntegerOutputs_;

	/** Number of boolean outputs. **/
	std::size_t nBooleanOutputs_;

	/** Number of string outputs. **/
	std::size_t nStringOutputs_;

	/** Look-ahead horizon. **/
	fmiTime lookAheadHorizon_;

	/** Look-ahead step size. **/
	fmiTime lookaheadStepSize_;

	/** Intergrator step size. **/
	fmiTime integratorStepSize_;

	/** Time the last event occurred **/
	fmiTime lastEventTime_;

	/** Resolution for internal time comparison. **/
	fmiTime timeDiffResolution_;

	/** Flag indicating logging on/off **/
	fmiBoolean loggingOn_;

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
	fmiStatus instantiateModelExchangeFMU(const std::string& modelIdentifier,
		FMUType modelType,
		const fmiBoolean loggingOn,
		const fmiReal timeDiffResolution,
		const IntegratorType integratorType);

	/** Compute state at time t from previous state predictions. **/
	void getState(fmiTime t, HistoryEntry& state);

	/** Retrieve values after each integration step from FMU. **/
	void retrieveFMUState( fmiReal* result, fmiReal* realValues, fmiInteger* integerValues, fmiBoolean* booleanValues, std::string* stringValues ) const;

};


#endif // _FMIPP_INCREMENTALFMU_H
