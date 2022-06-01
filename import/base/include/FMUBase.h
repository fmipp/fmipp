// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

#ifndef _FMIPP_FMUBASE_H
#define _FMIPP_FMUBASE_H

#include "common/FMIPPConfig.h"

class ModelDescription;

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
	virtual fmippSize nStates() const = 0;

	/// Get number of event indicators.
	virtual fmippSize nEventInds() const = 0;

	/// Get number of value references (equals the numer of variables).
	virtual fmippSize nValueRefs() const = 0;

	/**
	 * \brief Returns a pointer to the model description
	 * \details The pointer must be valid as soon as the FMU is successfully 
	 * loaded. It may be null in case getLastStatus() indicates a non-successful 
	 * operation on constructing the FMU. The returned pointer must be valid 
	 * until the object is deleted. In order to avoid inconsistent behavior, the 
	 * model description pointer is set to constant and the model description 
	 * must not be modified.
	 */
	virtual const ModelDescription* getModelDescription() const = 0;

	/// Get current time.
	virtual fmippTime getTime() const = 0;


	/// Get value reference associated to variable name.
	virtual fmippValueReference getValueRef( const fmippString& name ) const = 0;

	/// Get type of variable.
	virtual FMIPPVariableType getType( const fmippString& variableName ) const = 0;


	/// Get the status of the last operation on the FMU.
	virtual fmippStatus getLastStatus() const = 0;


	/// Get single value of type fmippReal, using the value reference.
	virtual fmippStatus getValue( fmippValueReference valref, fmippReal& val ) = 0;

	/// Get single value of type fmippInteger, using the value reference.
	virtual fmippStatus getValue( fmippValueReference valref, fmippInteger& val ) = 0;

	/// Get single value of type fmippBoolean, using the value reference.
	virtual fmippStatus getValue( fmippValueReference valref, fmippBoolean& val )  = 0;

	/// Get single value of string (using internaly type fmippString), using the value reference.
	virtual fmippStatus getValue( fmippValueReference valref, fmippString& val ) = 0;


	/// Get values of type fmippReal, using an array of value references. 
	virtual fmippStatus getValue( fmippValueReference* valref, fmippReal* val, fmippSize ival ) = 0;

	/// Get values of type fmippInteger, using an array of value references. 
	virtual fmippStatus getValue( fmippValueReference* valref, fmippInteger* val, fmippSize ival ) = 0;

	/// Get values of type fmippBoolean, using an array of value references. 
	virtual fmippStatus getValue( fmippValueReference* valref, fmippBoolean* val, fmippSize ival ) = 0;

	/// Get values of strings (using internaly type fmippString), using an array of value references. 
	virtual fmippStatus getValue( fmippValueReference* valref, fmippString* val, fmippSize ival ) = 0;


	/// Get single value of type fmippReal, using the variable name.
	virtual fmippStatus getValue( const fmippString& name, fmippReal& val ) = 0;

	/// Get single value of type fmippInteger, using the variable name.
	virtual fmippStatus getValue( const fmippString& name, fmippInteger& val ) = 0;

	/// Get single value of type fmippBoolean, using the variable name.
	virtual fmippStatus getValue( const fmippString& name, fmippBoolean& val ) = 0;

	/// Get single value of string (using internaly type fmippString), using the variable name.
	virtual fmippStatus getValue( const fmippString& name, fmippString& val ) = 0;


	/// Get single value of type fmippReal, using the variable name.
	virtual fmippReal getRealValue( const fmippString& name ) = 0;

	/// Get single value of type fmippInteger, using the variable name.
	virtual fmippInteger getIntegerValue( const fmippString& name ) = 0;

	/// Get single value of type fmippBoolean, using the variable name.
	virtual fmippBoolean getBooleanValue( const fmippString& name ) = 0;

	/// Get single value of type fmippString, using the variable name.
	virtual fmippString getStringValue( const fmippString& name ) = 0;


	/// Set single value of type fmippReal, using the value reference.
	virtual fmippStatus setValue( fmippValueReference valref, const fmippReal& val ) = 0;

	/// Set single value of type fmippInteger, using the value reference.
	virtual fmippStatus setValue( fmippValueReference valref, const fmippInteger& val ) = 0;

	/// Set single value of type fmippBoolean, using the value reference.
	virtual fmippStatus setValue( fmippValueReference valref, const fmippBoolean& val ) = 0;

	/// Set single single string value (using internaly type fmippString), using the value reference.
	virtual fmippStatus setValue( fmippValueReference valref, const fmippString& val ) = 0;


	/// Set values of type fmippReal, using an array of value references. 
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippReal* val, fmippSize ival ) = 0;

	/// Set values of type fmippInteger, using an array of value references. 
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippInteger* val, fmippSize ival ) = 0;

	/// Set values of type fmippBoolean, using an array of value references. 
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippBoolean* val, fmippSize ival ) = 0;

	/// Set values of string (using internaly type fmippString), using an array of value references. 
	virtual fmippStatus setValue( fmippValueReference* valref, const fmippString* val, fmippSize ival ) = 0;


	/// Set single value of type fmippReal, using the variable name.
	virtual fmippStatus setValue( const fmippString& name, const fmippReal& val ) = 0;

	/// Set single value of type fmippInteger, using the variable name.
	virtual fmippStatus setValue( const fmippString& name, const fmippInteger& val ) = 0;

	/// Set single value of type fmippBoolean, using the variable name.
	virtual fmippStatus setValue( const fmippString& name, const fmippBoolean& val ) = 0;

	/// Set single value of string (using internaly type fmippString), using the variable name.
	virtual fmippStatus setValue( const fmippString& name, const fmippString& val ) = 0;


	/// Call logger to issue a debug message.
	virtual void sendDebugMessage( const fmippString& msg ) const = 0;
};


#endif
