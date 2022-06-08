// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_ROLLBACKFMU_H
#define _FMIPP_ROLLBACKFMU_H

#include "import/base/include/FMUModelExchange_v1.h"
#include "import/base/include/FMUModelExchange_v2.h"

#include "import/utility/include/History.h"

/**
 * \file RollbackFMU.h 
 * \class RollbackFMU RollbackFMU.h 
 *  This class allows to perform rollbacks to times not longer
 *  ago than the previous update (or a saved internal state).
 **/

class __FMI_DLL RollbackFMU
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
	RollbackFMU( const fmippString& fmuDirUri,
		const fmippString& modelIdentifier,
		const fmippBoolean loggingOn = fmippFalse,
		const fmippReal timeDiffResolution = 1e-4,
#ifdef USE_SUNDIALS
		const IntegratorType integratorType = IntegratorType::bdf
#else
		const IntegratorType integratorType = IntegratorType::dp
#endif
	);

	virtual ~RollbackFMU();
	
	virtual fmippReal integrate( fmippReal tstop, unsigned int nsteps ); ///< Integrate internal state.
	virtual fmippReal integrate( fmippReal tstop, fmippTime deltaT = 1E-5 );  ///< Integrate internal state.

	/** Saves the current state of the FMU as internal rollback
	    state. This rollback state will not be overwritten until
	    "releaseRollbackState()" is called; **/
	void saveCurrentStateForRollback();
	
	/** Realease an internal rollback state, that was previously
	    saved via "saveCurrentStateForRollback()". **/
	void releaseRollbackState();

	/** getter functions for model variables **/

	fmippStatus getValue( const fmippString& name, fmippReal& val );
	fmippStatus getValue( const fmippString& name, fmippInteger& val );
	fmippStatus getValue( const fmippString& name, fmippBoolean& val );
	fmippStatus getValue( const fmippString& name, fmippString& val );

	/// Get single value of type fmippReal, using the variable name.
	fmippReal getRealValue( const fmippString& name );

	/// Get single value of type fmippInteger, using the variable name.
	fmippInteger getIntegerValue( const fmippString& name );

	/// Get single value of type fmippBoolean, using the variable name.
	fmippBoolean getBooleanValue( const fmippString& name );

	/// Get single value of type fmippString, using the variable name.
	fmippString getStringValue( const fmippString& name );
	
	/** setter functions for model variables **/

	fmippStatus setValue( const fmippString& name, fmippReal val );
	fmippStatus setValue( const fmippString& name, fmippInteger val );
	fmippStatus setValue( const fmippString& name, fmippBoolean val );
	fmippStatus setValue( const fmippString& name, fmippString val );

	fmippTime getTime();

	/** other functions copied from FMUModelExchangeBase **/

	fmippStatus initialize( const bool toleranceDefined = false,
		  const double tolerance = 1e-5 );
	fmippStatus instantiate( const fmippString& instanceName );

	/// Get the status of the last operation on the FMU.
	fmippStatus getLastStatus() const;

protected:

	fmippStatus rollback( fmippTime time ); ///<  Make a rollback.

private:
	/** pointer to fmu instance **/
	FMUModelExchangeBase* fmu_;

	/**  prevent calling the default constructor **/
	RollbackFMU();

	HistoryEntry rollbackState_;

	bool rollbackStateSaved_;

};


#endif // _FMIPP_ROLLBACKFMU_H
