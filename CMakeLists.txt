cmake_minimum_required(VERSION 2.8)

function( add_test_fmipp TEST_NAME TEST_EXECUTABLE )

   # special treatment for windows, set where to look for dlls when running the tests
   if ( WIN32 )

      # magic substitution. windows and cmake use the semicolon in a different way and fortunately windows uses the usual escape character backslash as path-separator
      STRING(REPLACE ";" "/\\;" PATH_FOR_TESTING "$ENV{PATH}")

      if ( MINGW )

         message( STATUS "Add test " ${TEST_NAME} " (MINGW)" )
         # append the directory with the built shared libraries
         set( PATH_FOR_TESTING "${CMAKE_BINARY_DIR}\\;${PATH_FOR_TESTING}" )
         add_test( ${TEST_NAME} ${TEST_EXECUTABLE} )
         # when adding tests, also add the set_test_properties for windows
         set_tests_properties( ${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${PATH_FOR_TESTING}" )

      elseif ( MSVC )

         # dll path depends on configuration (Debug, Release, etc.)
         foreach ( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )

            message( STATUS "Add test " ${TEST_NAME} " for configuration " ${OUTPUTCONFIG} )
            add_test(NAME ${TEST_NAME}_${OUTPUTCONFIG} CONFIGURATIONS ${OUTPUTCONFIG} COMMAND ${TEST_EXECUTABLE})
            # append the directory with the built shared libraries
            set(PATH_FOR_TESTING_OUTPUTCONFIG "${CMAKE_BINARY_DIR}\\${OUTPUTCONFIG}\\;${PATH_FOR_TESTING}")
            # when adding tests, also add the set_test_properties for windows
            set_tests_properties( ${TEST_NAME}_${OUTPUTCONFIG} PROPERTIES ENVIRONMENT "PATH=${PATH_FOR_TESTING_OUTPUTCONFIG}" )

         endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

      endif ()

   else ()

      message( STATUS "Add test " ${TEST_NAME} " (LINUX)" )
      add_test( ${TEST_NAME} ${TEST_EXECUTABLE} )

   endif ()

endfunction( add_test_fmipp )




project(fmipp)

# options, to switch the building of different subparts off, use a GUI or "cmake -Doption=OFF [...]"
# when you have switched them off, use the GUI or "cmake -Doption=ON [...]" to switch them on again
option(BUILD_SWIG "SWIG - Simplified Wrapper and Interface Generator stuff." ON)
option(BUILD_TESTS "Unit tests for FMI++." ON)

# set the name of the FMU-binaries-subdirectory according to the OS you're on
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

add_library(fmipp SHARED src/stack.c src/xml_parser.c src/FMU.cpp src/FMUIntegrator.cpp src/FMUIntegratorStepper.cpp src/History.cpp src/IncrementalFMU.cpp src/ModelManager.cpp src/FixedStepSizeFMU.cpp src/InterpolatingFixedStepSizeFMU.cpp src/FMUCoSimulation.cpp src/RollbackFMU.cpp)

# dependencies here
find_package(Boost REQUIRED)
if (Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS})
   set(BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS})
endif ()

find_package(EXPAT REQUIRED)
if (EXPAT_FOUND)
   include_directories(${EXPAT_INCLUDE_DIRS})
   target_link_libraries(fmipp ${EXPAT_LIBRARIES})
endif ()		       

target_link_libraries(fmipp ${CMAKE_DL_LIBS})

# OS-specific dependencies here
if (WIN32)
   target_link_libraries(fmipp Shlwapi)
endif ()

# windows-specific compiler flags
if (WIN32)
   set_target_properties(fmipp PROPERTIES COMPILE_FLAGS "-DBUILD_FMI_DLL")
   if (MINGW)
      add_definitions(-DMINGW)
   endif ()
endif ()

include_directories(include)

if (BUILD_SWIG)
   add_subdirectory(swig)
endif ()

if (BUILD_TESTS)

   if ( WIN32 )
      message( "Adding tests: Make sure all external library directories (Boost etc.) have been included in your PATH variable BEFORE you run CMake!!!" )
   endif ()

   enable_testing()

   add_subdirectory(test)

   add_test_fmipp(fmipp_testFMU test/testFMU)
   add_test_fmipp(fmipp_testIncrementalFMU test/testIncrementalFMU)
   add_test_fmipp(fmipp_testRollbackFMU test/testRollbackFMU)

endif ()

install(TARGETS fmipp DESTINATION lib)
install(FILES include/FMU.h include/IncrementalFMU.h include/FixedStepSizeFMU.h include/InterpolatingFixedStepSizeFMU.h include/FMUCoSimulation.h DESTINATION include)
