#include <InterpolatingFixedStepSizeFMU.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testFMU
#include <boost/test/unit_test.hpp>
#include <cmath>


BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string MODELNAME( "sine_standalone" );
	InterpolatingFixedStepSizeFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
}

BOOST_AUTO_TEST_CASE( test_fmu_instantiate )
{
	// FIXME: Implement test here.
	BOOST_REQUIRE( false );
}

BOOST_AUTO_TEST_CASE( test_fmu_initialize )
{
	// FIXME: Implement test here.
	BOOST_REQUIRE( false );
}

BOOST_AUTO_TEST_CASE( test_fmu_getvalue )
{
	// FIXME: Implement test here.
	BOOST_REQUIRE( false );
}

BOOST_AUTO_TEST_CASE( test_fmu_setvalue )
{
	// FIXME: Implement test here.
	BOOST_REQUIRE( false );
}

BOOST_AUTO_TEST_CASE( test_fmu_run_simulation )
{
	// FIXME: Implement test here.
	BOOST_REQUIRE( false );
}
