# -------------------------------------------------------------------
# Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)


project(fmipp_export)


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )


add_library( fmippex SHARED
   src/BackEndApplicationBase.cpp
   src/FMIComponentFrontEnd.cpp
   src/FMIComponentFrontEndBase.cpp
   src/FMIComponentBackEnd.cpp
   src/HelperFunctions.cpp
   src/IPCLogger.cpp
   src/IPCMasterLogger.cpp
   src/IPCSlaveLogger.cpp
   src/SHMMaster.cpp
   src/SHMSlave.cpp
   src/SHMManager.cpp
   src/ScalarVariable.cpp
   )

# FMI 1.0: front end component library (pre-stage for compiling an FMU DLL according to FMI 1.0).
add_library( libfmipp_fmu_frontend STATIC
   src/FMIComponentFrontEndBase.cpp
   src/FMIComponentFrontEnd.cpp
   src/IPCLogger.cpp
   src/IPCMasterLogger.cpp
   src/SHMMaster.cpp
   src/SHMManager.cpp
   src/HelperFunctions.cpp
   src/ScalarVariable.cpp
   ../import/base/src/ModelDescription.cpp
   ../import/base/src/PathFromUrl.cpp
   )

# FMI 2.0: complete FMU DLL.
add_library( fmi2 SHARED
   functions/fmi_v2.0/fmi2Functions.cpp
   src/ScalarVariable.cpp
   src/FMIComponentFrontEndBase.cpp
   src/FMIComponentFrontEnd.cpp
   src/IPCLogger.cpp
   src/IPCMasterLogger.cpp
   src/SHMMaster.cpp
   src/SHMManager.cpp
   src/HelperFunctions.cpp
   ../import/base/src/ModelDescription.cpp
   ../import/base/src/PathFromUrl.cpp
   )

find_package( Boost COMPONENTS date_time system filesystem REQUIRED )

target_link_libraries( fmippex ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} fmippim )
target_link_libraries( fmi2 ${Boost_LIBRARIES} )

# OS-specific dependencies here
if ( WIN32 )
   set_target_properties( fmippex PROPERTIES COMPILE_FLAGS "-DBUILD_FMI_DLL" )
   target_link_libraries( fmippex shlwapi )
   set_target_properties( libfmipp_fmu_frontend PROPERTIES COMPILE_FLAGS "-DBUILD_FMI_DLL -DBOOST_ALL_NO_LIB " )
   set_target_properties( fmi2 PROPERTIES COMPILE_FLAGS "-DBUILD_FMI_DLL -DBOOST_ALL_NO_LIB " )
   target_link_libraries( fmi2 shlwapi )
elseif ( APPLE )
   target_link_libraries( fmippex )
else ()
   target_link_libraries( fmippex rt )
endif ()

# Define compiler flags.
target_compile_definitions( fmi2 PRIVATE
   FRONT_END_TYPE=FMIComponentFrontEnd
   FRONT_END_TYPE_INCLUDE="export/include/FMIComponentFrontEnd.h"
   )

## Use flag below to specify shared memory segment names.
#add_definitions( -DSHM_SEGMENT_NAME="FMI_SEGMENT" )

## Use flag below to construct shared memory segment names using the parent's PID.
#add_definitions( -DBACKEND_USE_PARENT_PID )

#install( TARGETS fmippex DESTINATION lib )

#install( FILES include/FMIComponentBackEnd.h include/FMIComponentFrontEnd.h include/HelperFunctions.h include/IPCLogger.h include/IPCMaster.h include/IPCSlave.h include/SHMManager.h include/SHMMaster.h include/SHMSlave.h include/ScalarVariable.h DESTINATION include/fmipp_export )


if ( BUILD_SWIG )
   add_subdirectory( swig )
endif ()
