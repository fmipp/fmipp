// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

/**
 * \file ModelDescription.cpp
 */

#include <algorithm>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include "import/base/include/ModelDescription.h"
#include "import/base/include/PathFromUrl.h"

using namespace std;
using namespace ModelDescriptionUtilities;

//
// Implementation of class ModelDescription.
// 

ModelDescription::ModelDescription( const fmippString& xmlDescriptionFilePath )
{
	try {
		using namespace boost::property_tree::xml_parser;
		read_xml( xmlDescriptionFilePath, data_, trim_whitespace | no_comments );
	} catch( ... ) {
		isValid_ = fmippFalse;
		return;
	}

	try {
		// Sanity check.
		isValid_ = hasChild( data_, "fmiModelDescription" );
	} catch ( ... ) {
		isValid_ = fmippFalse;
		return;
	}

	detectFMUType();
}

ModelDescription::ModelDescription( const fmippString& modelDescriptionURL, fmippBoolean& isValid )
{
	isValid = fmippFalse;
	fmippString xmlDescriptionFilePath;
	isValid_ = PathFromUrl::getPathFromUrl( modelDescriptionURL, xmlDescriptionFilePath );
	if ( !isValid_ )
		return;

	try {
		using namespace boost::property_tree::xml_parser;
		read_xml( xmlDescriptionFilePath, data_, trim_whitespace | no_comments );
	} catch( ... ) {
		isValid_ = fmippFalse;
		return;
	}

	try {
		// Sanity check.
		isValid_ = hasChild( data_, "fmiModelDescription" );
	} catch ( ... ) {
		isValid_ = fmippFalse;
		return;
	}

	detectFMUType();
	
	isValid = isValid_;
}


// Check if XML model description file has been parsed successfully.
fmippBoolean
ModelDescription::isValid() const
{
	return isValid_;
}


// Get attributes of FMI model description (FMI version, GUID, model name, etc.).
const Properties&
ModelDescription::getModelAttributes() const
{
	return data_.get_child( "fmiModelDescription.<xmlattr>" );
}


/// Get specific description for ModelExchange (FMI 2.0).
const Properties&
ModelDescription::getModelExchange() const
{
	return data_.get_child( "fmiModelDescription.ModelExchange" );
}


