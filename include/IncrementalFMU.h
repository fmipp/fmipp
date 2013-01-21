#ifndef _IncrementalFMU_H
#define _IncrementalFMU_H

#include <string>
#include "FMIPPConfig.h"
#include "fmiModelTypes.h"
#include "HistoryBase.h"



class FMU;


class __FMI_DLL IncrementalFMU
{

public:

	IncrementalFMU( const std::string& modelName );

	IncrementalFMU( const std::string& fmuPath,
			const std::string& modelName );

	IncrementalFMU( const std::string& xmlPath,
			const std::string& dllPath,
			const std::string& modelName );


	IncrementalFMU( const std::string& name,
			const std::string inputs[],
			const std::size_t nInputs,
			const std::string outputs[],
			const std::size_t nOutputs);

	IncrementalFMU( const std::string& name,
			const std::size_t nInputs,
			const std::size_t nOutputs );

	IncrementalFMU( const IncrementalFMU& aIncrementalFMU );

	~IncrementalFMU();

	int init( const std::string& instanceName,
		  const std::string variableNames[],
		  const fmiReal* values,
		  const std::size_t nvars,
		  const fmiTime startTime,
		  const fmiTime looakaheadhorizon,
		  const fmiTime lookaheadstepsize,
		  const fmiTime integratorstepsize );

	void defineInputs( const std::string inputs[],
			   const std::size_t nInputs );

	void defineOutputs( const std::string outputs[],
			    const std::size_t nOutputs );

	fmiReal* getCurrentState() const { return currentState_.state; }

	fmiReal* getCurrentOutputs() const { return currentState_.values; }

	fmiTime sync( fmiTime t0, fmiTime t1 );

	fmiTime sync( fmiTime t0, fmiTime t1, fmiReal* inputs );


protected:

	/* A look-ahead prediction and its corresponding time are
	   stored as "History" are stored in a containers called "History" (see typedef below)*/

	typedef HistoryEntryBase HistoryEntry;
	typedef typename HistoryBase::History History;
	typedef typename HistoryBase::const_iterator History_const_iterator;
	typedef typename HistoryBase::iterator       History_iterator;
	typedef typename HistoryBase::const_reverse_iterator History_const_reverse_iterator;
	typedef typename HistoryBase::reverse_iterator       History_reverse_iterator;

	/* Vector of state predictions. */
	History predictions_;

	/* Check the latest prediction if an event has occured. If so, update the latest prediction accordingly. */
	virtual bool checkForEvent( const HistoryEntry& newestPrediction );

	/* Called in case checkForEvent() returns true. */
	virtual void handleEvent();

	/* Set initial values for integration (i.e. for each look-ahead). */
	virtual void initializeIntegration( HistoryEntry& initialPrediction );

	/* Define the initial inputs of the FMU (input states before initialization). */
	void setInitialInputs(const std::string variableNames[], const fmiReal* values, std::size_t nvars);

	/* Get the continuous state of the FMU. */
	void getContinuousStates( fmiReal* state ) const;

	/* Set the inputs of the FMU. */
	fmiStatus setInputs(fmiReal* inputs) const;

	/* Get the inputs of the FMU. */
	void getOutputs( fmiReal* outputs ) const;

	/* In case no look-ahead prediction is given for time t, this function is responsible to provide
	 * an estimate for the corresponding state. For convenience, a REVERSE iterator pointing to the
	 * next prediction available AFTER time t is handed over to the function. */
	void interpolateState(fmiTime t, History_const_reverse_iterator& historyEntry, HistoryEntry& state);

	/* Helper function: linear value interpolation. */
	double interpolateValue( fmiReal x, fmiReal x0, fmiReal y0, fmiReal x1, fmiReal y1 ) const;

private:

	/* Interface to the FMU. */
	FMU* fmu_;

	/* The current state. */
	HistoryEntry currentState_;

	/* Names of the inputs. */
	std::size_t* inputRefs_;

	/* Number of inputs. */
	std::size_t nInputs_;

	/* Names of the outputs. */
	std::size_t* outputRefs_;

	/* Number of outputs. */
	std::size_t nOutputs_;

	/* Event indicators. */
	fmiReal* eventinds_;

	/* Event indicator positions. */
	bool* eventindspos_;

	/* Look-ahead horizon. */
	fmiTime lookAheadHorizon_;

	/* Look-ahead step size. */
	fmiTime lookaheadStepSize_;

	/* Intergrator step size. */
	fmiTime integratorStepSize_;

	/* Protect default constructor. */
	IncrementalFMU() {}

	/* Compute state at time t from previous state predictions. */
	void getState(fmiTime t, HistoryEntry& state);

	/* Update state at time t1, i.e. change the actual state using previous prediction(s). */
	fmiTime updateState( fmiTime t0, fmiTime t1 );

	/* Compute state predictions. */
	fmiTime predictState( fmiTime t1 );

	/* Retrieve values after each integration step from FMU. */
	void retrieveFMUState(fmiReal* result, fmiReal* values, fmiReal* eventinds) const;

	void allocevmem();

};

#endif // _IncrementalFMU_H
