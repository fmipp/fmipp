/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file PowerFactoryFrontEnd.cpp

#include <iostream> /// \FIXME Remove.

#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>

#if ( _MSC_VER == 1600 ) // Visual Studio 2010.

#include "Windows.h"
#include "Shlwapi.h"
#include "TCHAR.h"

#else
#error This project requires Visual Studio 2010.
#endif


#include "PowerFactoryFrontEnd.h"
#include "PowerFactoryRealScalar.h"
#include "export/include/HelperFunctions.h"


// Includes for PFSim project (advanced PowerFactory wrapper)
#include "Types.h"
#include "PowerFactory.h"


using namespace std;



PowerFactoryFrontEnd::PowerFactoryFrontEnd( const string& instanceName, const string& fmuGUID,
					    const string& fmuLocation, const string& mimeType,
					    fmiReal timeout, fmiBoolean visible )
{
	pf_ = PowerFactory::create();
	if ( 0 == pf_ ) /// \FIXME Call logger.
		throw runtime_error( "[FMIComponentFrontEnd] creation of PF API wrapper failed" );

	const string seperator( "/" );
	string fileUrl = fmuLocation + seperator + string( "modelDescription.xml" );
	ModelDescription modelDescription( HelperFunctions::getPathFromUrl( fileUrl ) );

	string projectName( "\\PFAPItest" ); /// \FIXME Parse project name from XML model description file.
	if ( pf_->Ok != pf_->activateProject( projectName ) ) /// \FIXME Call logger.
		throw runtime_error( "[FMIComponentFrontEnd] could not activate project" );

	if ( pf_->Ok != pf_->showUI( visible ) ) /// \FIXME Call logger.
		throw runtime_error( "[FMIComponentFrontEnd] could not activate project" );

	if ( modelDescription.getGUID() != fmuGUID )
		throw runtime_error( "[FMIComponentFrontEnd] Wrong GUID." ); /// \FIXME Call logger.

	size_t nRealScalars;
	size_t nIntegerScalars;
	size_t nBooleanScalars;
	size_t nStringScalars;

	// Parse number of model variables from model description.
	modelDescription.getNumberOfVariables( nRealScalars, nIntegerScalars, nBooleanScalars, nStringScalars );

	/// \FIXME Call logger.
	if ( ( 0 != nIntegerScalars ) && ( 0 != nBooleanScalars ) && ( 0 != nStringScalars ) )
		throw runtime_error( "[FMIComponentFrontEnd] only variables of type 'fmiReal' supported" );

	initializeVariables( modelDescription );
}


PowerFactoryFrontEnd::~PowerFactoryFrontEnd()
{
	if ( pf_->Ok != pf_->deactivateProject() ) /// \FIXME Call logger.
		throw runtime_error( "[FMIComponentFrontEnd] deactivation of project failed" );

	if ( pf_->Ok != pf_->execute( "exit" ) ) /// \FIXME Call logger.
		throw runtime_error( "[FMIComponentFrontEnd] exiting failed" );

	BOOST_FOREACH( RealMap::value_type& v, realScalarMap_ )
		delete v.second;
}


fmiStatus
PowerFactoryFrontEnd::setReal( const fmiValueReference& ref, const fmiReal& val )
{
	// Search for value reference.
	RealMap::const_iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		/// \FIXME Call function logger.
		return fmiWarning;
	}

	const PowerFactoryRealScalar* scalar = itFind->second;
	// Check if scalar is defined as input.
	if ( scalar->causality_ != ScalarVariableAttributes::input )
	{
		/// \FIXME Call function logger.
		return fmiWarning;
	}

	api::DataObject* dataObj = 0;
	if ( pf_->getCalcRelevantObject( scalar->className_, scalar->objectName_, dataObj ) == pf_->Ok )
	{
		if ( pf_->setAttributeDouble( dataObj, scalar->parameterName_.c_str(), val ) == pf_->Ok ) 
		{
			return fmiOK;
		}
	}

	return fmiFatal;
}


fmiStatus
PowerFactoryFrontEnd::setInteger( const fmiValueReference& ref, const fmiInteger& val )
{
	return fmiFatal;
}


fmiStatus PowerFactoryFrontEnd::setBoolean( const fmiValueReference& ref, const fmiBoolean& val )
{
	return fmiFatal;
}


fmiStatus PowerFactoryFrontEnd::setString( const fmiValueReference& ref, const fmiString& val )
{
	return fmiFatal;
}


fmiStatus
PowerFactoryFrontEnd::getReal( const fmiValueReference& ref, fmiReal& val )
{
	// Search for value reference.
	RealMap::const_iterator itFind = realScalarMap_.find( ref );

	// Check if scalar according to the value reference exists.
	if ( itFind == realScalarMap_.end() )
	{
		/// \FIXME Call function logger.
		val = 0;
		return fmiWarning;
	}

	const PowerFactoryRealScalar* scalar = itFind->second;
	api::DataObject* dataObj = 0;
	if ( pf_->getCalcRelevantObject( scalar->className_, scalar->objectName_, dataObj ) == pf_->Ok )
	{
		if ( pf_->getAttributeDouble( dataObj, scalar->parameterName_.c_str(), val ) == pf_->Ok ) 
		{
			return fmiOK;
		}
	}

	return fmiFatal;
}


