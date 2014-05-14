/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/**
 * \file ModelDescription.cpp
 */

#include <boost/property_tree/xml_parser.hpp>

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
	const Properties& attributes = getChildAttributes( data_, "fmiModelDescription" );
	isCSv1_ = hasChild( attributes, "Implementation" );
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

	if ( hasChild( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" ) )
	{
		const Properties& attributes =
			getChildAttributes( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" );

		type = attributes.get<string>( "type" );
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


// Get description of model variables.
const Properties&
ModelDescription::getModelVariables() const
{
	return data_.get_child( "fmiModelDescription.ModelVariables" );
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
