/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _PF_API_UTILS_H
#define _PF_API_UTILS_H

/**
 * \file Utils.h 
 *
 * \authors Roman Schwalbe, Matthias Stifter, Edmund Widl
 */

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <map>

#include "PowerFactory.h"

namespace pf_api {

	namespace utils {

		void split( const std::string &text,char delim,std::vector<std::string> &cols );
		double strToDbl( std::string val );
		long strToLong( std::string val );
		long long strToLongLong( std::string val );

		void addVariantToValueVec( Value& vector, const PowerFactory::Variant& variant );
		std::string convertVariantToString( const PowerFactory::Variant& variant );
		PowerFactory::Variant convertStringToVariant( const std::string& type, const std::string& value );
		
		template <typename Tret,typename Targ> static Tret castWithException( Targ value )
		{
			Tret t = Tret();
			try {
				t = boost::lexical_cast<Tret>( value );
			} catch( std::exception e ) {
				LOG_DEBUG << "Utils: Error while converting " << typeid( Targ ).name()
						  << " value " << value << " to " << typeid( Tret ).name()
						  << ": " << e.what() << std::endl;
			}
			return t;
		}

	} // namespace utils

} // namespace pf_api

#endif // _PF_API_UTILS_H
