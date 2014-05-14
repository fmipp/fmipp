cmake_minimum_required(VERSION 2.8)


project(fmipp)


# optional builds: to switch the building of different subparts off, use a GUI or "cmake -Doption=OFF [...]"
# when you have switched them off, use the GUI or "cmake -Doption=ON [...]" to switch them on again
option(BUILD_SWIG "SWIG - Simplified Wrapper and Interface Generator stuff." ON)
option(BUILD_TESTS "Unit tests for FMI++." ON)


# set the name of the FMU-binaries-subdirectory according to the current OS
if (WIN32)
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMU_BIN_DIR "win64")
   else ()
      set(FMU_BIN_DIR "win32")
   endif ()
   set(FMU_BIN_EXT ".dll")
elseif (APPLE)
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMU_BIN_DIR "darwin64")
   else ()
      set(FMU_BIN_DIR "darwin32")
   endif ()
   set(FMU_BIN_EXT ".dylib")
elseif (UNIX)
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMU_BIN_DIR "linux64")
   else ()
      set(FMU_BIN_DIR "linux32")
   endif ()
   set(FMU_BIN_EXT ".so")
endif ()

add_definitions(-DFMU_BIN_DIR="${FMU_BIN_DIR}")
add_definitions(-DFMU_BIN_EXT="${FMU_BIN_EXT}")


# OS-specific compiler settings
if (WIN32) # windows-specific
   add_definitions( -DWIN32 )
   if (MINGW)
      add_definitions(-DMINGW)
      if ( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0" )
         add_definitions( -std=c++0x -Wall ) # -O1
      else ()
         add_definitions( -std=c++11 -Wall ) # -O1
      endif ()
   endif ()
   if ( MSVC )
      # disable auto-linking and use cmake's dependency handling
      add_definitions( -DBOOST_ALL_NO_LIB /W3 )
   endif ()
else () # linux-specific
      if ( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0" )
         add_definitions( -std=c++0x -Wall ) # -O1
      else ()
         add_definitions( -std=c++11 -Wall ) # -O1
      endif ()
endif ()


# Boost dependencies here
find_package(Boost REQUIRED)
if (Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS})
   set(BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS})
endif ()


# common include directories
include_directories( ${fmipp_SOURCE_DIR} )
include_directories( ${fmipp_SOURCE_DIR}/common )
include_directories( ${fmipp_SOURCE_DIR}/common/fmi_v1.0 )


# FMI++ import library
add_subdirectory(import)


# FMI++ export library
add_subdirectory(export)


# add tests for FMI++
if (BUILD_TESTS)
   enable_testing()
   add_subdirectory(test)

   add_test_fmipp(fmipp_testFMUModelExchange test/testFMUModelExchange)
   add_test_fmipp(fmipp_testIncrementalFMU test/testIncrementalFMU)
   add_test_fmipp(fmipp_testRollbackFMU test/testRollbackFMU)
   add_test_fmipp(fmipp_testFMIExportUtilities test/testFMIExportUtilities)
   add_test_fmipp(fmipp_testFixedStepSizeFMU test/testFixedStepSizeFMU)
   add_test_fmipp(fmipp_testInterpolatingFixedStepSizeFMU test/testInterpolatingFixedStepSizeFMU)
   add_test_fmipp(fmipp_testModelDescription test/testModelDescription)
   add_test_fmipp(fmipp_testModelManager test/testModelManager)
endif ()


# add a target to generate documentation with Doxygen
find_package( Doxygen )
if ( DOXYGEN_FOUND )
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY )
   add_custom_target( doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif ()
