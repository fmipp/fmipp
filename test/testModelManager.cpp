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

/* Function definition used in the test set */
static void testLoadFMUAutoname( const std::string& fmuDirUrl, 
	const std::string& refModelName, FMUType refType );
static void testUnloadFMU( const std::string& modelName );

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

// Test an invalid file location on the instance-name free loadFMU function
BOOST_AUTO_TEST_CASE(test_model_manager_me_no_file_autoname)
{
	// Specify non-existing FMU location.
	std::string fmuDirUrl = std::string( FMU_URI_PRE ) + "no-file-here";

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Try to load the FMU.
	ModelManager::LoadFMUStatus status = ModelManager::failed;
	FMUType type = invalid;
	std::string instanceName("Do not change me!");

	status = ModelManager::loadFMU( fmuDirUrl, fmiTrue, type, instanceName );
	
	BOOST_REQUIRE_EQUAL( status, ModelManager::description_invalid );
	BOOST_CHECK_EQUAL( type, invalid );
	BOOST_CHECK_EQUAL( instanceName, std::string("Do not change me!") );
}

// Test model manager for FMI ME 1.0
BOOST_AUTO_TEST_CASE( test_model_manager_me_v1_0 )
{
	// Specify location of unzipped FMU directory.
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

// Test model manager for FMI ME 1.0 on using automatic instance names
BOOST_AUTO_TEST_CASE( test_model_manager_me_v1_0_autoname )
{
	// Test loading the FMU
	std::string modelName("zigzag");
	testLoadFMUAutoname( std::string(FMU_URI_PRE) + modelName, 
		modelName, fmi_1_0_me );

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();
	{ // Instantiate two "bare FMUs".
		BareFMUModelExchangePtr bareFMU1 = manager.getModel( modelName );
		BareFMUModelExchangePtr bareFMU2 = manager.getModel( modelName );

		// Verify that they internally use the same FMU instance.
		BOOST_REQUIRE_MESSAGE(bareFMU1.get() == bareFMU2.get(),
			"bare FMUs are not equal.");
		
		// Try to unload
		auto unloadStatus = ModelManager::unloadFMU( modelName );
		BOOST_CHECK_EQUAL( unloadStatus, ModelManager::in_use );
	}

	testUnloadFMU( modelName );
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

// Test model manager for FMI ME 2.0 on using automatic instance names
BOOST_AUTO_TEST_CASE( test_model_manager_me_v2_0_autoname )
{
	// Test loading the FMU
	std::string modelName("zigzag2");
	testLoadFMUAutoname( std::string(FMU_URI_PRE) + modelName, 
		modelName, fmi_2_0_me );

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();
	{ // Instantiate two "bare FMUs".
		BareFMU2Ptr bareFMU1 = manager.getInstance( modelName );
		BareFMU2Ptr bareFMU2 = manager.getInstance( modelName );

		// Verify that they internally use the same FMU instance.
		BOOST_REQUIRE_MESSAGE(bareFMU1.get() == bareFMU2.get(),
			"bare FMUs are not equal.");
		
		// Try to unload
		auto unloadStatus = ModelManager::unloadFMU( modelName );
		BOOST_CHECK_EQUAL( unloadStatus, ModelManager::in_use );
	}
	// Try to unload the model
	testUnloadFMU( modelName );
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
	
	// Check that this model cannot be retrieved with a bare FMU according to FMI ME 1.0.
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

// Test model manager for CS 1.0 on using automatic instance names
BOOST_AUTO_TEST_CASE( test_model_manager_cs_v1_0_autoname )
{
	// Test loading the FMU
	std::string modelName("sine_standalone");
	testLoadFMUAutoname( std::string(FMU_URI_PRE) + modelName, 
		modelName, fmi_1_0_cs );

	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();
	{ // Instantiate two "bare FMUs".
		BareFMUCoSimulationPtr bareFMU1 = manager.getSlave( modelName );
		BareFMUCoSimulationPtr bareFMU2 = manager.getSlave( modelName );

		// Verify that they internally use the same FMU instance.
		BOOST_REQUIRE_MESSAGE(bareFMU1.get() == bareFMU2.get(),
			"bare FMUs are not equal.");
		
		// Try to unload
		auto unloadStatus = ModelManager::unloadFMU( modelName );
		BOOST_CHECK_EQUAL( unloadStatus, ModelManager::in_use );
	}
	// Try to unload the model
	testUnloadFMU( modelName );
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


/**
 * Loads an fmu into the model manager instance and tests the outcome.
 * It is assumed that initially, no instance is loaded. After the tests 
 * successfully completed, the given model instance will be present. It is not
 * tried to access the given instance. The access functions have to be tested
 * individually
 * \param[in] fmuDirUrl A valid URL to the FMU directory
 * \param[in] refModelName The name of the model for reference purpose
 * \param[in] refType The expected FMU type.
 */
static void
testLoadFMUAutoname(const std::string& fmuDirUrl,
	const std::string& refModelName, FMUType refType)
{
	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Load FMU.
	ModelManager::LoadFMUStatus status = ModelManager::failed;
	FMUType type = invalid;
	std::string modelName( "uninitialized" );
	status = ModelManager::loadFMU( fmuDirUrl, fmiTrue, type, modelName );
	
	// Check if the FMU was successfully loaded.
	BOOST_REQUIRE_EQUAL( status, ModelManager::success );
	BOOST_REQUIRE_EQUAL( type, refType );
	BOOST_CHECK_EQUAL(modelName, refModelName);

	// Try to load the FMU a second time.
	// The model manager should not have loaded it a second time.
	status = ModelManager::loadFMU( modelName, fmuDirUrl, fmiTrue, type );
	BOOST_REQUIRE_EQUAL( status, ModelManager::duplicate );
	BOOST_REQUIRE_EQUAL( type, refType );

	// Try to load the FMU a third time. Again, with automatic name deduction
	modelName = "uninitialized again";
	status = ModelManager::loadFMU(fmuDirUrl, fmiTrue, type, modelName );
	BOOST_REQUIRE_EQUAL( status, ModelManager::duplicate );
	BOOST_REQUIRE_EQUAL( type, refType );
	BOOST_CHECK_EQUAL(modelName, refModelName);
}

/**
 * Test unloading the given model
 * It is assumed that the model was previously loaded. The function will not 
 * test the model access function before or after the model has been removed.
 * \param[in] modelName The model identifier which was used to load the model.
 */
static void
testUnloadFMU(const std::string& modelName)
{
	// Get model manager.
	ModelManager& manager = ModelManager::getModelManager();

	// Unload FMU
	auto unloadStatus = ModelManager::unloadFMU( modelName );
	BOOST_CHECK_EQUAL( unloadStatus, ModelManager::ok );

	// Try to unload FMU again
	unloadStatus = ModelManager::unloadFMU( modelName );
	BOOST_CHECK_EQUAL( unloadStatus, ModelManager::not_found );
}