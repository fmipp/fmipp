/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file ModelDescription.cpp
 */

#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include "common/fmi_v1.0/fmiModelTypes.h"

#include "import/base/include/ModelDescription.h"


using namespace std;
using namespace ModelDescriptionUtilities;


////
//   Implemetntation of class ModelDescription.
////


ModelDescription::ModelDescription( const string& xmlDescriptionFilePath )
{
	read_xml( xmlDescriptionFilePath, data_ );
	isCSv1_ = hasChild( data_, "fmiModelDescription.Implementation" );
}


// Get attributes of FMI model description (FMI version, GUID, model name, etc.).
const Properties&
ModelDescription::getModelAttributes() const
{
	return data_.get_child( "<xmlattr>" );
}


// Get unit definitions.
const Properties&
ModelDescription::getUnitDefinitions() const
{
	return data_.get_child( "fmiModelDescription.UnitDefinitions" );
}


// Get type definitions.
const Properties&
ModelDescription::getTypeDefinitions() const
{
	return data_.get_child( "fmiModelDescription.TypeDefinitions" );
}


// Get description of model variables.
const Properties&
ModelDescription::getDefaultExperiment() const
{
	return data_.get_child( "fmiModelDescription.DefaultExperiment" );
}


// Get vendor annotations.
const Properties&
ModelDescription::getVendorAnnotations() const
{
	return data_.get_child( "fmiModelDescription.VendorAnnotations" );
}


// Get description of model variables.
const Properties&
ModelDescription::getModelVariables() const
{
	return data_.get_child( "fmiModelDescription.ModelVariables" );
}


// Get information concerning implementation of co-simulation tool (FMI CS feature).
const Properties&
ModelDescription::getImplementation() const
{
	return data_.get_child( "fmiModelDescription.Implementation" );
}


// Check if model description has unit definitions element.
bool
ModelDescription::hasUnitDefinitions() const
{
	return hasChild( data_, "fmiModelDescription.UnitDefinitions" );
}


// Check if model description has type definitions element.
bool
ModelDescription::hasTypeDefinitions() const
{
	return hasChild( data_, "fmiModelDescription.TypeDefinitions" );
}


// Check if model description has default experiment element.
bool
ModelDescription::hasDefaultExperiment() const
{
	return hasChild( data_, "fmiModelDescription.DefaultExperiment" );
}


// Check if model description has vendor annotations element.
bool
ModelDescription::hasVendorAnnotations() const
{
	return hasChild( data_, "fmiModelDescription.VendorAnnotations" );
}


// Check if model description has model variables element.
bool
ModelDescription::hasModelVariables() const
{
	return hasChild( data_, "fmiModelDescription.ModelVariables" );
}


// Check if model description has implementation element.
bool
ModelDescription::hasImplementation() const
{
	return hasChild( data_, "fmiModelDescription.Implementation" );
}


// Get model identifier from description.
string
ModelDescription::getModelIdentifier() const
{
	const Properties& attributes = getChildAttributes( data_, "fmiModelDescription" );
	return attributes.get<string>( "modelIdentifier" );
}


// Get GUID from description.
string
ModelDescription::getGUID() const
{
	const Properties& attributes = getChildAttributes( data_, "fmiModelDescription");
	return attributes.get<string>( "guid" );
}


