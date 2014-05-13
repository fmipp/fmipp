/* --------------------------------------------------------------
 * Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/


#ifndef _FMIPP_FMUHANDLE_H
#define _FMIPP_FMUHANDLE_H


#include <string>

#include "common/fmi_v1.0/fmiModelTypes.h"

#include "common/FMIPPConfig.h"
#include "common/FMIType.h"


/**
 * \file FMUBase.h
 *
 * \class FMUBase FMUBase.h  Abstract base class for wrappers handling FMUs.
 *
 * This class defined the getter/setter functions that need to be implemented
 * by any class used for handling FMUs, regardless of their type (ME or CS).
 *
 **/


class __FMI_DLL FMUBase
{

public:

	/// Destructor.
	virtual ~FMUBase() {}


	/// Get number of continuous states. 
	virtual std::size_t nStates() const = 0;

	/// Get number of event indicators.
	virtual std::size_t nEventInds() const = 0;

	/// Get number of value references (equals the numer of variables).
	virtual std::size_t nValueRefs() const = 0;


	/// Get current time.
	virtual fmiReal getTime() const = 0;


	/// Get value reference associated to variable name.
	virtual fmiValueReference getValueRef( const std::string& name ) const = 0;

	/// Get type of variable.
	virtual FMIType getType( const std::string& variableName ) const = 0;


	/// Get single value of type fmiReal, using the value reference.
	virtual fmiStatus getValue( fmiValueReference valref, fmiReal& val ) const = 0;

	/// Get single value of type fmiInteger, using the value reference.
	virtual fmiStatus getValue( fmiValueReference valref, fmiInteger& val ) const = 0;

	/// Get single value of type fmiBoolean, using the value reference.
	virtual fmiStatus getValue( fmiValueReference valref, fmiBoolean& val ) const = 0;

	/// Get single value of string (using internaly type fmiString), using the value reference.
	virtual fmiStatus getValue( fmiValueReference valref, std::string& val ) const = 0;


	/// Get values of type fmiReal, using an array of value references. 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) const = 0;

	/// Get values of type fmiInteger, using an array of value references. 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) const = 0;

	/// Get values of type fmiBoolean, using an array of value references. 
	virtual fmiStatus getValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) const = 0;

	/// Get values of strings (using internaly type fmiString), using an array of value references. 
	virtual fmiStatus getValue( fmiValueReference* valref, std::string* val, std::size_t ival ) const = 0;


	/// Get single value of type fmiReal, using the variable name.
	virtual fmiStatus getValue( const std::string& name, fmiReal& val ) const = 0;

	/// Get single value of type fmiInteger, using the variable name.
	virtual fmiStatus getValue( const std::string& name, fmiInteger& val ) const = 0;

	/// Get single value of type fmiBoolean, using the variable name.
	virtual fmiStatus getValue( const std::string& name, fmiBoolean& val ) const = 0;

	/// Get single value of string (using internaly type fmiString), using the variable name.
	virtual fmiStatus getValue( const std::string& name, std::string& val ) const = 0;


	/// Set single value of type fmiReal, using the value reference.
	virtual fmiStatus setValue( fmiValueReference valref, fmiReal& val ) = 0;

	/// Set single value of type fmiInteger, using the value reference.
	virtual fmiStatus setValue( fmiValueReference valref, fmiInteger& val ) = 0;

	/// Set single value of type fmiBoolean, using the value reference.
	virtual fmiStatus setValue( fmiValueReference valref, fmiBoolean& val ) = 0;

	/// Set single single string value (using internaly type fmiString), using the value reference.
	virtual fmiStatus setValue( fmiValueReference valref, std::string& val ) = 0;


	/// Set values of type fmiReal, using an array of value references. 
	virtual fmiStatus setValue( fmiValueReference* valref, fmiReal* val, std::size_t ival ) = 0;

	/// Set values of type fmiInteger, using an array of value references. 
	virtual fmiStatus setValue( fmiValueReference* valref, fmiInteger* val, std::size_t ival ) = 0;

	/// Set values of type fmiBoolean, using an array of value references. 
	virtual fmiStatus setValue( fmiValueReference* valref, fmiBoolean* val, std::size_t ival ) = 0;

	/// Set values of string (using internaly type fmiString), using an array of value references. 
	virtual fmiStatus setValue( fmiValueReference* valref, std::string* val, std::size_t ival ) = 0;


	/// Set single value of type fmiReal, using the variable name.
	virtual fmiStatus setValue( const std::string& name, fmiReal val ) = 0;

	/// Set single value of type fmiInteger, using the variable name.
	virtual fmiStatus setValue( const std::string& name, fmiInteger val ) = 0;

	/// Set single value of type fmiBoolean, using the variable name.
	virtual fmiStatus setValue( const std::string& name, fmiBoolean val ) = 0;

	/// Set single value of string (using internaly type fmiString), using the variable name.
	virtual fmiStatus setValue( const std::string& name, std::string val ) = 0;

};


#endif
