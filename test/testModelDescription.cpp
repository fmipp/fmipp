#include <ModelDescription.h>
#include <fmiModelTypes.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testModelDescription
#include <boost/test/unit_test.hpp>


#if defined( WIN32 )
#include "Windows.h"
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
	return std::string( filePath );
#else
	// FIXME: Replace with proper Linux implementation.
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
	BOOST_REQUIRE_MESSAGE( numberOfContinuousStates == 2,
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
	BOOST_REQUIRE_MESSAGE( type == "sine_standalone_exe",
			       "wrong MIME type: " << type );
}



// BOOST_AUTO_TEST_CASE( test_model_description_xxx )
// {
// 	BOOST_REQUIRE( false );
// }
