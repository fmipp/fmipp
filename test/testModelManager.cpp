#include <FMIPPConfig.h>
#include <ModelManager.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testModelDescription
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( test_model_manager_me )
{
	std::string modelName( "zigzag" );
	std::string fmuUrl = std::string( FMU_URI_PRE ) + modelName;

	ModelManager& manager = ModelManager::getModelManager();

	BareFMUModelExchange* bareFMU1 = manager.getModel( fmuUrl, modelName );
	BareFMUModelExchange* bareFMU2 = manager.getModel( fmuUrl, modelName );

	BOOST_REQUIRE_MESSAGE( bareFMU1 == bareFMU2,
			       "Bare FMUs are not equal." );
}


BOOST_AUTO_TEST_CASE( test_model_manager_cs )
{
	std::string modelName( "sine_standalone" );
	std::string fmuUrl = std::string( FMU_URI_PRE ) + modelName;

	ModelManager& manager = ModelManager::getModelManager();

	BareFMUCoSimulation* bareFMU1 = manager.getSlave( fmuUrl, modelName );
	BareFMUCoSimulation* bareFMU2 = manager.getSlave( fmuUrl, modelName );

	BOOST_REQUIRE_MESSAGE( bareFMU1 == bareFMU2,
			       "Bare FMUs are not equal." );
}
