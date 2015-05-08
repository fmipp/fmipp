// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

#include <stdlib.h>
#include <common/fmi_v1.0/fmiModelTypes.h>
#include <import/base/include/ModelDescription.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testModelDescription
#include <boost/test/unit_test.hpp>


#if defined( WIN32 )
#include "windows.h"
#include "Shlwapi.h"
#include "TCHAR.h"
#endif


std::string getPathFromUrl( const std::string& inputFileUrl )
{
#if defined( WIN32 )
	LPCTSTR fileUrl = inputFileUrl.c_str();
	LPTSTR filePath = new TCHAR[MAX_PATH];
	DWORD filePathSize = inputFileUrl.size() + 1;
	DWORD tmp = 0;
	PathCreateFromUrl( fileUrl, filePath, &filePathSize, tmp );
	std::string strFilePath( filePath );
	delete filePath;
	return strFilePath;
#else
	/// \FIXME Replace with proper Linux implementation.
	if ( inputFileUrl.substr( 0, 7 ) != "file://" )
		throw std::invalid_argument( std::string( "Cannot handle URI: " ) + inputFileUrl );

	return inputFileUrl.substr( 7, inputFileUrl.size() );
#endif
}



BOOST_AUTO_TEST_CASE( test_model_description_me )
{
	std::string modelName( "zigzag" );
	std::string fileUrl = std::string( FMU_URI_PRE ) + modelName + std::string( "/modelDescription.xml" ); 
	ModelDescription md( getPathFromUrl( fileUrl ) );

	std::string modelIdentifier = md.getModelIdentifier();
	BOOST_REQUIRE_MESSAGE( modelIdentifier == "zigzag",
			       "Wrong model identifier: " << modelIdentifier );

	std::string guid = md.getGUID();
	BOOST_REQUIRE_MESSAGE( guid == "{12345678-1234-1234-1234-123456789910f}",
			       "Wrong GUID: " << guid );

	int numberOfContinuousStates = md.getNumberOfContinuousStates();
	BOOST_REQUIRE_MESSAGE( numberOfContinuousStates == 1,
			       "wrong number of continuous states: " << numberOfContinuousStates );

	int numberOfEventIndicators = md.getNumberOfEventIndicators();
	BOOST_REQUIRE_MESSAGE( numberOfEventIndicators == 1,
			       "wrong number of event indicators: " << numberOfEventIndicators );

	const ModelDescription::Properties& modelVariables = md.getModelVariables();
	BOOST_REQUIRE_MESSAGE( modelVariables.size() == 4,
			       "wrong number of model variables: " << modelVariables.size() );

	const ModelDescription::Properties& attributes =
		ModelDescriptionUtilities::getAttributes( modelVariables.begin() );
	BOOST_REQUIRE_MESSAGE( attributes.get<std::string>( "name" ) == "x",
			       "wrong variable name: " << attributes.get<std::string>( "name" ) );

	bool hasRealTag = ModelDescriptionUtilities::hasChild( modelVariables.begin(), "Real" );
	BOOST_REQUIRE_MESSAGE( hasRealTag == true,
			       "no child node named 'Real'" );

	const ModelDescription::Properties& realTagAttributes =
		ModelDescriptionUtilities::getChildAttributes( modelVariables.begin(), "Real" );
	BOOST_REQUIRE_MESSAGE( realTagAttributes.get<fmiReal>( "start" ) == 0.,
			       "wrong variable start value: " << realTagAttributes.get<fmiReal>( "start" ) );

	size_t nReal = static_cast<size_t>(-1);
	size_t nInt = static_cast<size_t>(-1);
	size_t nBool = static_cast<size_t>(-1);
	size_t nString = static_cast<size_t>(-1);
	md.getNumberOfVariables( nReal, nInt, nBool, nString );
	BOOST_REQUIRE_MESSAGE( nReal == 4, "wrong number of real variables: " << nReal );
	BOOST_REQUIRE_MESSAGE( nInt == 0, "wrong number of integer variables: " << nInt );
	BOOST_REQUIRE_MESSAGE( nBool == 0, "wrong number of boolean variables: " << nBool );
	BOOST_REQUIRE_MESSAGE( nString == 0, "wrong number of string variables: " << nString );
}


BOOST_AUTO_TEST_CASE( test_model_description_me_2_0 )
{
	// check parsing of the modelDescription of v2_0
	std::string modelName( "v2_0" );
	std::string fileUrl = std::string( FMU_URI_PRE ) + modelName + std::string( "/modelDescription.xml" );
	ModelDescription md( getPathFromUrl( fileUrl ) );

	BOOST_REQUIRE( md.isValid() );

	BOOST_REQUIRE_EQUAL( md.getVersion(), 2 );

	std::string modelIdentifier = md.getModelIdentifier();
	BOOST_CHECK_EQUAL( modelIdentifier, "v2_0" );

	BOOST_CHECK_EQUAL( md.getNumberOfContinuousStates(), 1 );
	unsigned int stateRef, derivRef;
	md.getStatesAndDerivativesReferences( &stateRef, &derivRef );
	BOOST_CHECK_EQUAL( stateRef, 0 );
	BOOST_CHECK_EQUAL( derivRef, 1 );

	BOOST_CHECK_EQUAL( md.providesJacobian(), false );

	// load the stiff fmu
	modelName = "numeric/stiff2";
	fileUrl = std::string( FMU_URI_PRE ) + modelName + std::string( "/modelDescription.xml" );
	ModelDescription md2( getPathFromUrl( fileUrl ) );

	BOOST_REQUIRE( md2.isValid() );

	BOOST_CHECK_EQUAL( md2.providesJacobian(), true );
}


BOOST_AUTO_TEST_CASE( test_model_description_cs )
{
	std::string modelName( "sine_standalone" );
	std::string fileUrl = std::string( FMU_URI_PRE ) + modelName + std::string( "/modelDescription.xml" ); 
	ModelDescription md( getPathFromUrl( fileUrl ) );

	std::string modelIdentifier = md.getModelIdentifier();
	BOOST_REQUIRE_MESSAGE( modelIdentifier == "sine_standalone",
			       "Wrong model identifier: " << modelIdentifier );

	std::string guid = md.getGUID();
	BOOST_REQUIRE_MESSAGE( guid == "{00000000-0000-0000-0000-000000000000}",
			       "Wrong GUID: " << guid );

	int numberOfContinuousStates = md.getNumberOfContinuousStates();
	BOOST_REQUIRE_MESSAGE( numberOfContinuousStates == 0,
			       "wrong number of continuous states: " << numberOfContinuousStates );

	int numberOfEventIndicators = md.getNumberOfEventIndicators();
	BOOST_REQUIRE_MESSAGE( numberOfEventIndicators == 0,
			       "wrong number of event indicators: " << numberOfEventIndicators );

	std::string type = md.getMIMEType();
	BOOST_REQUIRE_MESSAGE( type == "application/x-sine_standalone_exe",
			       "wrong MIME type: " << type );

	std::string entryPoint = md.getEntryPoint();
	BOOST_REQUIRE_MESSAGE( entryPoint == "file://entry/point",
			       "wrong entry point: " << entryPoint );
}



// BOOST_AUTO_TEST_CASE( test_model_description_xxx )
// {
// 	BOOST_REQUIRE( false );
// }
