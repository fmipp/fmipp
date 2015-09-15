// --------------------------------------------------------------
// Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// --------------------------------------------------------------

%include std_common.i
%include std_string.i
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

%{
  //  typedef double fmiReal;
#include "common/FMIType.h"
#include "common/fmi_v1.0/fmiModelTypes.h"
#include "import/base/include/FMUBase.h"
#include "import/base/include/FMUModelExchangeBase.h"
#include "import/base/include/FMUModelExchange_v1.h"
#include "import/base/include/FMUCoSimulationBase.h"
#include "import/base/include/FMUCoSimulation.h"
#include "import/base/include/LogBuffer.h"
#include "import/integrators/include/IntegratorType.h"
#include "import/utility/include/IncrementalFMU.h"
#include "import/utility/include/FixedStepSizeFMU.h"
#include "import/utility/include/InterpolatingFixedStepSizeFMU.h"
%}
%rename(setRealValue) setValue( const std::string&, fmiReal );
%rename(setIntegerValue) setValue( const std::string&, fmiInteger );
%rename(setBooleanValue) setValue( const std::string&, fmiBoolean );
%rename(setStringValue) setValue( const std::string&, std::string );
%rename(integrateN) integrate( fmiTime, unsigned int );

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
%ignore getValue( const std::string& , fmiReal* );
%include "common/FMIType.h"
%include "common/fmi_v1.0/fmiModelTypes.h"
 //%include "import/base/include/FMUBase.h"
 //%include "import/base/include/FMUModelExchangeBase.h"
%include "import/base/include/FMUModelExchange_v1.h"
 //%include "import/base/include/FMUCoSimulationBase.h"
%include "import/base/include/FMUCoSimulation.h"
%include "import/base/include/LogBuffer.h"
%include "import/integrators/include/IntegratorType.h"
%include "import/utility/include/IncrementalFMU.h"
%include "import/utility/include/FixedStepSizeFMU.h"
%include "import/utility/include/InterpolatingFixedStepSizeFMU.h"
