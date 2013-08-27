#include "HelperFunctions.h"

//#include <algorithm>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>


using namespace std;


namespace HelperFunctions {

	void splitAndTrim( const string& input,
			   vector<string>& result,
			   const string& seperators )
	{
		boost::split( result, input, boost::is_any_of( seperators ) );
		for_each( result.begin(), result.end(), boost::bind( &boost::trim<string>, _1, locale() ) );
	}



	TCHAR* copyStringToTCHAR( const std::string& str, size_t extra_length )
	{
		TCHAR *result = new TCHAR[str.size() + extra_length + 1];
		result[str.size()] = 0;
		copy( str.begin(), str.end(), result );
		return result;
	}

}

