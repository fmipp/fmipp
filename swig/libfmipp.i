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
#include "common/FMIType.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "base/include/FMUBase.h"
#include "base/include/FMUModelExchangeBase.h"
#include "base/include/FMUModelExchange.h"
#include "base/include/FMUCoSimulationBase.h"
#include "base/include/FMUCoSimulation.h"
#include "utility/include/IncrementalFMU.h"
#include "utility/include/FixedStepSizeFMU.h"
#include "utility/include/InterpolatingFixedStepSizeFMU.h"
%}
%rename(setRealValue) setValue( const std::string& name, fmiReal val );
%rename(setIntegerValue) setValue( const std::string& name, fmiInteger val );
%rename(setBooleanValue) setValue( const std::string& name, fmiBoolean val );
%rename(setStringValue) setValue( const std::string& name, std::string val );

%ignore getCurrentState;
%ignore getValue(const std::string&  name, fmiReal* val);
%include "common/FMIType.h"
%include "common/fmi_v1.0/fmiModelTypes.h"
 //%include "base/include/FMUBase.h"
 //%include "base/include/FMUModelExchangeBase.h"
%include "base/include/FMUModelExchange.h"
 //%include "base/include/FMUCoSimulationBase.h"
%include "base/include/FMUCoSimulation.h"
%include "utility/include/IncrementalFMU.h"
%include "utility/include/FixedStepSizeFMU.h"
%include "utility/include/InterpolatingFixedStepSizeFMU.h"
