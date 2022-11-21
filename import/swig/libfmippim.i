// -------------------------------------------------------------------
// Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
// All rights reserved. See file FMIPP_LICENSE for details.
// -------------------------------------------------------------------

%include typemaps.i
%include stdint.i
%include std_common.i
%include std_string.i
%include std_vector.i
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

%module(package="fmipp", moduleimport="import _fmippim") fmippim

%{
#include "common/FMIPPVariableType.h"
#include "common/FMIPPStatus.h"
#include "common/FMIPPTypes.h"
#include "import/base/include/FMUBase.h"
#include "import/base/include/FMUModelExchangeBase.h"
#include "import/base/include/FMUModelExchange_v1.h"
#include "import/base/include/FMUModelExchange_v2.h"
#include "import/base/include/FMUCoSimulationBase.h"
#include "import/base/include/FMUCoSimulation_v1.h"
#include "import/base/include/FMUCoSimulation_v2.h"
#include "import/base/include/LogBuffer.h"
#include "import/integrators/include/IntegratorType.h"
#include "import/utility/include/RollbackFMU.h"
#include "import/utility/include/IncrementalFMU.h"
#include "import/utility/include/FixedStepSizeFMU.h"
#include "import/utility/include/VariableStepSizeFMU.h"
#include "import/utility/include/InterpolatingFixedStepSizeFMU.h"
%}

%ignore getValue;
%ignore setValue;

%rename(setRealValue) setValue( const fmippString&, const fmippReal& );
%rename(setIntegerValue) setValue( const fmippString&, const fmippInteger& );
%rename(setBooleanValue) setValue( const fmippString&, const fmippBoolean& );
%rename(setStringValue) setValue( const fmippString&, const fmippString& );
%rename(integrateN) integrate( fmippTime, unsigned int );

 // Resolve namespaces for FMI 1.0 und 2.0
%rename(FMUModelExchangeV1) fmi_1_0::FMUModelExchange;
%rename(FMUCoSimulationV1) fmi_1_0::FMUCoSimulation;
%rename(FMUModelExchangeV2) fmi_2_0::FMUModelExchange;
%rename(FMUCoSimulationV2) fmi_2_0::FMUCoSimulation;

#if defined(SWIGPYTHON)

%feature("autodoc", "0");

%ignore fmippFalse;
%ignore fmippTrue;

%rename(statusOK) fmippOK;
%rename(statusWarning) fmippWarning;
%rename(statusDiscard) fmippDiscard;
%rename(statusError) fmippError;
%rename(statusFatal) fmippFatal;

%rename(clockActive) fmippClockActive;
%rename(clockInactive) fmippClockInactive;

%rename(typeBoolean) fmippTypeBoolean;
%rename(typeInteger) fmippTypeInteger;
%rename(typeReal) fmippTypeReal;
%rename(typeString) fmippTypeString;
%rename(typeUnknown) fmippTypeUnknown;

%rename(integratorEU) eu;
%rename(integratorRK) rk;
%rename(integratorABM) abm;
%rename(integratorCK) ck;
%rename(integratorDP) dp;
%rename(integratorFE) fe;
%rename(integratorBS) bs;
%rename(integratorRO) ro;
%rename(integratorBDF) bdf;
%rename(integratorABM2) abm2;

%rename(fmuTypeME1) fmi_1_0_me;
%rename(fmuTypeCS1) fmi_1_0_cs;
%rename(fmuTypeME2) fmi_2_0_me;
%rename(fmuTypeCS2) fmi_2_0_cs;
%rename(fmuTypeMEAndCS2) fmi_2_0_me_and_cs;

namespace std {
  %template(StringVector) vector<string>;
  %template(UnsignedIntVector) vector<unsigned int>;
}
#else
#endif

%ignore FMUBase::getModelDescription;
%ignore fmi_1_0::FMUModelExchange::getModelDescription;
%ignore fmi_1_0::FMUCoSimulation::getModelDescription;
%ignore fmi_2_0::FMUModelExchange::getModelDescription;
%ignore fmi_2_0::FMUCoSimulation::getModelDescription;
%ignore IncrementalFMU::getModelDescription;
%ignore getCurrentState;

%include "common/FMIPPVariableType.h"
%include "common/FMIPPStatus.h"
%include "common/FMIPPTypes.h"
%include "common/FMUType.h"
%include "import/base/include/FMUModelExchange_v1.h"
%include "import/base/include/FMUModelExchange_v2.h"
%include "import/base/include/FMUCoSimulation_v1.h"
%include "import/base/include/FMUCoSimulation_v2.h"
%include "import/base/include/LogBuffer.h"
%include "import/integrators/include/IntegratorType.h"
%include "import/utility/include/IncrementalFMU.h"
%include "import/utility/include/RollbackFMU.h"
%include "import/utility/include/FixedStepSizeFMU.h"
%include "import/utility/include/VariableStepSizeFMU.h"
%include "import/utility/include/InterpolatingFixedStepSizeFMU.h"
