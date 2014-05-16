cmake_minimum_required(VERSION 2.8)


if ( WIN32 )
   message( "Adding tests: Make sure the Boost libraries have been included in your PATH variable BEFORE you run CMake!!!" )
endif ()


project(test)


add_definitions(-DFMU_URI_PRE="file://${CMAKE_CURRENT_BINARY_DIR}/")
add_definitions(-DEPS_TIME=1e-9)


add_executable(testFMUModelExchange testFMUModelExchange.cpp)
add_executable(testIncrementalFMU testIncrementalFMU.cpp)
add_executable(testRollbackFMU testRollbackFMU.cpp)
add_executable(testFMIExportUtilities testFMIExportUtilities.cpp)
add_executable(testFixedStepSizeFMU testFixedStepSizeFMU.cpp)
add_executable(testInterpolatingFixedStepSizeFMU testInterpolatingFixedStepSizeFMU.cpp)
add_executable(testModelDescription testModelDescription.cpp)
add_executable(testModelManager testModelManager.cpp)


find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)


target_link_libraries(testFMUModelExchange
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim)


target_link_libraries(testIncrementalFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim)


target_link_libraries(testRollbackFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim)


target_link_libraries(testFMIExportUtilities
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim
			fmippex)


target_link_libraries(testFixedStepSizeFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim)


target_link_libraries(testInterpolatingFixedStepSizeFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim)


target_link_libraries(testModelDescription
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim)


target_link_libraries(testModelManager
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim)


# add subdirectories including FMUs for testing
add_subdirectory(zigzag_fmu)
add_subdirectory(step_t0_fmu)
add_subdirectory(sine_standalone_fmu)


# special function for adding test for FMI++
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
            set(PATH_FOR_TESTING_OUTPUTCONFIG "${CMAKE_BINARY_DIR}\\test\\${OUTPUTCONFIG}\\;${CMAKE_BINARY_DIR}\\${OUTPUTCONFIG}\\;${PATH_FOR_TESTING}")
            # when adding tests, also add the set_test_properties for windows
            set_tests_properties( ${TEST_NAME}_${OUTPUTCONFIG} PROPERTIES ENVIRONMENT "PATH=${PATH_FOR_TESTING_OUTPUTCONFIG}" )

         endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

      endif ()

    else ( APPLE )

      message( STATUS "Add test " ${TEST_NAME} " (DARWIN)" )
      add_test( ${TEST_NAME} ${TEST_EXECUTABLE} )
      set_tests_properties( ${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/test" )

    else ( UNIX )

      message( STATUS "Add test " ${TEST_NAME} " (LINUX)" )
      add_test( ${TEST_NAME} ${TEST_EXECUTABLE} )
      set_tests_properties( ${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/test" )

   endif ()

endfunction( add_test_fmipp )
