// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------
#include <iostream>
#include <stdlib.h>
#include <common/fmi_v1.0/fmiModelTypes.h>
#include <common/FMIPPConfig.h>
#include <import/base/include/ModelManager.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testModelDescription
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( test_model_manager_me )
{
	std::string modelName( "zigzag" );
	std::string fmuUrl = std::string( FMU_URI_PRE ) + modelName;

	ModelManager& manager = ModelManager::getModelManager();

	BareFMUModelExchangePtr bareFMU1 = manager.getModel( fmuUrl, modelName, fmiTrue );
	BareFMUModelExchangePtr bareFMU2 = manager.getModel( fmuUrl, modelName, fmiTrue );

	BOOST_REQUIRE_MESSAGE( bareFMU1.get() == bareFMU2.get(),
			       "Bare FMUs are not equal." );
}


BOOST_AUTO_TEST_CASE( test_model_manager_me_no_file )
{
	std::string modelName( "idontexist" );
	std::string fmuUrl = std::string( FMU_URI_PRE ) + modelName;

	ModelManager& manager = ModelManager::getModelManager();

	BareFMUModelExchangePtr bareFMU = manager.getModel( fmuUrl, modelName, fmiTrue );
	BOOST_REQUIRE( 0 == bareFMU.get() );
}


BOOST_AUTO_TEST_CASE( test_model_manager_me_no_v1_0 )
{
	std::string modelName( "v2_0" );
	std::string fmuUrl = std::string( FMU_URI_PRE ) + modelName;

	ModelManager& manager = ModelManager::getModelManager();

	BareFMUModelExchangePtr bareFMU = manager.getModel( fmuUrl, modelName, fmiTrue );
	BOOST_REQUIRE( 0 == bareFMU.get() );
}


BOOST_AUTO_TEST_CASE( test_model_manager_cs )
{
	std::string modelName( "sine_standalone" );
	std::string fmuUrl = std::string( FMU_URI_PRE ) + modelName;

	ModelManager& manager = ModelManager::getModelManager();

	BareFMUCoSimulationPtr bareFMU1 = manager.getSlave( fmuUrl, modelName, fmiTrue );
	BareFMUCoSimulationPtr bareFMU2 = manager.getSlave( fmuUrl, modelName, fmiTrue );

	BOOST_REQUIRE_MESSAGE( bareFMU1.get() == bareFMU2.get(),
			       "Bare FMUs are not equal." );
}


BOOST_AUTO_TEST_CASE( test_model_remove_model )
{
	std::string meModelName( "zigzag" );
	std::string meFmuUrl = std::string( FMU_URI_PRE ) + meModelName;

	std::string csModelName( "sine_standalone" );
	std::string csFmuUrl = std::string( FMU_URI_PRE ) + csModelName;

	ModelManager& manager = ModelManager::getModelManager();
	ModelManager::ModelDeleteStatus status = ModelManager::unknown;

	if ( true ) {
		BareFMUModelExchangePtr meBareFMU = manager.getModel( meFmuUrl, meModelName, fmiTrue );
		BareFMUCoSimulationPtr csBareFMU = manager.getSlave( csFmuUrl, csModelName, fmiTrue );

		status = ModelManager::deleteModel( meModelName );
		BOOST_REQUIRE_MESSAGE( status == ModelManager::in_use,
			"Deleting the model (FMI ME 1.0) should NOT have succeded." );

		status = ModelManager::deleteModel( csModelName );
		BOOST_REQUIRE_MESSAGE( status == ModelManager::in_use,
			"Deleting the model  (FMI ME 2.0) should NOT have succeded." );
	} // After exiting the scope, the smart pointers will be destructed --> deleting the models should work!
	
	status = ModelManager::deleteModel( meModelName );
	BOOST_REQUIRE_MESSAGE( status == ModelManager::ok,
		"Deleting the model (FMI ME 1.0) should have succeded." );

	status = ModelManager::deleteModel( csModelName );
	BOOST_REQUIRE_MESSAGE( status == ModelManager::ok,
		"Deleting the model (FMI CS 1.0) should have succeded." );

	status = ModelManager::deleteModel( "this model does not exist" );
	BOOST_REQUIRE_MESSAGE( status == ModelManager::not_found,
		"Deleting a non-existing model should not have succeded." );
}
