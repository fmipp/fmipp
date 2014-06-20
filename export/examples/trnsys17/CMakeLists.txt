cmake_minimum_required(VERSION 2.8)

#
# This project builds an FMI wrapper for TRNSYS, including the FMI front
# end component (FMU DLL) and the TRNSYS back end component (Type6139).
#


# Variables to be defined by the user.
set( User_MODEL_IDENTIFIER "FMIProject2" CACHE STRING "FMI model identfier" )
set( User_FMIPP_SOURCE_DIR "" CACHE PATH "FMI++ source directory" )
set( User_FMIPP_BINARY_DIR "" CACHE PATH "FMI++ binary directory" )
set( User_TRNDLL_SHARED_LIB "" CACHE FILEPATH "TRNSYS shared library (TRNDll.dll)" )


# Define project.
project( TRNSYS_Wrapper_${User_MODEL_IDENTIFIER} )


# Optional builds.
option( BUILD_TEST "Build test application for FMI++ TRNSYS wrapper." ON )


# OS-specific settings.
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
#add_definitions( /W3 )


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
             ${PROJECT_SOURCE_DIR}/Type6139.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/FMIComponentBackEnd.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/SHMSlave.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/SHMManager.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/HelperFunctions.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/ScalarVariable.cpp )

# FMI front end component (FMU DLL).
add_library( ${User_MODEL_IDENTIFIER} SHARED
             ${User_FMIPP_SOURCE_DIR}/export/functions/fmiFunctions.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/FMIComponentFrontEnd.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/SHMMaster.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/SHMManager.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/HelperFunctions.cpp
             ${User_FMIPP_SOURCE_DIR}/export/src/ScalarVariable.cpp
             ${User_FMIPP_SOURCE_DIR}/import/base/src/ModelDescription.cpp )


# Include directories.
include_directories( ${PROJECT_SOURCE_DIR} )
include_directories( ${User_FMIPP_SOURCE_DIR} )
include_directories( ${User_FMIPP_SOURCE_DIR}/export/include )


# Link libraries.
target_link_libraries( ${User_MODEL_IDENTIFIER}
                       Shlwapi
                       ${CMAKE_DL_LIBS}
                       ${Boost_LIBRARIES} )


target_link_libraries( Type6139Lib
                       Shlwapi
                       ${User_TRNDLL_SHARED_LIB} )


# Build test.
if ( BUILD_TEST )
   add_executable( testTRNSYS WIN32 ${PROJECT_SOURCE_DIR}/test/main.cpp )
   target_link_libraries( testTRNSYS ${User_MODEL_IDENTIFIER} )
endif ()