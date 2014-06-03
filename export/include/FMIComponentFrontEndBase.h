/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMI_COMPONENT_FRONT_END_BASE_H
#define _FMI_COMPONENT_FRONT_END_BASE_H

#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/FMIPPConfig.h"


/**
 * \file FMIComponentFrontEndBase.h
 * \class FMIComponentFrontEndBase FMIComponentFrontEndBase.h
 * The front end component is the actual gateway for a master algorithm to communicate and exchange
 * data with an external simulation application.
 *
 * This abstract interface is designed such that it can be easily used to implement an actual
 * FMI component (FMI model type fmiComponent), implementing functionalities close to the
 * requirements of the FMI specification, e.g., functions initializeSlave(...), doStep(...) or setReal(...).
 */ 



class __FMI_DLL FMIComponentFrontEndBase
{

public:

	/// Destructor.
	virtual ~FMIComponentFrontEndBase() {}

	///
	//  Functions for data exchange.
	///

	virtual fmiStatus setReal( const fmiValueReference& ref, const fmiReal& val ) = 0;
	virtual fmiStatus setInteger( const fmiValueReference& ref, const fmiInteger& val ) = 0;
	virtual fmiStatus setBoolean( const fmiValueReference& ref, const fmiBoolean& val ) = 0;
	virtual fmiStatus setString( const fmiValueReference& ref, const fmiString& val ) = 0;

	virtual fmiStatus getReal( const fmiValueReference& ref, fmiReal& val ) = 0;
	virtual fmiStatus getInteger( const fmiValueReference& ref, fmiInteger& val ) = 0;
	virtual fmiStatus getBoolean( const fmiValueReference& ref, fmiBoolean& val ) = 0;
	virtual fmiStatus getString( const fmiValueReference& ref, fmiString& val ) = 0;


	///
	//  Functions specific for FMI for Co-simulation.
	///

	virtual fmiStatus initializeSlave( fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop ) = 0;
	//virtual fmiStatus terminateSlave(...) = 0; /// NOT NEEDED HERE? -> fmiFunctions.cpp
	//virtual fmiStatus resetSlave(...) = 0;
	//virtual fmiStatus freeSlaveInstance(...) = 0;

	//virtual fmiStatus setRealInputDerivatives(...) = 0;
	//virtual fmiStatus getRealOutputDerivatives(...) = 0;

	virtual fmiStatus doStep( fmiReal comPoint, fmiReal stepSize, fmiBoolean newStep ) = 0;
	//virtual fmiStatus cancelStep(...) = 0;

	//virtual fmiStatus getStatus(...) = 0;
	//virtual fmiStatus getRealStatus(...) = 0;
	//virtual fmiStatus getIntegerStatus(...) = 0;
	//virtual fmiStatus getBooleanStatus(...) = 0;
	//virtual fmiStatus getStringStatus(...) = 0;

};



template<typename FrontEndType>
FMIComponentFrontEndBase*
create( const std::string& instanceName, const std::string& fmuGUID,
	const std::string& fmuLocation, const std::string& mimeType,
	fmiReal timeout, fmiBoolean visible )
{
	FrontEndType* fet = new FrontEndType( instanceName, fmuGUID,
					      fmuLocation, mimeType,
					      timeout, visible );

	return dynamic_cast<FMIComponentFrontEndBase*>( fet );
}


#endif // _FMI_COMPONENT_FRONT_END_BASE_H
