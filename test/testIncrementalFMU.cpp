#include <IncrementalFMU.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE testIncrementalFMU
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( test_fmu_load )
{
	std::string MODELNAME( "zigzag" );
	IncrementalFMU fmu( FMU_URI_PRE + MODELNAME, MODELNAME );
}