/// Get specific description for CoSimulation (FMI 2.0).
const Properties&
ModelDescription::getCoSimulation() const
{
	return data_.get_child( "fmiModelDescription.CoSimulation" );
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


// Get the available entries from the defaultExperiment node.
const void
ModelDescription::getDefaultExperiment( fmippTime& startTime, fmippTime& stopTime,
	fmippTime& tolerance, fmippTime& stepSize) const
{
	// return tolerance = inf if tolerance is not available, etc.
	startTime = stopTime = tolerance = stepSize = std::numeric_limits<fmippTime>::quiet_NaN();

	// return here if the fmu version is 1.0 ???

	// get the attributes since there are no other childs of DefaultExperient
	// documented in the fmi standard
	Properties defaultExperiment = getChildAttributes( data_, "fmiModelDescription.DefaultExperiment" );

	// read the childattributes defined in the documentation
	if ( hasChild( defaultExperiment, "startTime" ) )
		startTime = defaultExperiment.get<fmippTime>( "startTime" );
	if ( hasChild( defaultExperiment, "stopTime" ) )
		stopTime = defaultExperiment.get<fmippTime>( "stopTime" );
	if ( hasChild( defaultExperiment, "tolerance" ) )
		tolerance = defaultExperiment.get<fmippTime>( "tolerance" );
	if ( hasChild( defaultExperiment, "stepsize" ) )
		 stepSize = defaultExperiment.get<fmippTime>( "stepsize" );
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


// Get the verion of the FMU (1.0 or 2.0) as integer.
const int
ModelDescription::getVersion() const
{
	int version = -1;
	
	switch ( fmuType_ ) {
		case fmi_1_0_me:
		case fmi_1_0_cs:
			version = 1;
			break;
		case fmi_2_0_me:
		case fmi_2_0_cs:
		case fmi_2_0_me_and_cs:
			version = 2;
			break;
		case invalid:
			break;
	}

	return version;
}


/// Check if model description has ModelExchange element.
fmippBoolean
ModelDescription::hasModelExchange() const
{
	return hasChild( data_, "fmiModelDescription.ModelExchange" );
}


/// Check if model description has CoSimulation element.
fmippBoolean
ModelDescription::hasCoSimulation() const
{
	return hasChild( data_, "fmiModelDescription.CoSimulation" );
}


// Check if model description has unit definitions element.
fmippBoolean
ModelDescription::hasUnitDefinitions() const
{
	return hasChild( data_, "fmiModelDescription.UnitDefinitions" );
}


// Check if model description has type definitions element.
fmippBoolean
ModelDescription::hasTypeDefinitions() const
{
	return hasChild( data_, "fmiModelDescription.TypeDefinitions" );
}


// Check if model description has default experiment element.
fmippBoolean
ModelDescription::hasDefaultExperiment() const
{
	return hasChildAttributes( data_, "fmiModelDescription.DefaultExperiment" );
}


// Check if model description has vendor annotations element.
fmippBoolean
ModelDescription::hasVendorAnnotations() const
{
	return hasChild( data_, "fmiModelDescription.VendorAnnotations" );
}


// Check if model description has model variables element.
fmippBoolean
ModelDescription::hasModelVariables() const
{
	return hasChild( data_, "fmiModelDescription.ModelVariables" );
}


// Check if a Jacobian can be computed
fmippBoolean
ModelDescription::providesJacobian() const
{
	if ( 1 == getVersion() ) return fmippFalse;
	// if the flag providesDirectionalDerivative exists, and is "true", return fmippTrue...
	const Properties& attributes = getChildAttributes( data_, "fmiModelDescription.ModelExchange" );
	if ( hasChild( attributes, "providesDirectionalDerivative" ) ){
		if ( attributes.get<fmippString>( "providesDirectionalDerivative" ) == "true" )
			return fmippTrue;
	}
	// ...otherwise return fmippFalse
	return fmippFalse;
}


// Check if model description has implementation element.
fmippBoolean
ModelDescription::hasImplementation() const
{
	return hasChild( data_, "fmiModelDescription.Implementation" );
}


// Check if model description has element VerndorAnnotations with nested element Tool.
fmippBoolean
ModelDescription::hasVendorAnnotationsTool() const
{
	return hasChild( data_, "fmiModelDescription.VendorAnnotations.Tool" );
}


// Get model identifier from description.
vector<fmippString>
ModelDescription::getModelIdentifier() const
{
	if ( ( fmuType_ == fmi_1_0_me ) || ( fmuType_ == fmi_1_0_cs ) )
	{
		const Properties& attributes = getChildAttributes( data_, "fmiModelDescription" );
		return vector<fmippString>( 1, attributes.get<fmippString>( "modelIdentifier" ) );
	}
	else if ( fmuType_ == fmi_2_0_me )
	{
		const Properties& attributes = getChildAttributes( data_, "fmiModelDescription.ModelExchange" );
		return vector<fmippString>( 1, attributes.get<fmippString>( "modelIdentifier" ) );
	}
	else if ( fmuType_ == fmi_2_0_cs )
	{
		const Properties& attributes = getChildAttributes( data_, "fmiModelDescription.CoSimulation" );
		return vector<fmippString>( 1, attributes.get<fmippString>( "modelIdentifier" ) );
	}
	else if ( fmuType_ == fmi_2_0_me_and_cs )
	{
		vector<fmippString> res( 2 );

		const Properties& attributesME = getChildAttributes( data_, "fmiModelDescription.ModelExchange" );
		res[0] = attributesME.get<fmippString>( "modelIdentifier" );

		const Properties& attributesCS = getChildAttributes( data_, "fmiModelDescription.CoSimulation" );
		res[1] = attributesCS.get<fmippString>( "modelIdentifier" );
		
		return res;
	}
	
	return vector<fmippString>();
}

fmippBoolean 
ModelDescription::hasModelIdentifier(const fmippString& modelIdentifier) const
{
	std::vector<fmippString> ids = getModelIdentifier();
	auto it = std::find( ids.begin(), ids.end(), modelIdentifier );
	return (it != ids.end());
}

// Get GUID from description.
fmippString
ModelDescription::getGUID() const
{
	const Properties& attributes = getChildAttributes( data_, "fmiModelDescription");
	return attributes.get<fmippString>( "guid" );
}


// Get MIME type from description (FMI CS feature).
fmippString
ModelDescription::getMIMEType() const
{
	fmippString type;

	if ( fmuType_ != fmi_1_0_cs ) return type;

	if ( hasChild( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" ) )
	{
		const Properties& attributes =
			getChildAttributes( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" );

		type = attributes.get<fmippString>( "type" );
	}

	return type;
}


// Get entry point from description (FMI CS feature).
fmippString
ModelDescription::getEntryPoint() const
{
	fmippString entryPoint;

	if ( fmuType_ != fmi_1_0_cs ) return entryPoint;

	if ( hasChild( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" ) )
	{
		const Properties& attributes =
			getChildAttributes( data_, "fmiModelDescription.Implementation.CoSimulation_Tool.Model" );

		entryPoint = attributes.get<fmippString>( "entryPoint" );
	}

	return entryPoint;
}


// Get number of continuous states from description.
fmippSize
ModelDescription::getNumberOfContinuousStates() const
{
	if ( 1 == getVersion() ) {
		const Properties& attributes = getChildAttributes( data_, "fmiModelDescription");
		return attributes.get<fmippSize>( "numberOfContinuousStates" );
	}

	// in the 2.0 specification, the entry number OfContinuousStattes has been removed because of redundancy
	// to get the number of continuous states, count the number of derivatives
	if ( fmippFalse == hasChild( data_, "fmiModelDescription.ModelStructure.Derivatives" ) ) return 0;

	const Properties& derivatives = data_.get_child("fmiModelDescription.ModelStructure.Derivatives");

	return derivatives.size();
}


// Get number of event indicators from description.
fmippSize
ModelDescription::getNumberOfEventIndicators() const
{
	const Properties& attributes = getChildAttributes( data_, "fmiModelDescription");
	return attributes.get<fmippSize>( "numberOfEventIndicators" );
}


// Get number of variables of type real, integer, boolean and string.
void
ModelDescription::getNumberOfVariables( fmippSize& nReal, fmippSize& nInt,
	fmippSize& nBool, fmippSize& nString ) const
{
	// Define XML tags to search for.
	const fmippString xmlRealTag( "Real" );
	const fmippString xmlIntTag( "Integer" );
	const fmippString xmlBoolTag( "Boolean" );
	const fmippString xmlStringTag( "String" );

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
			fmippString error( "[ModelDescription::getNumberOfVariables] unknown type: " );
			error += v.second.back().first;
			throw runtime_error( error );
		}
	}
}


// Get a vector of value references for all derivatives
void
ModelDescription::getStatesAndDerivativesReferences( fmippValueReference* state_ref, fmippValueReference* der_ref ) const
{
	int i = 0;
	const Properties& derivatives = data_.get_child( "fmiModelDescription.ModelStructure.Derivatives" );

	// get the index attribute from all derivatives
	BOOST_FOREACH( const Properties::value_type &v, derivatives )
		{
			der_ref[i]= v.second.get<unsigned int>( "<xmlattr>.index" );
			i++;
			continue;
		}
	const Properties& modelVariables = getModelVariables();
	i = 0;
	unsigned int j = 1;

	// use the indices to get all value references. suppose the index vector is monotone
	BOOST_FOREACH( const Properties::value_type & v, modelVariables )
		{
			if ( der_ref[i] == j ){
				der_ref[i] = v.second.get<unsigned int>( "<xmlattr>.valueReference" );
				state_ref[i] = v.second.get<unsigned int>( "Real.<xmlattr>.derivative" );
				i++;
				}
			j++;
			continue;
		}
	i = 0;
	j = 1;
	BOOST_FOREACH( const Properties::value_type & v, modelVariables )
		{
			if ( state_ref[i] == j ){
				state_ref[i] = v.second.get<unsigned int>( "<xmlattr>.valueReference" );
				i++;
			}
			j++;
		}
}


// Detect the type of FMU from the XML model description.
void
ModelDescription::detectFMUType()
{
	// Get the FMI model description attributes.
	const Properties& attributes = getChildAttributes( data_, "fmiModelDescription" );

	if ( hasChild( attributes, "fmiVersion" ) )
	{
		fmippString version = attributes.get<fmippString>( "fmiVersion" );

		if ( version == "1.0" )
		{
			if ( hasChild( data_, "fmiModelDescription.Implementation" ) ) {
				fmuType_ = fmi_1_0_cs;
				isValid_ = fmippTrue;
				return;
			} else {
				fmuType_ = fmi_1_0_me;
				isValid_ = fmippTrue;
				return;
			}
		}
		else if ( version == "2.0" )
		{
			if ( hasChild( data_, "fmiModelDescription.CoSimulation" ) &&
				 hasChild( data_, "fmiModelDescription.ModelExchange" ) ) {
				fmuType_ = fmi_2_0_me_and_cs;
				isValid_ = fmippTrue;
				return;
			} else if ( hasChild( data_, "fmiModelDescription.ModelExchange" ) ) {
				fmuType_ = fmi_2_0_me;
				isValid_ = fmippTrue;
				return;
			} else if ( hasChild( data_, "fmiModelDescription.CoSimulation" ) ) {
				fmuType_ = fmi_2_0_cs;
				isValid_ = fmippTrue;
				return;
			}
		}
	}

	isValid_ = fmippFalse;
	fmuType_ = invalid;
}


//
// Implementation of functionalities from namespace ModelDescriptionUtilities.
//

// Check for attributes.
fmippBoolean
ModelDescriptionUtilities::hasAttributes( const Properties& p )
{
	boost::optional<const Properties&> c = p.get_child_optional( "<xmlattr>" );
	return ( !c ) ? fmippFalse : fmippTrue;
}


// Check for attributes.
fmippBoolean
ModelDescriptionUtilities::hasAttributes( const Properties::iterator& it )
{
	boost::optional<Properties&> c = it->second.get_child_optional( "<xmlattr>" );
	return ( !c ) ? fmippFalse : fmippTrue;
}


// Check for attributes.
fmippBoolean
ModelDescriptionUtilities::hasAttributes( const Properties::const_iterator& it )
{
	boost::optional<const Properties&> c = it->second.get_child_optional( "<xmlattr>" );
	return ( !c ) ? fmippFalse : fmippTrue;
}


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


// Check child node for attributes.
fmippBoolean
ModelDescriptionUtilities::hasChildAttributes( const Properties& p,
	const fmippString& childName )
{
	if ( hasChild( p, childName ) ) {
		boost::optional<const Properties&> c =
			p.get_child( childName ).get_child_optional( "<xmlattr>" );
		return ( !c ) ? fmippFalse : fmippTrue;
	}
	return fmippFalse;
}


// Check child node for attributes.
fmippBoolean
ModelDescriptionUtilities::hasChildAttributes( const Properties::iterator& it,
	const fmippString& childName )
{
	if ( hasChild( it, childName ) ) {
		boost::optional<Properties&> c =
			it->second.get_child( childName ).get_child_optional( "<xmlattr>" );
		return ( !c ) ? fmippFalse : fmippTrue;
	}
	return fmippFalse;
}


// Check child node for attributes.
fmippBoolean
ModelDescriptionUtilities::hasChildAttributes( const Properties::const_iterator& it,
	const fmippString& childName )
{
	if ( hasChild( it, childName ) ) {
		boost::optional<const Properties&> c =
			it->second.get_child( childName ).get_child_optional( "<xmlattr>" );
		return ( !c ) ? fmippFalse : fmippTrue;
	}
	return fmippFalse;
}


// Get attributes of child node.
const Properties&
ModelDescriptionUtilities::getChildAttributes( const Properties& p,
	const fmippString& childName )
{
 	return p.get_child( childName ).get_child( "<xmlattr>" );
}


// Get attributes of child node.
const Properties&
ModelDescriptionUtilities::getChildAttributes( const Properties::iterator& it,
	const fmippString& childName )
{
 	return it->second.get_child( childName ).get_child( "<xmlattr>" );
}


// Get attributes of child node.
const Properties&
ModelDescriptionUtilities::getChildAttributes( const Properties::const_iterator& it,
	const fmippString& childName )
{
 	return it->second.get_child( childName ).get_child( "<xmlattr>" );
}


// Check for child node.
fmippBoolean
ModelDescriptionUtilities::hasChild( const Properties& p,
	const fmippString& childName )
{
	boost::optional<const Properties&> c = p.get_child_optional( childName );
	return ( !c ) ? fmippFalse : fmippTrue;
}


// Check for child node.
fmippBoolean
ModelDescriptionUtilities::hasChild( const Properties::iterator& it,
	const fmippString& childName )
{
	boost::optional<Properties&> c = it->second.get_child_optional( childName );
	return ( !c ) ? fmippFalse : fmippTrue;
}


// Check for child node.
fmippBoolean
ModelDescriptionUtilities::hasChild( const Properties::const_iterator& it,
	const fmippString& childName )
{
	boost::optional<const Properties&> c = it->second.get_child_optional( childName );
	return ( !c ) ? fmippFalse : fmippTrue;
}
