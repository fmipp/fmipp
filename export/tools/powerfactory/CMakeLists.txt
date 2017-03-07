# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)

#
# This project builds an FMI wrapper for the PowerFactory API. It requires Microsoft Visual Studio 2013.
#


# Variables to be defined by the user.
set( User_MODEL_IDENTIFIER "PFTest" CACHE STRING "FMI model identfier" )
set( User_FMIPP_SOURCE_DIR "" CACHE PATH "FMI++ source directory" )
set( User_PF_API_INCLUDE_DIR "C:/DIgSILENT/pf2016/Api/include" CACHE PATH "directory containing the implementation of the PF API" )


# Define project.
project( PF_Wrapper_${User_MODEL_IDENTIFIER} )


# Optional builds.
option( BUILD_TEST "Build test application for FMI++ PowerFactory wrapper." ON )


# OS-specific settings.
if ( NOT DEFINED MSVC12 )
   message( FATAL_ERROR "This project requires Microsift Visual Studio 2013" )
else ()
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMU_BIN_DIR "win64")
   else ()
      set(FMU_BIN_DIR "win32")
   endif ()
   set(FMU_BIN_EXT ".dll")
endif ()
add_definitions(-DFMU_BIN_DIR="${FMU_BIN_DIR}")
add_definitions(-DFMU_BIN_EXT="${FMU_BIN_EXT}")


# Compiler flags.
add_definitions( -DMODEL_IDENTIFIER=${User_MODEL_IDENTIFIER} )
add_definitions( -DFRONT_END_TYPE=PowerFactoryFrontEnd )
add_definitions( -DFRONT_END_TYPE_INCLUDE="PowerFactoryFrontEnd.h" )
add_definitions( -DBUILD_FMI_DLL )
add_definitions( -DBOOST_ALL_NO_LIB )
add_definitions( -D_CRT_SECURE_NO_WARNINGS )
add_definitions( -D_WINSOCK_DEPRECATED_NO_WARNINGS )
add_definitions( /W3 )

# Boost dependencies here
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost COMPONENTS thread system filesystem REQUIRED )
if ( Boost_FOUND )
   include_directories( ${Boost_INCLUDE_DIRS} )
   link_directories( ${Boost_LIBRARY_DIRS} )
endif ()


# Source code.
add_library( ${User_MODEL_IDENTIFIER} SHARED
	     PowerFactoryFrontEnd.cpp
	     PowerFactoryTimeAdvance.cpp
	     PowerFactoryExtraOutput.cpp
	     PowerFactoryRealScalar.cpp
		 api/src/PowerFactory.cpp
		 api/src/PowerFactoryRMS.cpp
		 api/src/RmsSimEventQueue.cpp
		 api/src/SimpleLogger.cpp
		 api/src/Utils.cpp
	     ${User_FMIPP_SOURCE_DIR}/export/functions/fmi_v1.0/fmiFunctions.cpp
	     ${User_FMIPP_SOURCE_DIR}/export/src/FMIComponentFrontEndBase.cpp
	     ${User_FMIPP_SOURCE_DIR}/export/src/HelperFunctions.cpp
	     ${User_FMIPP_SOURCE_DIR}/export/src/ScalarVariable.cpp
	     ${User_FMIPP_SOURCE_DIR}/import/base/src/ModelDescription.cpp
	     ${User_FMIPP_SOURCE_DIR}/import/base/src/PathFromUrl.cpp )


# Include directories.
include_directories( ${PROJECT_SOURCE_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/api/include )
include_directories( ${User_FMIPP_SOURCE_DIR} )
include_directories( ${User_PF_API_INCLUDE_DIR} )


# Link libraries.
target_link_libraries( ${User_MODEL_IDENTIFIER}
		       Shlwapi
		       ${CMAKE_DL_LIBS}
		       ${Boost_LIBRARIES}
			   ${User_PF_API_INCLUDE_DIR}/digapivalue.lib
			   )


# Build test.
if ( BUILD_TEST )
   # Find packages needed for unit test framework.
   find_package( Boost COMPONENTS unit_test_framework REQUIRED )

   # Path to XML model description and additional files.
   set( FMU_URI_BASE "file://${PROJECT_SOURCE_DIR}/test" CACHE INTERNAL "" )
   add_definitions( -DFMU_URI_BASE="${FMU_URI_BASE}" )

   # Add and link executable.
   add_executable( testPowerFactoryBasics WIN32
   		   ${PROJECT_SOURCE_DIR}/test/testPowerFactoryBasics.cpp
		   ${User_FMIPP_SOURCE_DIR}/import/base/src/CallbackFunctions.cpp
		   ${User_FMIPP_SOURCE_DIR}/import/base/src/LogBuffer.cpp )
   target_link_libraries( testPowerFactoryBasics
                          ${User_MODEL_IDENTIFIER}
                          ${Boost_FILESYSTEM_LIBRARY}
                          ${Boost_SYSTEM_LIBRARY}
                          ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

   # Add and link executable.
   add_executable( testPowerFactoryTriggers WIN32
   		   ${PROJECT_SOURCE_DIR}/test/testPowerFactoryTriggers.cpp
		   ${User_FMIPP_SOURCE_DIR}/import/base/src/CallbackFunctions.cpp
		   ${User_FMIPP_SOURCE_DIR}/import/base/src/LogBuffer.cpp )
   target_link_libraries( testPowerFactoryTriggers
                          ${User_MODEL_IDENTIFIER}
                          ${Boost_FILESYSTEM_LIBRARY}
                          ${Boost_SYSTEM_LIBRARY}
                          ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

   # Add and link executable.
   add_executable( testPowerFactoryDPLScript WIN32
   		   ${PROJECT_SOURCE_DIR}/test/testPowerFactoryDPLScript.cpp
		   ${User_FMIPP_SOURCE_DIR}/import/base/src/CallbackFunctions.cpp
		   ${User_FMIPP_SOURCE_DIR}/import/base/src/LogBuffer.cpp )
   target_link_libraries( testPowerFactoryDPLScript
                          ${User_MODEL_IDENTIFIER}
                          ${Boost_FILESYSTEM_LIBRARY}
                          ${Boost_SYSTEM_LIBRARY}
                          ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

   # Post build action: copy executable to (new) subdirectory test.
   add_custom_command( TARGET testPowerFactoryBasics POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:testPowerFactoryBasics> ${CMAKE_BINARY_DIR}/test )

   # Post build action: copy executable to (new) subdirectory test.
   add_custom_command( TARGET testPowerFactoryTriggers POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:testPowerFactoryTriggers> ${CMAKE_BINARY_DIR}/test )

   # Post build action: copy executable to (new) subdirectory test.
   add_custom_command( TARGET testPowerFactoryDPLScript POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:testPowerFactoryDPLScript> ${CMAKE_BINARY_DIR}/test )

   # Add test.
   enable_testing()
   include( ${User_FMIPP_SOURCE_DIR}/test/CMakeAddTests.txt )
   add_test_fmipp( testPowerFactoryBasics )
   add_test_fmipp( testPowerFactoryTriggers )
   add_test_fmipp( testPowerFactoryDPLScript )
endif ()