fmiStatus
PowerFactoryFrontEnd::getInteger( const fmiValueReference& ref, fmiInteger& val )
{
	return fmiFatal;
}


fmiStatus PowerFactoryFrontEnd::getBoolean( const fmiValueReference& ref, fmiBoolean& val )
{
	return fmiFatal;
}


fmiStatus PowerFactoryFrontEnd::getString( const fmiValueReference& ref, fmiString& val )
{
	return fmiFatal;
}


fmiStatus
PowerFactoryFrontEnd::initializeSlave( fmiReal tStart, fmiBoolean StopTimeDefined, fmiReal tStop )
{
	return fmiOK;
}


fmiStatus
PowerFactoryFrontEnd::resetSlave()
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
PowerFactoryFrontEnd::setRealInputDerivatives( const fmiValueReference vr[], size_t nvr,
					       const fmiInteger order[], const fmiReal value[])
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
PowerFactoryFrontEnd::getRealOutputDerivatives( const fmiValueReference vr[], size_t nvr,
						const fmiInteger order[], fmiReal value[])
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
PowerFactoryFrontEnd::doStep( fmiReal comPoint, fmiReal stepSize, fmiBoolean newStep )
{
	/// \FIXME: RMS simulation?
	return ( pf_->calculatePowerFlow() == pf_->Ok ) ? fmiOK : fmiFatal;
}


fmiStatus
PowerFactoryFrontEnd::cancelStep()
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
PowerFactoryFrontEnd::getStatus( const fmiStatusKind s, fmiStatus* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
PowerFactoryFrontEnd::getRealStatus( const fmiStatusKind s, fmiReal* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
PowerFactoryFrontEnd::getIntegerStatus( const fmiStatusKind s, fmiInteger* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
PowerFactoryFrontEnd::getBooleanStatus( const fmiStatusKind s, fmiBoolean* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


fmiStatus
PowerFactoryFrontEnd::getStringStatus( const fmiStatusKind s, fmiString* value )
{
	return fmiFatal; /// \FIXME Replace dummy implementation.
}


void
PowerFactoryFrontEnd::initializeVariables( const ModelDescription& modelDescription )
{
	const ModelDescription::Properties& modelVariables = modelDescription.getModelVariables();

	PowerFactoryRealScalar* scalar;

	BOOST_FOREACH( const ModelDescription::Properties::value_type &v, modelVariables )
	{
		scalar = new PowerFactoryRealScalar;
		initializeScalar( scalar, v.second );
		realScalarMap_[scalar->valueReference_] = scalar;
	}
}


void
PowerFactoryFrontEnd::initializeScalar( PowerFactoryRealScalar* scalar,
					const ModelDescription::Properties& description ) const
{
	using namespace ScalarVariableAttributes;
	using namespace ModelDescriptionUtilities;

	const Properties& attributes = getAttributes( description );

	bool parseStatus = parseFMIVariableName( attributes.get<string>( "name" ),
						 scalar->className_, scalar->objectName_, scalar->parameterName_ );

	/// \FIXME Use logger.
	if ( false == parseStatus )
		cerr << "[PowerFactoryFrontEnd] Bad variable name: "
		     << attributes.get<string>( "name" ) << endl;

	scalar->valueReference_ = attributes.get<int>( "valueReference" );
	scalar->causality_ = getCausality( attributes.get<string>( "causality" ) );
	scalar->variability_ = getVariability( attributes.get<string>( "variability" ) );

	/// \FIXME Replace try/catch with 'optional' get.
	try { // Throws in case there are no xml attributes defined.
		// This wrapper handles only variables of type 'fmiReal'!
		const Properties& properties = getChildAttributes( description, "Real" );

		if ( properties.find( "start" ) != properties.not_found() ) {

			// Check if scalar is defined as input.
			if ( scalar->causality_ != ScalarVariableAttributes::input ) {
				cerr << "[PowerFactoryFrontEnd] Not an input: "
				     << attributes.get<string>( "name" ) << endl; /// \FIXME Use logger.
			}
 
			api::DataObject* dataObj = 0;
			int check = -1;
			check = pf_->getCalcRelevantObject( scalar->className_, scalar->objectName_, dataObj );
			if ( check != pf_->Ok )
			{
				cerr << "[PowerFactoryFrontEnd] Unable to get object: "
				     << attributes.get<string>( "name" ) << endl; /// \FIXME Use logger.
			} else {
				fmiReal start = properties.get<fmiReal>( "start" );
				check = pf_->setAttributeDouble( dataObj, scalar->parameterName_.c_str(), start );
				if (  check != pf_->Ok ) 
				{
					cerr << "[PowerFactoryFrontEnd] Unable to set attribute: "
					     << attributes.get<string>( "name" ) << endl; /// \FIXME Use logger.
				}
			}
		}

	} catch ( ... ) {} // Do nothing ...

	/// \FIXME What about the remaining properties?
}


bool
PowerFactoryFrontEnd::parseFMIVariableName( const string& name,
					    string& className,
					    string& objectName,
					    string& parameterName ) const
{
	using namespace boost;

	vector<string> strs;
	boost::split( strs, name, is_any_of(".") );

	if ( 3 == strs.size() )
	{
		className = algorithm::trim_copy( strs[0] );
		objectName = algorithm::trim_copy( strs[1] );
		parameterName = algorithm::trim_copy( strs[2] );
		return true;
	}

	return false;
}
