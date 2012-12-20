%include "std_string.i"
%include carrays.i
%array_functions(double, double_array)
%array_functions(std::string, string_array)

%module helper

%{
  //  typedef double fmiReal;
#include "../include/IncrementalFMU.h"
%}
%ignore getCurrentState;
%ignore getValue(const std::string&  name, fmiReal* val);
typedef double TIMESTAMP;
typedef double fmiReal;
typedef unsigned int size_t;
%include "../include/IncrementalFMU.h"