// Get MIME type from description (FMI CS feature).
string
ModelDescription::getMIMEType() const
{
	string type;

	if ( false == isCSv1_ ) return type;

	if ( hasChild( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" ) )
	{
		const Properties& attributes =
			getChildAttributes( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" );

		type = attributes.get<string>( "type" );
	}

	return type;
}


// Get entry point from description (FMI CS feature).
string
ModelDescription::getEntryPoint() const
{
	string type;

	if ( false == isCSv1_ ) return type;

	if ( hasChild( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" ) )
	{
		const Properties& attributes =
			getChildAttributes( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" );

		type = attributes.get<string>( "entryPoint" );
	}

	return type;
}


// Get number of continuous states from description.
int
ModelDescription::getNumberOfContinuousStates() const
{
	const Properties& attributes = getChildAttributes( data_, "fmiModelDescription");
	return attributes.get<int>( "numberOfContinuousStates" );
}


// Get number of event indicators from description.
int
ModelDescription::getNumberOfEventIndicators() const
{
	const Properties& attributes = getChildAttributes( data_, "fmiModelDescription");
	return attributes.get<int>( "numberOfEventIndicators" );
}


// Get number of variables of type fmiReal, fmiInteger, fmiBoolean and fmiString.
void
ModelDescription::getNumberOfVariables( size_t& nReal, size_t& nInt,
					size_t& nBool, size_t& nString ) const
{
	// Define XML tags to search for.
	const string xmlRealTag( "Real" );
	const string xmlIntTag( "Integer" );
	const string xmlBoolTag( "Boolean" );
	const string xmlStringTag( "String" );

	// Reset counters.
	nReal = 0;
	nInt = 0;
	nBool = 0;
	nString = 0;

	const Properties& modelVariables = getModelVariables();

	BOOST_FOREACH( const Properties::value_type &v, modelVariables )
	{
		if ( v.second.find( xmlRealTag ) != v.second.not_found() ) { ++nReal; continue; }
		else if ( v.second.find( xmlIntTag ) != v.second.not_found() ) { ++nInt; continue; }
		else if ( v.second.find( xmlBoolTag ) != v.second.not_found() ) { ++nBool; continue; }
		else if ( v.second.find( xmlStringTag ) != v.second.not_found() ) { ++nString; continue; }
		else {
			string error( "[ModelDescription::getNumberOfVariables] unknown type: " );
			error += v.second.back().first;
			throw runtime_error( error );
		}
	}
}


////
//   Implemetntation of functionalities from namespace ModelDescriptionUtilities.
////


// Get attributes of current node.
const Properties&
ModelDescriptionUtilities::getAttributes( const Properties& p )
{
	return p.get_child( "<xmlattr>" );
}


// Get attributes of current node.
const Properties&
ModelDescriptionUtilities::getAttributes( const Properties::iterator& it )
{
 	return it->second.get_child( "<xmlattr>" );
}


// Get attributes of current node.
const Properties&
ModelDescriptionUtilities::getAttributes( const Properties::const_iterator& it )
{
 	return it->second.get_child( "<xmlattr>" );
}


// Get attributes of child node.
const Properties&
ModelDescriptionUtilities::getChildAttributes( const Properties& p,
					       const string& childName )
{
 	return p.get_child( childName ).get_child( "<xmlattr>" );
}


// Get attributes of child node.
const Properties&
ModelDescriptionUtilities::getChildAttributes( const Properties::iterator& it,
					       const string& childName )
{
 	return it->second.get_child( childName ).get_child( "<xmlattr>" );
}


// Get attributes of child node.
const Properties&
ModelDescriptionUtilities::getChildAttributes( const Properties::const_iterator& it,
					       const string& childName )
{
 	return it->second.get_child( childName ).get_child( "<xmlattr>" );
}


// Check for child node.
bool
ModelDescriptionUtilities::hasChild( const Properties& p,
				     const string& childName )
{
	boost::optional<const Properties&> c = p.get_child_optional( childName );
	return ( !c ) ? false : true;
}


// Check for child node.
bool
ModelDescriptionUtilities::hasChild( const Properties::iterator& it,
				     const string& childName )
{
	boost::optional<Properties&> c = it->second.get_child_optional( childName );
	return ( !c ) ? false : true;
}


// Check for child node.
bool
ModelDescriptionUtilities::hasChild( const Properties::const_iterator& it,
				     const string& childName )
{
	boost::optional<const Properties&> c = it->second.get_child_optional( childName );
	return ( !c ) ? false : true;
}
