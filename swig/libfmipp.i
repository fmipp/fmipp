%include "std_string.i"
%include cpointer.i
%pointer_functions(double, double_pointer)
%pointer_functions(int, int_pointer)
%pointer_functions(std::string, string_pointer)
%pointer_functions(char, char_pointer)
%include carrays.i
%array_functions(double, double_array)
%array_functions(std::string, string_array)
 //%include <windows.i>
#define __FMI_DLL

%module fmipp

%{
  //  typedef double fmiReal;
#include "../include/FMU.h"
#include "../include/IncrementalFMU.h"
#include "../include/FixedStepSizeFMU.h"
#include "../include/InterpolatingFixedStepSizeFMU.h"
%}
%ignore getCurrentState;
%ignore getValue(const std::string&  name, fmiReal* val);
typedef double fmiTime;
typedef double fmiReal;
typedef unsigned int size_t;
%include "../include/FMU.h"
%include "../include/IncrementalFMU.h"
%include "../include/FixedStepSizeFMU.h"
%include "../include/InterpolatingFixedStepSizeFMU.h"
