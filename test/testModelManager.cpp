// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <common/fmi_v1.0/fmiModelTypes.h>
#include <common/FMIPPConfig.h>
#include <import/base/include/ModelManager.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testModelDescription
#include <boost/test/unit_test.hpp>


// Test handling of bad FMU location.
BOOST_AUTO_TEST_CASE( test_model_manager_me_no_file )
{
	// Specify non-existing FMU location.
	std::string modelName( "idontexist" );
	std::string fmuDirUrl = std::string( FMU_URI_PRE ) + modelName;

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Try to load the FMU.
	ModelManager::LoadFMUStatus status = ModelManager::failed;
	FMUType type = invalid;
	status = ModelManager::loadFMU( modelName, fmuDirUrl, fmiTrue, type );
	
	BOOST_REQUIRE_MESSAGE( status == ModelManager::description_invalid, "should have failed" );
	BOOST_REQUIRE_MESSAGE( type == invalid, "should be an invalid FMU type" );

	BareFMUModelExchangePtr bareFMU = manager.getModel( modelName );
	BOOST_REQUIRE( 0 == bareFMU.get() );
}


// Test model manager for FMI ME 1.0
BOOST_AUTO_TEST_CASE( test_model_manager_me_v1_0 )
{
	// Secify location of unzipped FMU directory.
	std::string modelName( "zigzag" );
	std::string fmuDirUrl = std::string( FMU_URI_PRE ) + modelName;

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Load FMU.
	ModelManager::LoadFMUStatus status = ModelManager::failed;
	FMUType type = invalid;
	status = ModelManager::loadFMU( modelName, fmuDirUrl, fmiTrue, type );
	
	// Check if the FMU was successfully loaded.
	BOOST_REQUIRE_MESSAGE( status == ModelManager::success, "failed to load FMU" );
	BOOST_REQUIRE_MESSAGE( type == fmi_1_0_me, "wrong FMU type" );
	
	// Try to load the FMU a second time.
	status = ModelManager::loadFMU( modelName, fmuDirUrl, fmiTrue, type );
	
	// The model manager should not have loaded it a second time.
	BOOST_REQUIRE_MESSAGE( status == ModelManager::duplicate, "FMU should not have been loaded a second time" );
	BOOST_REQUIRE_MESSAGE( type == fmi_1_0_me, "wrong FMU type" );

	// Instantiate two "bare FMUs".
	BareFMUModelExchangePtr bareFMU1 = manager.getModel( modelName );
	BareFMUModelExchangePtr bareFMU2 = manager.getModel( modelName );

	// Verify that they internally use the same FMU instance.
	BOOST_REQUIRE_MESSAGE( bareFMU1.get() == bareFMU2.get(), "bare FMUs are not equal." );
}


// Test model manager for FMI ME 2.0
BOOST_AUTO_TEST_CASE( test_model_manager_me_v2_0 )
{
	// Secify location of unzipped FMU directory.
	std::string modelName( "stiff2" );
	std::string fmuDirUrl = std::string( FMU_URI_PRE ) + "numeric/" + modelName;

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Load FMU.
	ModelManager::LoadFMUStatus status = ModelManager::failed;
	FMUType type = invalid;
	status = ModelManager::loadFMU( modelName, fmuDirUrl, fmiTrue, type );
	
	// Check if the FMU was successfully loaded.
	BOOST_REQUIRE_MESSAGE( status == ModelManager::success, "failed to load FMU" );
	BOOST_REQUIRE_MESSAGE( type == fmi_2_0_me, "wrong FMU type" );
	
	// Try to load the FMU a second time.
	status = ModelManager::loadFMU( modelName, fmuDirUrl, fmiTrue, type );
	
	// The model manager should not have loaded it a second time.
	BOOST_REQUIRE_MESSAGE( status == ModelManager::duplicate, "FMU should not have been loaded a second time" );
	BOOST_REQUIRE_MESSAGE( type == fmi_2_0_me, "wrong FMU type" );

	// Instantiate two "bare FMUs".
	BareFMU2Ptr bareFMU1 = manager.getInstance( modelName );
	BareFMU2Ptr bareFMU2 = manager.getInstance( modelName );

	// Verify that they internally use the same FMU instance.
	BOOST_REQUIRE_MESSAGE( bareFMU1.get() == bareFMU2.get(), "bare FMUs are not equal." );
}


// Test distinction in mode manager between FMI ME 1.0 and FMI ME 2.0
BOOST_AUTO_TEST_CASE( test_model_manager_me_no_v1_0 )
{
	// Secify location of unzipped FMU directory.
	std::string modelName( "v2_0" );
	std::string fmuDirUrl = std::string( FMU_URI_PRE ) + modelName;

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Try to load the FMU.
	ModelManager::LoadFMUStatus status = ModelManager::failed;
	FMUType type = invalid;
	status = ModelManager::loadFMU( modelName, fmuDirUrl, fmiTrue, type );
	
	// Check if the FMU was successfully loaded (as FMU ME 2.0).
	BOOST_REQUIRE_MESSAGE( status == ModelManager::success, "failed to load FMU" );
	BOOST_REQUIRE_MESSAGE( type == fmi_2_0_me, "wrong FMU type" );
	
	// Ceck that that this model cannot be retrieved with a bare FMU according to FMI ME 1.0.
	BareFMUModelExchangePtr bareFMU = manager.getModel( modelName );
	BOOST_REQUIRE( 0 == bareFMU.get() );
}


