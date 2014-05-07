/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _FMIPP_ModelDescription_H
#define _FMIPP_ModelDescription_H


/**
 *  \file ModelDescription.h 
 *  \class ModelDescription ModelDescription.h 
 *  Parses and stores FMI model descriptions. 
 *  
 *  The FMI standard defines an XML model description scheme. This class 
 *  provides the utilities to parse and store this information dynamically
 *  during run-time. It uses Boost's PropertyTree internally.
 */

#include "FMIPPConfig.h"

#include <string>
#include <boost/property_tree/ptree.hpp>


class __FMI_DLL ModelDescription
{

public:

	typedef boost::property_tree::ptree Properties;

public:

	ModelDescription( const std::string& xmlDescriptionFilePath );

	const Properties& getModelVariables() const; ///< Get description of model variables.

	std::string getModelIdentifier() const; ///< Get model identifier from description.
	std::string getGUID() const; ///< Get GUID from description.

	std::string getMIMEType() const; ///< Get MIME type from description (FMI CS feature).

	int getNumberOfContinuousStates() const; ///< Get number of continuous states from description.
	int getNumberOfEventIndicators() const; ///< Get number of event indicators from description.


private:

	Properties data_; ///< This data structure (a Boost PropertyTree) holds the parsed model description.

	//bool isMEv1_; ///< Flag to indicated whether this FMU is ME (v1.0).
	bool isCSv1_; ///< Flag to indicated whether this FMU is CS (v1.0).

	//bool isMEv2_; ///< Flag to indicated whether this FMU is ME (v2.0).
	//bool isCSv2_; ///< Flag to indicated whether this FMU is CS (v2.0).
};




namespace ModelDescriptionUtilities ///< Namespace containing helper functions for dealing with class ModelDescription.
{
	typedef ModelDescription::Properties Properties;

	__FMI_DLL const Properties& getAttributes( const Properties& p ); ///< Get attributes of current node.
	__FMI_DLL const Properties& getAttributes( const Properties::iterator& it ); ///< Get attributes of current node.
	__FMI_DLL const Properties& getAttributes( const Properties::const_iterator& it ); ///< Get attributes of current node.

	__FMI_DLL const Properties& getChildAttributes( const Properties& p,
							const std::string& childName ); ///< Get attributes of child node.
	__FMI_DLL const Properties& getChildAttributes( const Properties::iterator& it,
							const std::string& childName ); ///< Get attributes of child node.
	__FMI_DLL const Properties& getChildAttributes( const Properties::const_iterator& it,
							const std::string& childName ); ///< Get attributes of child node.

	__FMI_DLL bool hasChild( const Properties& p,
				 const std::string& childName ); ///< Check for child node.
	__FMI_DLL bool hasChild( const Properties::iterator& it,
				 const std::string& childName ); ///< Check for child node.
	__FMI_DLL bool hasChild( const Properties::const_iterator& it,
				 const std::string& childName ); ///< Check for child node.
}


#endif
