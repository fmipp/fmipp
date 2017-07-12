// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FIXEDSTEPSIZEFMU_H
#define _FMIPP_FIXEDSTEPSIZEFMU_H


#include <string>

#include "common/FMIPPConfig.h"
#include "common/fmi_v1.0/fmiModelTypes.h"

#include "import/utility/include/History.h"


class FMUCoSimulationBase;


/**
 * \file FixedStepSizeFMU.h
 * \class FixedStepSizeFMU FixedStepSizeFMU.h
 * Eases the handling of FMU CS in case a fixed communication step size is enforced by the enclosed model.
 *
 * The FixedStepSizeFMU handles the proper synchronization of the FMU CS internally.
 */

class __FMI_DLL FixedStepSizeFMU
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
	FixedStepSizeFMU( const std::string& fmuDirUri,
		const std::string& modelIdentifier,
		const fmiBoolean loggingOn = fmiFalse,
		const fmiReal timeDiffResolution = 1e-4 );


	~FixedStepSizeFMU();

	int init( const std::string& instanceName,
		const std::string realVariableNames[],
		const fmiReal* realValues,
		const std::size_t nRealVars,
		const fmiTime startTime,
		const fmiTime communicationStepSize,
		const fmiBoolean stopTimeDefined = fmiFalse,
		const fmiTime stopTime = INVALID_FMI_TIME,
		const fmiReal timeout = 0.,
		const fmiBoolean visible = fmiFalse,
		const fmiBoolean interactive = fmiFalse );  ///< Initialize the FMU.

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
		const fmiTime communicationStepSize,
		const fmiBoolean stopTimeDefined = fmiFalse,
		const fmiTime stopTime = INVALID_FMI_TIME,
		const fmiReal timeout = 0.,
		const fmiBoolean visible = fmiFalse,
		const fmiBoolean interactive = fmiFalse ); ///< Initialize the FMU.


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

	/// Get pointer to current outputs.
	fmiReal* getRealOutputs() const { return currentState_.realValues_; }

	/// Get pointer to current outputs.
	fmiInteger* getIntegerOutputs() const { return currentState_.integerValues_; }

	/// Get pointer to current outputs.
	fmiBoolean* getBooleanOutputs() const { return currentState_.booleanValues_; }

	/// Get pointer to current outputs.
	std::string* getStringOutputs() const { return currentState_.stringValues_; }

	/// Simulate FMU from time t0 until t1.
	fmiTime sync( fmiTime t0, fmiTime t1 );


	/// Simulate FMU from time t0 until t1. The inputs are set at the _end_ of the interval [t0, t1].
	fmiTime sync( fmiTime t0, fmiTime t1,
		fmiReal* realInputs, fmiInteger* integerInputs,
		fmiBoolean* booleanInputs, std::string* stringInputs,
		fmiBoolean iterateOnce = fmiFalse );

	/// Iterate once at the current communication point (i.e., call doStep(...) with step size = 0).
	void iterateOnce();


	/// Get the status of the last operation on the FMU.
	fmiStatus getLastStatus() const;

protected:

	fmiReal currentCommunicationPoint_;
	fmiTime finalCommunicationPoint_;
	fmiReal communicationStepSize_;

	/** Calculate next proposed synchronization time. **/
	fmiTime getNextSyncTime( const fmiTime& currentSyncTime ) const;

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

	/** Get the integer outputs of the FMU. **/
	void getOutputs( fmiInteger* outputs ) const;

	/** Get the boolean outputs of the FMU. **/
	void getOutputs( fmiBoolean* outputs ) const;

	/** Get the string outputs of the FMU. **/
	void getOutputs( std::string* outputs ) const;

private:

	/** Interface to the CS FMU. **/
	FMUCoSimulationBase* fmu_;

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

	/** Flag indicating logging on/off **/
	fmiBoolean loggingOn_;

	/** Protect default constructor. **/
	FixedStepSizeFMU() {}

};


#endif // _FMIPP_FIXEDSTEPSIZEFMU_H
