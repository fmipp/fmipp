// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_INTERPOLATINGFIXEDSTEPSIZEFMU_H
#define _FMIPP_INTERPOLATINGFIXEDSTEPSIZEFMU_H

#include "common/FMIPPConfig.h"

#include "import/utility/include/History.h"

class FMUCoSimulationBase;

/**
 * \file InterpolatingFixedStepSizeFMU.h
 * \class InterpolatingFixedStepSizeFMU InterpolatingFixedStepSizeFMU.h
 * Eases the handling of FMU CS in case a fixed communication step size is enforced by the enclosed model.
 *
 * The FixedStepSizeFMU handles the proper synchronization of the FMU CS internally. The outputs between
 * two internal synchronizations are linearly interpolated.
 */

class __FMI_DLL InterpolatingFixedStepSizeFMU
{

public:

	/**
	 * Constructor.
	 *
	 * @param[in]  fmuDirUri  path to unzipped FMU directory (as URI)
	 * @param[in]  modelIdentifier  FMI model identifier
	 * @param[in]  loggingOn  flag for logging
	 * @param[in]  timeDiffResolution  resolution for time comparison and event search during integration
	 */
	InterpolatingFixedStepSizeFMU( const fmippString& fmuDirUri,
		const fmippString& modelIdentifier,
		const fmippBoolean loggingOn = fmippFalse,
		const fmippTime timeDiffResolution = 1e-4 );

	~InterpolatingFixedStepSizeFMU();

	int init( const fmippString& instanceName,
		  const fmippString realVariableNames[],
		  const fmippReal* realValues,
		  const fmippSize nRealVars,
		  const fmippTime startTime,
		  const fmippTime communicationStepSize,
		  const fmippBoolean stopTimeDefined = fmippFalse,
		  const fmippTime stopTime = INVALID_FMI_TIME,
		  const fmippTime timeout = 0.,
		  const fmippBoolean visible = fmippFalse,
		  const fmippBoolean interactive = fmippFalse );  ///< Initialize the FMU.

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
		  const fmippTime communicationStepSize,
		  const fmippBoolean stopTimeDefined = fmippFalse,
		  const fmippTime stopTime = INVALID_FMI_TIME,
		  const fmippTime timeout = 0.,
		  const fmippBoolean visible = fmippFalse,
		  const fmippBoolean interactive = fmippFalse ); ///< Initialize the FMU.

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

	//fmippReal* getCurrentState() const { return currentState_.state_; } ///< Get pointer to current state.

	/// Get pointer to current outputs.
	fmippReal* getRealOutputs() const { return currentState_.realValues_; }

	/// Get pointer to current outputs.
	fmippInteger* getIntegerOutputs() const { return currentState_.integerValues_; }

	/// Get pointer to current outputs.
	fmippBoolean* getBooleanOutputs() const { return currentState_.booleanValues_; }

	/// Get pointer to current outputs.
	fmippString* getStringOutputs() const { return currentState_.stringValues_; }

	/// Simulate FMU from time t0 until t1.
	fmippTime sync( fmippTime t0, fmippTime t1 );

	/// Simulate FMU from time t0 until t1. The inputs are set at the _end_ of the interval [t0, t1].
	fmippTime sync( fmippTime t0, fmippTime t1,
		fmippReal* realInputs, fmippInteger* integerInputs,
		fmippBoolean* booleanInputs, fmippString* stringInputs,
		fmippBoolean iterateOnce = fmippFalse );

	/// Iterate once at the current communication point (i.e., call doStep(...) with step size = 0).
	void iterateOnce();

	/// Get the status of the last operation on the FMU.
	fmippStatus getLastStatus() const;

protected:

	fmippTime currentCommunicationPoint_;
	fmippTime finalCommunicationPoint_;
	fmippTime communicationStepSize_;

	/** Define the initial inputs of the FMU (input states before initialization). **/
	void setInitialInputs( const fmippString realVariableNames[],
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

	/** Get the integer outputs of the FMU. **/
	void getOutputs( fmippInteger* outputs ) const;

	/** Get the boolean outputs of the FMU. **/
	void getOutputs( fmippBoolean* outputs ) const;

	/** Get the string outputs of the FMU. **/
	void getOutputs( fmippString* outputs ) const;

	/** Interpolate FMU state between two steps. **/
	void interpolateCurrentState( fmippTime t );

	/** Helper function: linear value interpolation. **/
	double interpolateValue( fmippReal x, fmippReal x0, fmippReal y0, fmippReal x1, fmippReal y1 ) const;

private:

	/** Interface to the CS FMU. **/
	FMUCoSimulationBase* fmu_;

	/** The previous state (can coincide with the current state). **/
	HistoryEntry previousState_;

	/** The current state. **/
	HistoryEntry currentState_;

	/** The next state. **/
	HistoryEntry nextState_;

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

	/** Flag indicating logging on/off **/
	fmippBoolean loggingOn_;

	/** Protect default constructor. **/
	InterpolatingFixedStepSizeFMU() {}

};

#endif // _FMIPP_INTERPOLATINGFIXEDSTEPSIZEFMU_H
