# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)

#
# This project builds an FMI wrapper for TRNSYS, including the FMI front end
# component (FMU DLL) and the TRNSYS back end component (Type6139).
#
# ATTENTION: When using Visual Studio, be sure to compile in "Release" mode.
#


# Variables to be defined by the user.
set( User_MODEL_IDENTIFIER "Type6139_Test" CACHE STRING "FMI model identfier" )
set( User_FMIPP_SOURCE_DIR "c:/Development/fmipp" CACHE PATH "FMI++ source directory" )
set( User_TRNSYS17_PATH "c:/Trnsys17" CACHE FILEPATH "TRNSYS 17 root directory" )


# Define project.
project( TRNSYS_Wrapper_${User_MODEL_IDENTIFIER} )


# Optional builds.
option( BUILD_TEST "Build test application for FMI++ TRNSYS wrapper." ON )


# OS- and compiler-specific settings.
if ( NOT DEFINED WIN32 )
   message( FATAL_ERROR "This project requires Windows." )
else ()
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMU_BIN_DIR "win64")
   else ()
      set(FMU_BIN_DIR "win32")
   endif ()
   set(FMU_BIN_EXT ".dll")

   set( PATH_SEPARATOR ";" )
   set( CMAKE_SHARED_LIBRARY_PREFIX "" )
   set( CMAKE_SHARED_MODULE_PREFIX "" )
   add_definitions( -DWIN32 )

   if ( MINGW )
      add_definitions( -DMINGW -Wall ) # -O1
      if ( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0" )
         set( CMAKE_CXX_FLAGS "-std=c++0x" )
      else ()
         set( CMAKE_CXX_FLAGS "-std=c++11" )
      endif ()
   endif ()

   if ( MSVC )
      # disable auto-linking and use cmake's dependency handling
      add_definitions( -DBOOST_ALL_NO_LIB /W3 )

      # Add reminder to compile in "Release" mode when using Visual Studio.
      message( "ATTENTION: The TRNSYS wrapper has to be compiled in RELEASE mode!!!" )

   endif ()

endif ()

add_definitions(-DFMU_BIN_DIR="${FMU_BIN_DIR}")
add_definitions(-DFMU_BIN_EXT="${FMU_BIN_EXT}")


# Compiler flags.
add_definitions( -DMODEL_IDENTIFIER=${User_MODEL_IDENTIFIER} )
add_definitions( -DFRONT_END_TYPE=FMIComponentFrontEnd )
add_definitions( -DFRONT_END_TYPE_INCLUDE="FMIComponentFrontEnd.h" )
add_definitions( -DBUILD_FMI_DLL )
add_definitions( -DBOOST_ALL_NO_LIB )


# Boost dependencies here
if ( MSVC )
   set( Boost_USE_STATIC_LIBS OFF )
   set( Boost_USE_MULTITHREADED ON )
   set( Boost_USE_STATIC_RUNTIME OFF )
endif ()

find_package( Boost COMPONENTS date_time system filesystem REQUIRED )

if ( Boost_FOUND )
   include_directories( ${Boost_INCLUDE_DIRS} )
   link_directories( ${Boost_LIBRARY_DIRS} )
endif ()


# FMI back end component (TRNSYS type Type6139).
add_library( Type6139Lib SHARED
             ${PROJECT_SOURCE_DIR}/Type6139/Type6139.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/FMIComponentBackEnd.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/IPCLogger.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/IPCSlaveLogger.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/SHMSlave.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/SHMManager.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/HelperFunctions.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/ScalarVariable.cpp )

# FMI front end component (FMU DLL).
add_library( ${User_MODEL_IDENTIFIER} SHARED
             ${User_FMIPP_SOURCE_DIR}/export/functions/fmiFunctions.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/FMIComponentFrontEndBase.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/FMIComponentFrontEnd.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/IPCLogger.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/IPCMasterLogger.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/SHMMaster.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/SHMManager.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/HelperFunctions.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/ScalarVariable.cpp
             ${User_FMIPP_SOURCE_DIR}/import/base/src/ModelDescription.cpp
             ${User_FMIPP_SOURCE_DIR}/import/base/src/CallbackFunctions.cpp )


# Include directories.
include_directories( ${PROJECT_SOURCE_DIR}/Type6139 )
include_directories( ${User_FMIPP_SOURCE_DIR} )
include_directories( ${User_FMIPP_SOURCE_DIR}/export/include )


# Link libraries.
target_link_libraries( ${User_MODEL_IDENTIFIER}
                       Shlwapi
                       ${CMAKE_DL_LIBS}
                       ${Boost_LIBRARIES} )


target_link_libraries( Type6139Lib
                       Shlwapi
                       ${User_TRNSYS17_PATH}/Exe/TRNDll.lib )


# Install Type6139 for TRNSYS.
add_custom_command( TARGET Type6139Lib POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Type6139Lib> "${User_TRNSYS17_PATH}/UserLib/ReleaseDLLs"
					COMMAND ${CMAKE_COMMAND} -E make_directory "${User_TRNSYS17_PATH}/Studio/Proformas/My Components"
                    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/Type6139/Type6139.tmf "${User_TRNSYS17_PATH}/Studio/Proformas/My Components"
                    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/Type6139/Type6139.bmp "${User_TRNSYS17_PATH}/Studio/Proformas/My Components"
)



# Build test.
if ( BUILD_TEST )

   if ( WIN32 )
      message( "Adding test: Make sure TRNExe.exe and the Boost libraries have been included in your PATH variable BEFORE you run CMake!!!" )
   endif ()

   # Find packages needed for unit test framework.
   find_package( Boost COMPONENTS system filesystem unit_test_framework REQUIRED )

   # Path to XML model description and additional files.
   set( FMU_URI "file://${PROJECT_SOURCE_DIR}/test" CACHE INTERNAL "" )
   add_definitions( -DFMU_URI="${FMU_URI}" )

   # Add and link executable.
   add_executable( testTrnsysFMU WIN32 ${PROJECT_SOURCE_DIR}/test/testTrnsysFMU.cpp )
   target_link_libraries( testTrnsysFMU
                          ${User_MODEL_IDENTIFIER}
                          ${Boost_FILESYSTEM_LIBRARY}
                          ${Boost_SYSTEM_LIBRARY}
                          ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

   # Post build action: copy executable to new subdirectory test.
   add_custom_command( TARGET testTrnsysFMU POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/test
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:testTrnsysFMU> ${CMAKE_BINARY_DIR}/test )

   # Add test.
   enable_testing()
   include( ${User_FMIPP_SOURCE_DIR}/test/CMakeAddTests.txt )
   add_test_fmipp( testTrnsysFMU )
endif ()
