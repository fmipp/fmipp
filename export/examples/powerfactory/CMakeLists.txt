cmake_minimum_required(VERSION 2.8)

#
# This project builds an FMI wrapper for the PowerFactory API. It requires Microsoft Visual Studio 2010.
#


# Variables to be defined by the user.
set( User_MODEL_IDENTIFIER "PFTest" CACHE STRING "FMI model identfier" )
set( User_FMIPP_SOURCE_DIR "" CACHE PATH "FMI++ source directory" )
set( User_FMIPP_BINARY_DIR "" CACHE PATH "FMI++ binary directory" )
set( User_PFSIM_SOURCE_DIR "" CACHE PATH "PFSim source directory" )
set( User_PFSIM_BINARY_DIR "" CACHE PATH "PFSim binary directory" )


# Define project.
project( PF_Wrapper_${User_MODEL_IDENTIFIER} )


# OS-specific settings.
if ( NOT DEFINED MSVC10 )
   message( FATAL_ERROR "This project requires Microsift Visual Studio 10" )
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
add_definitions( /W3 )


# Boost dependencies here
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost COMPONENTS thread REQUIRED )
if ( Boost_FOUND )
   include_directories( ${Boost_INCLUDE_DIRS} )
   link_directories( ${Boost_LIBRARY_DIRS} )
endif ()


# Source code.
add_library( ${User_MODEL_IDENTIFIER} SHARED
	     PowerFactoryFrontEnd.cpp
	     ${User_FMIPP_SOURCE_DIR}/export/functions/fmiFunctions.cpp
	     ${User_FMIPP_SOURCE_DIR}/export/src/HelperFunctions.cpp
	     ${User_FMIPP_SOURCE_DIR}/export/src/ScalarVariable.cpp
	     ${User_FMIPP_SOURCE_DIR}/import/base/src/ModelDescription.cpp )


# Include directories.
include_directories( ${PROJECT_SOURCE_DIR} )
include_directories( ${User_FMIPP_SOURCE_DIR} )
include_directories( ${User_PFSIM_SOURCE_DIR} )


# Link libraries.
target_link_libraries( ${User_MODEL_IDENTIFIER}
		       Shlwapi
		       ${CMAKE_DL_LIBS}
		       ${Boost_LIBRARIES}
		       ${User_PFSIM_BINARY_DIR}/PFSim.lib
		       ${User_PFSIM_BINARY_DIR}/digexdyn.lib
		       ${User_PFSIM_BINARY_DIR}/digexfun1.lib )


