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

%module fmippim

%inline %{
typedef unsigned int size_t;
%}

%{
  //  typedef double fmiReal;
#include "common/FMIType.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "import/base/include/FMUBase.h"
#include "import/base/include/FMUModelExchangeBase.h"
#include "import/base/include/FMUModelExchange.h"
#include "import/base/include/FMUCoSimulationBase.h"
#include "import/base/include/FMUCoSimulation.h"
#include "import/utility/include/IncrementalFMU.h"
#include "import/utility/include/FixedStepSizeFMU.h"
#include "import/utility/include/InterpolatingFixedStepSizeFMU.h"
%}
%rename(setRealValue) setValue( const std::string& name, fmiReal val );
%rename(setIntegerValue) setValue( const std::string& name, fmiInteger val );
%rename(setBooleanValue) setValue( const std::string& name, fmiBoolean val );
%rename(setStringValue) setValue( const std::string& name, std::string val );

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
#else
#endif

%ignore getCurrentState;
%ignore getValue(const std::string&  name, fmiReal* val);
%include "common/FMIType.h"
%include "common/fmi_v1.0/fmiModelTypes.h"
 //%include "import/base/include/FMUBase.h"
 //%include "import/base/include/FMUModelExchangeBase.h"
%include "import/base/include/FMUModelExchange.h"
 //%include "import/base/include/FMUCoSimulationBase.h"
%include "import/base/include/FMUCoSimulation.h"
%include "import/utility/include/IncrementalFMU.h"
%include "import/utility/include/FixedStepSizeFMU.h"
%include "import/utility/include/InterpolatingFixedStepSizeFMU.h"
