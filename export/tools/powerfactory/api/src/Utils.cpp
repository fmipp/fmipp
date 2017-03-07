/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

/// \file Utils.cpp
/// \authors Roman Schwalbe, Matthias Stifter, Edmund Widl

#ifndef _WIN32_WINDOWS                      // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS _WIN32_WINNT_WIN7    // Target Windows 7 or later.
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#endif

// Check for compilation with Visual Studio 2010 (required).
#if ( _MSC_VER == 1800 )
#include "windows.h"
#else
#error This project requires Visual Studio 2013.
#endif

// PowerFactory API includes.
#include "v1\Api.hpp"

// Boost includes.
#include <boost/lexical_cast.hpp>

// Project includes.
#include "Utils.h"
#include "SimpleLogger.h"


using namespace pf_api;


void
utils::split( const std::string &text, char delim, std::vector<std::string> &elems )
{
	elems.clear();
    std::stringstream ss( text );
    std::string item;
    while( std::getline( ss, item, delim ) ) {
        elems.push_back( item );
    }
}


double
utils::strToDbl( std::string val )
{
	size_t pos=val.find(',');
	if( pos != val.npos ) val.replace( pos, 1, "." );
	double d = 0;
	try {
		d = boost::lexical_cast<double>( val );
	} catch( std::exception e ) {
		LOG_DEBUG << "Utils: Error while converting " << val << " to double: " << e.what() << std::endl;
	}
	return d;
}


long
utils::strToLong( std::string val )
{
	long l = 0;
	try {
		l = boost::lexical_cast<long>( val );
	} catch( std::exception e ) {
		LOG_DEBUG << "Utils: Error while converting " << val << " to long: " << e.what() << std::endl;
	}
	return l;
}


long long
utils::strToLongLong( std::string val )
{
	long long l = 0;
	try {
		l = boost::lexical_cast<long long>( val );
	} catch( std::exception e ) {
		LOG_DEBUG << "Utils: Error while converting " << val << " to long: " << e.what() << std::endl;
	}
	return l;
}


void
utils::addVariantToValueVec( Value& vector, const PowerFactory::Variant& variant )
{	
	if ( const int* ip = boost::get<int>( &variant ) )
		vector.VecInsertInteger( *ip );
	else if	( const double *dp = boost::get<double>( &variant ) )
		vector.VecInsertDouble( *dp );
	else if ( const std::string *sp = boost::get<std::string>( &variant ) )
		vector.VecInsertString( sp->c_str() );
	else
		LOG_ERROR << "[addVariantToValueVec] error while adding " << variant << " to vector" << std::endl;
}


std::string 
utils::convertVariantToString( const PowerFactory::Variant& variant )
{
	if ( const int* ip = boost::get<int>( &variant ) )
	{	std::stringstream ss;
		ss << *ip;
		return ss.str();
	} else if ( const double* dp = boost::get<double>( &variant ) ) {
		std::stringstream ss;
		ss << *dp;
		return ss.str();
	} else if ( const std::string* sp = boost::get<std::string>( &variant ) ) {
		return *sp;
	} else {
		LOG_ERROR << "[convertVariantToString] error while converting " << variant << " to string" << std::endl;
	}

	return std::string();
}


PowerFactory::Variant
utils::convertStringToVariant( const std::string& type, const std::string& value ) {
	if ( 0 == type.compare( "string" ) ) {
		// PowerFactory-Strings are always surrounded by '
		return PowerFactory::Variant( value.size() < 2 ? value : value.substr( 1, value.size() - 2 ) );
	} else if ( 0 == type.compare( "int" ) ) {
		return PowerFactory::Variant( static_cast<int>( utils::strToLong( value ) ) );
	} else if ( 0 == type.compare( "double" ) ) {
		return PowerFactory::Variant( utils::strToDbl( value ) );
	} else {
		LOG_ERROR << "[convertStringToVariant] error while converting " << type << " to variant" << std::endl;
	}

	return PowerFactory::Variant();
}
