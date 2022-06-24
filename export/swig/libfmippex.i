// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

%include typemaps.i
%include stdint.i
%include std_common.i
%include std_string.i
%include cpointer.i
%pointer_functions(double, double_pointer)
%pointer_functions(int, int_pointer)
%pointer_functions(std::string, string_pointer)
%pointer_functions(bool, bool_pointer)
%include carrays.i
%array_functions(double, double_array)
%array_functions(int, int_array)
%array_functions(std::string, string_array)
%array_functions(bool, bool_array)

 //%include <windows.i>
#define __FMI_DLL

%module(package="fmipp.export", moduleimport="import _fmippex") fmippex

#if defined(SWIGPYTHON)

%feature("autodoc", "0");

#else
#endif

%{
#include "common/FMIPPVariableType.h"
#include "common/FMIPPStatus.h"
#include "common/FMIPPTypes.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "common/fmi_v2.0/fmi2ModelTypes.h"
#include "export/include/FMIComponentBackEnd.h"
%}

%include "common/fmi_v1.0/fmiModelTypes.h"
%include "common/fmi_v2.0/fmi2ModelTypes.h"
%include "common/FMIPPVariableType.h"
%include "common/FMIPPStatus.h"
%include "common/FMIPPTypes.h"
%include "export/include/FMIComponentBackEnd.h"
