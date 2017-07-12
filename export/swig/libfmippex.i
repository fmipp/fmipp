// -------------------------------------------------------------------
// Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

%include std_common.i
%include std_string.i
%include cpointer.i
%pointer_functions(double, double_pointer)
%pointer_functions(int, int_pointer)
%pointer_functions(std::string, string_pointer)
%pointer_functions(char, char_pointer)
%include carrays.i
%array_functions(double, double_array)
%array_functions(int, int_array)
%array_functions(std::string, string_array)
%array_functions(char, char_array)

 //%include <windows.i>
#define __FMI_DLL


%module fmippex

%{
//#include "common/FMIType.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/fmi_v2.0/fmi2ModelTypes.h"
#include "export/include/FMIComponentBackEnd.h"
%}

//%rename(setRealValue) setValue( const std::string&, fmiReal );

#if defined(SWIGPYTHON)
%typemap(out) fmiBoolean {
	if($1)
		$result = (PyObject *)Py_True;
	else
		$result = (PyObject *)Py_False;
	//    Py_CLEAR($1);
    Py_INCREF($result);
 }
%ignore fmiFalse;
%ignore fmiTrue;

%typemap(out) fmi2Boolean {
	if($1)
		$result = (PyObject *)Py_True;
	else
		$result = (PyObject *)Py_False;
	//    Py_CLEAR($1);
    Py_INCREF($result);
 }
%ignore fmi2False;
%ignore fmi2True;
#else
#endif


%ignore initializeRealParameters( const std::vector<std::string>&, std::vector<fmi2Real*>& );
%ignore initializeIntegerParameters( const std::vector<std::string>&, std::vector<fmi2Integer*>& );
%ignore initializeBooleanParameters( const std::vector<std::string>&, std::vector<fmi2Boolean*>& );
%ignore initializeStringParameters( const std::vector<std::string>&, std::vector<std::string*>& );
%ignore initializeRealInputs( const std::vector<std::string>&, std::vector<fmi2Real*>& );
%ignore initializeIntegerInputs( const std::vector<std::string>&, std::vector<fmi2Integer*>& );
%ignore initializeBooleanInputs( const std::vector<std::string>&, std::vector<fmi2Boolean*>& );
%ignore initializeStringInputs( const std::vector<std::string>&, std::vector<std::string*>& );
%ignore initializeRealOutputs( const std::vector<std::string>&, std::vector<fmi2Real*>& );
%ignore initializeIntegerOutputs( const std::vector<std::string>&, std::vector<fmi2Integer*>& );
%ignore initializeBooleanOutputs( const std::vector<std::string>&, std::vector<fmi2Boolean*>& );
%ignore initializeStringOutputs( const std::vector<std::string>&, std::vector<std::string*>& );
%ignore getRealInputs( std::vector<fmiReal*>& );
%ignore getIntegerInputs( std::vector<fmiInteger*>& );
%ignore getBooleanInputs( std::vector<fmiBoolean*>& );
%ignore getStringInputs( std::vector<std::string*>& );
%ignore setRealOutputs( const std::vector<fmiReal*>& );
%ignore setIntegerOutputs( const std::vector<fmiInteger*>& );
%ignore setBooleanOutputs( const std::vector<fmiBoolean*>& );
%ignore setStringOutputs( const std::vector<std::string*>& );


//%include "common/FMIType.h"
//%include "common/fmi_v1.0/fmiModelTypes.h"
%include "common/fmi_v2.0/fmi2ModelTypes.h"
%include "export/include/FMIComponentBackEnd.h"
