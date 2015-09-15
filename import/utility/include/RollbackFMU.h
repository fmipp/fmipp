/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

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

	// RollbackFMU( const std::string& modelName );

	RollbackFMU( const std::string& fmuPath,
		     const std::string& modelName );

	RollbackFMU( const std::string& xmlPath,
		     const std::string& dllPath,
		     const std::string& modelName );

	~RollbackFMU();

	
	virtual fmiReal integrate( fmiReal tstop, unsigned int nsteps ); ///< Integrate internal state.
	virtual fmiReal integrate( fmiReal tstop, double deltaT = 1E-5 );  ///< Integrate internal state.

	/** Saves the current state of the FMU as internal rollback
	    state. This rollback state will not be overwritten until
	    "releaseRollbackState()" is called; **/
	void saveCurrentStateForRollback();
	
	/** Realease an internal rollback state, that was previously
	    saved via "saveCurrentStateForRollback()". **/
	void releaseRollbackState();

	/** getter functions for model variables **/

	fmiStatus getValue( const std::string& name, fmiReal& val );
	fmiStatus getValue( const std::string& name, fmiInteger& val );
	fmiStatus getValue( const std::string& name, fmiBoolean& val );
	fmiStatus getValue( const std::string& name, std::string& val );

	/** setter functions for model variables **/

	fmiStatus setValue( const std::string& name, fmiReal val );
	fmiStatus setValue( const std::string& name, fmiInteger val );
	fmiStatus setValue( const std::string& name, fmiBoolean val );
	fmiStatus setValue( const std::string& name, std::string val );

	fmiTime   getTime();

	/** other functions copied from FMUModelExchangeBase **/

	fmiStatus initialize();
	fmiStatus instantiate( const std::string& instanceName );

protected:

	fmiStatus rollback( fmiTime time ); ///<  Make a rollback.

private:
	/** pointer to fmu instance **/
	FMUModelExchangeBase* fmu_;

	/**  prevent calling the default constructor **/
	RollbackFMU();

	HistoryEntry rollbackState_;

	bool rollbackStateSaved_;

};


#endif // _FMIPP_ROLLBACKFMU_H