// Test model manager for FMI CS 1.0
BOOST_AUTO_TEST_CASE( test_model_manager_cs_v1_0 )
{
	// Secify location of unzipped FMU directory.
	std::string modelName( "sine_standalone" );
	std::string fmuDirUrl = std::string( FMU_URI_PRE ) + modelName;

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Try to load the FMU.
	ModelManager::LoadFMUStatus status = ModelManager::failed;
	FMUType type = invalid;
	status = ModelManager::loadFMU( modelName, fmuDirUrl, fmiTrue, type );
	
	// Check if the FMU was successfully loaded (as FMU CS 1.0).
	BOOST_REQUIRE_MESSAGE( status == ModelManager::success, "failed to load FMU" );
	BOOST_REQUIRE_MESSAGE( type == fmi_1_0_cs, "wrong FMU type" );

	// Instantiate two "bare FMUs".
	BareFMUCoSimulationPtr bareFMU1 = manager.getSlave( modelName );
	BareFMUCoSimulationPtr bareFMU2 = manager.getSlave( modelName );

	// Verify that they internally use the same FMU instance.
	BOOST_REQUIRE_MESSAGE( bareFMU1.get() == bareFMU2.get(), "bare FMUs are not equal." );
}


// Test unloading of FMUs from model manager.
BOOST_AUTO_TEST_CASE( test_model_remove_model )
{
	// Secify location of unzipped FMU directory (FMI ME 1.0).
	std::string meModelName( "zigzag" );
	std::string meFmuDirUrl = std::string( FMU_URI_PRE ) + meModelName;

	// Secify location of unzipped FMU directory (FMI CS 1.0).
	std::string csModelName( "sine_standalone" );
	std::string csFmuDirUrl = std::string( FMU_URI_PRE ) + csModelName;

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();

	ModelManager::LoadFMUStatus loadStatus = ModelManager::failed;
	ModelManager::UnloadFMUStatus unloadStatus = ModelManager::unknown;
	FMUType type = invalid;

	// Load the FMU (FMI ME 1.0).
	loadStatus = ModelManager::loadFMU( meModelName, meFmuDirUrl, fmiTrue, type );
	BOOST_REQUIRE_MESSAGE( ( loadStatus == ModelManager::success ) || ( loadStatus == ModelManager::duplicate ), "failed to load FMU" );
	BOOST_REQUIRE_MESSAGE( type == fmi_1_0_me, "wrong FMU type" );

	// Load the FMU (FMI CS 1.0).
	loadStatus = ModelManager::loadFMU( csModelName, csFmuDirUrl, fmiTrue, type );
	BOOST_REQUIRE_MESSAGE( ( loadStatus == ModelManager::success ) || ( loadStatus == ModelManager::duplicate ), "failed to load FMU" );
	BOOST_REQUIRE_MESSAGE( type == fmi_1_0_cs, "wrong FMU type" );

	if ( true ) {
		// Retrieve smart pointers to bare FMUs.
		BareFMUModelExchangePtr meBareFMU = manager.getModel( meModelName );
		BareFMUCoSimulationPtr csBareFMU = manager.getSlave( csModelName );

		// Unloading should fail, because bare FMU is in use.
		unloadStatus = ModelManager::unloadFMU( meModelName );
		BOOST_REQUIRE_MESSAGE( unloadStatus == ModelManager::in_use,
			"Deleting the model (FMI ME 1.0) should NOT have succeded." );

		// Unloading should fail, because bare FMU is in use.
		unloadStatus = ModelManager::unloadFMU( csModelName );
		BOOST_REQUIRE_MESSAGE( unloadStatus == ModelManager::in_use,
			"Deleting the model  (FMI ME 2.0) should NOT have succeded." );
	} // After exiting the scope, the smart pointers will be destructed --> unloading the FMUs should work!
	
	// Unloading should work now.
	unloadStatus = ModelManager::unloadFMU( meModelName );
	BOOST_REQUIRE_MESSAGE( unloadStatus == ModelManager::ok,
		"Deleting the model (FMI ME 1.0) should have succeded." );

	// Unloading should work now.
	unloadStatus = ModelManager::unloadFMU( csModelName );
	BOOST_REQUIRE_MESSAGE( unloadStatus == ModelManager::ok,
		"Deleting the model (FMI CS 1.0) should have succeded." );

	// Unloading a non-existing model ...
	unloadStatus = ModelManager::unloadFMU( "this model does not exist" );
	BOOST_REQUIRE_MESSAGE( unloadStatus == ModelManager::not_found,
		"deleting a non-existing model should not have succeded." );
}
