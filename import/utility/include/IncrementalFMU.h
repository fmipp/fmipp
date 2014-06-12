/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_INCREMENTALFMU_H
#define _FMIPP_INCREMENTALFMU_H


#include <cstdio>
#include <string>

#include "common/FMIPPConfig.h"
#include "common/FMIType.h"

#include "common/fmi_v1.0/fmiModelTypes.h"

#include "import/utility/include/History.h"


class FMUModelExchangeBase;


/**
 * \file IncrementalFMU.h 
 * \class IncrementalFMU IncrementalFMU.h 
 * The FMI++ library offers the possibility to combine the basic ability to integrate
 * the state of an FMU with advanced event handling capabilities.
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

	IncrementalFMU( const std::string& fmuPath,
			const std::string& modelName,
			const fmiReal eventSearchPrecision = 1e-4 );

	IncrementalFMU( const std::string& xmlPath,
			const std::string& dllPath,
			const std::string& modelName,
			const fmiReal eventSearchPrecision = 1e-4 );

	IncrementalFMU( const IncrementalFMU& aIncrementalFMU );

	~IncrementalFMU();

	int init( const std::string& instanceName,
		  const std::string realVariableNames[],
		  const fmiReal* realValues,
		  const std::size_t nRealVars,
		  const fmiTime startTime,
		  const fmiTime lookAheadHorizon,
		  const fmiTime lookAheadStepSize,
		  const fmiTime integratorStepSize )  ///< Initialize the FMU.
	{ return init( instanceName, realVariableNames, realValues, nRealVars, NULL, NULL, 0, NULL, NULL, 0, NULL, NULL, 0, startTime, lookAheadHorizon, lookAheadStepSize, integratorStepSize ); }

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
		  const fmiTime integratorStepSize ); ///< Initialize the FMU.

	
	FMIType getType( const std::string& varName ) const;

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

	fmiTime sync( fmiTime t0, fmiTime t1, fmiReal* realInputs, fmiInteger* integerInputs, fmiBoolean* booleanInputs, std::string* stringInputs ); ///< Simulate FMU from time t0 until t1. 

	/** Update state at time t1, i.e. change the actual state using previous prediction(s). **/
	fmiTime updateState( fmiTime t1 );

	/** Sync state according to the current inputs **/
	void syncState( fmiTime t1, fmiReal* realInputs, fmiInteger* integerInputs, fmiBoolean* booleanInputs, std::string* stringInputs );

	/** Compute state predictions. **/
	fmiTime predictState( fmiTime t1 );

protected:

	typedef History::History History;
	typedef History::const_iterator History_const_iterator;
	typedef History::iterator       History_iterator;
	typedef History::const_reverse_iterator History_const_reverse_iterator;
	typedef History::reverse_iterator       History_reverse_iterator;

	History predictions_; ///< Vector of state predictions.

	/// Resolution for internal time comparison.
	/// \FIXME Is this (nanosecond) resolution reasonable?
	static double timeDiffResolution_;

	/// Check the latest prediction if an event has occured. If so, update the latest prediction accordingly.
	virtual bool checkForEvent( const HistoryEntry& newestPrediction );

	/** Called in case checkForEvent() returns true. **/
	virtual void handleEvent();

	/** Set initial values for integration (i.e. for each look-ahead). **/
	virtual void initializeIntegration( HistoryEntry& initialPrediction );

	/** Define the initial inputs of the FMU (input states before initialization). **/
	void setInitialInputs( const std::string realVariableNames[],
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

	/** Set the inputs of the FMU. **/
	fmiStatus setInputs(fmiReal* inputs) const;

	fmiStatus setInputs(fmiInteger* inputs) const;

	fmiStatus setInputs(fmiBoolean* inputs) const;

	fmiStatus setInputs(std::string* inputs) const;

	/** Get the inputs of the FMU. **/
	void getOutputs( fmiReal* outputs ) const;

	void getOutputs( fmiInteger* outputs ) const;

	void getOutputs( fmiBoolean* outputs ) const;

	void getOutputs( std::string* outputs ) const;

	/** In case no look-ahead prediction is given for time t, this function is responsible to provide
	 *  an estimate for the corresponding state. For convenience, a REVERSE iterator pointing to the
	 *  next prediction available AFTER time t is handed over to the function.
	 **/
	void interpolateState(fmiTime t, History_const_reverse_iterator& historyEntry, HistoryEntry& state);

	/** Helper function: linear value interpolation. **/
	double interpolateValue( fmiReal x, fmiReal x0, fmiReal y0, fmiReal x1, fmiReal y1 ) const;

private:

	/** Interface to the FMU ME. **/
	FMUModelExchangeBase* fmu_;

	/** The current state. **/
	HistoryEntry currentState_;

	/** Names of the inputs. **/
	std::size_t* realInputRefs_;

	std::size_t* integerInputRefs_;

	std::size_t* booleanInputRefs_;

	std::size_t* stringInputRefs_;

	/** Number of inputs. **/
	std::size_t nRealInputs_;

	std::size_t nIntegerInputs_;

	std::size_t nBooleanInputs_;

	std::size_t nStringInputs_;

	/** Names of the outputs. **/
	std::size_t* realOutputRefs_;

	std::size_t* integerOutputRefs_;

	std::size_t* booleanOutputRefs_;

	std::size_t* stringOutputRefs_;

	/** Number of outputs. **/
	std::size_t nRealOutputs_;

	std::size_t nIntegerOutputs_;

	std::size_t nBooleanOutputs_;

	std::size_t nStringOutputs_;

	/** Look-ahead horizon. **/
	fmiTime lookAheadHorizon_;

	/** Look-ahead step size. **/
	fmiTime lookaheadStepSize_;

	/** Intergrator step size. **/
	fmiTime integratorStepSize_;

	/** Time the last event occurred **/
	fmiTime lastEventTime_;

	/** Protect default constructor. **/
	IncrementalFMU() {}

	/** Compute state at time t from previous state predictions. **/
	void getState(fmiTime t, HistoryEntry& state);

	/** Retrieve values after each integration step from FMU. **/
	void retrieveFMUState( fmiReal* result, fmiReal* realValues, fmiInteger* integerValues, fmiBoolean* booleanValues, std::string* stringValues ) const;

};


#endif // _FMIPP_INCREMENTALFMU_H
