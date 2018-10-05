# -------------------------------------------------------------------
# Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required( VERSION 2.8.12 )


project( fmipp )


# add a target to generate documentation with Doxygen
find_package( Doxygen )
if ( DOXYGEN_FOUND )
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY )
   add_custom_target( doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif ()

# stop execution of cmake in case only the documentation is needed
if ( BUILD_DOXYGEN_DOCS_ONLY )
   return()
endif()


# optional builds: to switch the building of different subparts off, use a GUI or "cmake -Doption=OFF [...]"
# when you have switched them off, use the GUI or "cmake -Doption=ON [...]" to switch them on again
option( BUILD_SWIG "SWIG - Simplified Wrapper and Interface Generator stuff." ON )
option( BUILD_TESTS "Unit tests for FMI++." ON )

# include the sundials library and choose use an integrator from CVode
option( INCLUDE_SUNDIALS "Use SUNDIALS integrator suite." OFF )
if ( INCLUDE_SUNDIALS )
   message( "ATTENTION: Sundials will be included!" )

   set( SUNDIALS_INCLUDEDIR "" CACHE PATH "Optional: Sundials include directory." )
   include_directories( ${SUNDIALS_INCLUDEDIR} )

   set( SUNDIALS_LIBRARYDIR "" CACHE PATH "Optional: Sundials library directory." )
   link_directories( ${SUNDIALS_LIBRARYDIR} )

   add_definitions( -DUSE_SUNDIALS )
endif ()


if ( BUILD_SWIG )
   find_package( SWIG REQUIRED )

   option( BUILD_SWIG_JAVA "Build Java wrapper." ON )
   if ( BUILD_SWIG_JAVA )
      find_package( Java REQUIRED )
      find_package( JNI REQUIRED )
      set( BUILD_SWIG_JAVA_PACKAGE "" CACHE STRING "Optional: Put java wrapper into a package." )
   endif()

   option( BUILD_SWIG_PYTHON "Build Python wrapper." ON )
   option( BUILD_SWIG_PYTHON_3 "Force building for Python 3.x." OFF )
   if ( BUILD_SWIG_PYTHON )
      if ( BUILD_SWIG_PYTHON_3 )
         set( BUILD_SWIG_MIN_PYTHON_VERSION "3" )
      else()
         set( BUILD_SWIG_MIN_PYTHON_VERSION "2" )
      endif()
      find_package( PythonInterp ${BUILD_SWIG_MIN_PYTHON_VERSION} REQUIRED )
      find_package( PythonLibs ${BUILD_SWIG_MIN_PYTHON_VERSION} REQUIRED )
   endif()

   option( BUILD_SWIG_OCTAVE "Build Octave wrapper." OFF )
   if ( BUILD_SWIG_OCTAVE )
      find_package( HDF5 REQUIRED )
   endif ()

   option( BUILD_SWIG_MATLAB "Build MATLAB wrapper." OFF )
   if ( BUILD_SWIG_MATLAB )
      find_package( Matlab REQUIRED COMPONENTS MX_LIBRARY )
   endif ()
endif ()


option( DYMOLA2015_WORKAROUND "Use a workaround for the calculation of Jacobians for Dymola FMUs" OFF )
if ( DYMOLA2015_WORKAROUND )
  add_definitions( -DDYMOLA2015_WORKAROUND )
endif()


# set the name of the FMU-binaries-subdirectory according to the current OS
if ( WIN32 )
   if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( FMU_BIN_DIR "win64" )
   else ()
      set( FMU_BIN_DIR "win32" )
   endif ()
   set( FMU_BIN_EXT ".dll" )
elseif ( APPLE )
   if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( FMU_BIN_DIR "darwin64" )
   else ()
      set( FMU_BIN_DIR "darwin32" )
   endif ()
   set( FMU_BIN_EXT ".dylib" )
elseif ( CYGWIN )
   if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( FMU_BIN_DIR "cygwin64" )
   else ()
      set( FMU_BIN_DIR "cygwin32" )
   endif ()
   set( FMU_BIN_EXT ".dll" )
elseif ( UNIX )
   if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( FMU_BIN_DIR "linux64" )
   else ()
      set( FMU_BIN_DIR "linux32" )
   endif ()
   set( FMU_BIN_EXT ".so" )
endif ()

add_definitions( -DFMU_BIN_DIR="${FMU_BIN_DIR}" )
add_definitions( -DFMU_BIN_EXT="${FMU_BIN_EXT}" )

# OS-specific compiler settings
if ( WIN32 ) # windows-specific
   set( PATH_SEPARATOR ";" )
   set( CMAKE_SHARED_LIBRARY_PREFIX "" )
   set( CMAKE_SHARED_MODULE_PREFIX "" )
   add_definitions( -DWIN32 )
   if ( MINGW )
      add_definitions( -DMINGW -Wall ) # -O1
      if ( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0" )
         set( CMAKE_CXX_FLAGS "-std=c++0x" )
      else ()
         set( CMAKE_CXX_FLAGS "-std=gnu++11" )
      endif ()
   endif ()
   if ( MSVC )
      # disable auto-linking and use cmake's dependency handling
      add_definitions( -DBOOST_ALL_NO_LIB /W3 )
   endif ()
else () # linux-specific
   set( PATH_SEPARATOR ":" )
   add_definitions( -Wall ) # -O1
      if ( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0" )
         set( CMAKE_CXX_FLAGS "-std=c++0x" )
      else ()
         set( CMAKE_CXX_FLAGS "-std=c++11" )
      endif ()
endif ()


# Boost dependencies here
find_package( Boost 1.56.0 REQUIRED )
if ( Boost_FOUND )
   include_directories( ${Boost_INCLUDE_DIRS} )
   set( BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS} )

   option( BOOST_STATIC_LINKING "Turn on/off static linking for Boost" OFF )
   if ( BOOST_STATIC_LINKING )
      set( Boost_USE_STATIC_LIBS ON ) # ON when using static linking.
      message( "ATTENTION: Use static linking for Boost libraries!" )
   else ()
      set( Boost_USE_STATIC_LIBS OFF )
   endif ()

   set( Boost_USE_MULTITHREADED ON )
   set( Boost_USE_STATIC_RUNTIME OFF )
endif ()


# common include directories
include_directories( ${fmipp_SOURCE_DIR} )
include_directories( ${fmipp_SOURCE_DIR}/common )
include_directories( ${fmipp_SOURCE_DIR}/common/fmi_v1.0 )


# FMI++ import library
add_subdirectory( import )


# FMI++ export library
add_subdirectory( export )


# add tests for FMI++
if ( BUILD_TESTS )
   enable_testing()
   add_subdirectory( test )

   add_test_fmipp( testFMUModelExchange )
   add_test_fmipp( testFMUIntegrator )
   add_test_fmipp( testIncrementalFMU )
   add_test_fmipp( testRollbackFMU )
   add_test_fmipp( testFMIExportUtilities )
   add_test_fmipp( testFMI2ExportUtilities )
   add_test_fmipp( testFixedStepSizeFMU )
   add_test_fmipp( testVariableStepSizeFMU )
   add_test_fmipp( testInterpolatingFixedStepSizeFMU )
   add_test_fmipp( testModelDescription )
   add_test_fmipp( testModelManager )
   add_test_fmipp( testFMU2SDKImport )
   add_test_fmipp( testFMU2Integrator )
   add_test_fmipp( testFMU2ModelExchange )

   # add tests for SWIG interfaces to FMI++
   if ( BUILD_SWIG )
      if ( BUILD_SWIG_JAVA )
         add_test_fmipp_java( testFMUModelExchange )
         add_test_fmipp_java( testFMUCoSimulation )
         add_test_fmipp_java( testIncrementalFMU )
      endif ()

      if (BUILD_SWIG_PYTHON)
         add_test_fmipp_python( testFMUModelExchange )
         add_test_fmipp_python( testFMUCoSimulation )
         add_test_fmipp_python( testIncrementalFMU )
         add_test_fmipp_python( testRollbackFMU )
         add_test_fmipp_python( testFixedStepSizeFMU )
         add_test_fmipp_python( testVariableStepSizeFMU )
         add_test_fmipp_python( testInterpolatingFixedStepSizeFMU )
      endif ()
   endif ()
endif ()
