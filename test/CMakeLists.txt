cmake_minimum_required( VERSION 2.8.12 )


if ( BUILD_SWIG )
   #function for copying the python files
   function( add_python FILE_NAME )
      add_custom_command( OUTPUT ${FILE_NAME}
                          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME} ${CMAKE_CURRENT_BINARY_DIR}
			  MAIN_DEPENDENCY ${FILE_NAME} )
      add_custom_target( ${FILE_NAME}_python_file_copy ALL DEPENDS ${FILE_NAME} )
   endfunction( add_python )
endif ()

if ( WIN32 )
   message( "Adding tests: Make sure the Boost libraries have been included in your PATH variable BEFORE you run CMake!!!" )
endif ()


project( test )

set( EPS_TIME 1e-9 CACHE INTERNAL "" )
set( FMU_URI_PRE "file://${CMAKE_CURRENT_BINARY_DIR}/" CACHE INTERNAL "" )
add_definitions( -DFMU_URI_PRE="${FMU_URI_PRE}" )
add_definitions( -DEPS_TIME=${EPS_TIME} )


add_executable( testFMUModelExchange testFMUModelExchange.cpp )
add_executable( testIncrementalFMU testIncrementalFMU.cpp )
add_executable( testRollbackFMU testRollbackFMU.cpp )
add_executable( testFMIExportUtilities testFMIExportUtilities.cpp )
add_executable( testFixedStepSizeFMU testFixedStepSizeFMU.cpp )
add_executable( testInterpolatingFixedStepSizeFMU testInterpolatingFixedStepSizeFMU.cpp )
add_executable( testModelDescription testModelDescription.cpp )
add_executable( testModelManager testModelManager.cpp )

if ( BUILD_SWIG )
   # build java tests
   find_package( Java REQUIRED )
   include( UseJava )
   set( CMAKE_JAR_CLASSES_PREFIX "" )

   add_jar( testFMUModelExchange_java testFMUModelExchange.java INCLUDE_JARS ${FMIPPIM_JAR} )

   # copy python tests
   add_python( testFMUModelExchange.py )
endif ()

find_package( Boost COMPONENTS system filesystem unit_test_framework REQUIRED )


target_link_libraries( testFMUModelExchange
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )


target_link_libraries( testIncrementalFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )


target_link_libraries( testRollbackFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )


target_link_libraries( testFMIExportUtilities
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim
			fmippex )


target_link_libraries( testFixedStepSizeFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )


target_link_libraries( testInterpolatingFixedStepSizeFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )


target_link_libraries( testModelDescription
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )


target_link_libraries( testModelManager
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )


# add subdirectories including FMUs for testing
add_subdirectory( zigzag_fmu )
add_subdirectory( step_t0_fmu )
add_subdirectory( sine_standalone_fmu )


# special function for adding test for FMI++
function( add_test_fmipp TEST_NAME )

   # special treatment for windows, set where to look for dlls when running the tests
   if ( WIN32 )

      # magic substitution. windows and cmake use the semicolon in a different way and fortunately windows uses the usual escape character backslash as path-separator
      STRING( REPLACE ";" "/\\;" PATH_FOR_TESTING "$ENV{PATH}" )

      if ( MINGW )

         message( STATUS "Add test " ${TEST_NAME} " (MINGW)" )
         # append the directory with the built shared libraries
         set( PATH_FOR_TESTING "${CMAKE_BINARY_DIR}\\;${PATH_FOR_TESTING}" )
         add_test( fmipp_${TEST_NAME} test/${TEST_NAME} )
         # when adding tests, also add the set_test_properties for windows
         set_tests_properties( fmipp_${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${PATH_FOR_TESTING}" )

      elseif ( MSVC )

         # dll path depends on configuration (Debug, Release, etc.)
         foreach ( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )

            message( STATUS "Add test " ${TEST_NAME} " for configuration " ${OUTPUTCONFIG} )
            add_test( NAME fmipp_${TEST_NAME}_${OUTPUTCONFIG} CONFIGURATIONS ${OUTPUTCONFIG} COMMAND test/${TEST_NAME} )
            # append the directory with the built shared libraries
            set( PATH_FOR_TESTING_OUTPUTCONFIG "${CMAKE_BINARY_DIR}\\test\\${OUTPUTCONFIG}\\;${CMAKE_BINARY_DIR}\\${OUTPUTCONFIG}\\;${PATH_FOR_TESTING}" )
            # when adding tests, also add the set_test_properties for windows
            set_tests_properties( fmipp_${TEST_NAME}_${OUTPUTCONFIG} PROPERTIES ENVIRONMENT "PATH=${PATH_FOR_TESTING_OUTPUTCONFIG}" )

         endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

      endif ()

    elseif ( APPLE )

      message( STATUS "Add test ${TEST_NAME} (DARWIN)" )
      add_test( fmipp_${TEST_NAME} test/${TEST_NAME} )
      set_tests_properties( fmipp_${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/test" )

    else ( UNIX )

      message( STATUS "Add test ${TEST_NAME} (LINUX)" )
      add_test( fmipp_${TEST_NAME} test/${TEST_NAME} )
      set_tests_properties( fmipp_${TEST_NAME} PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/test" )

   endif ()

endfunction( add_test_fmipp )

if ( BUILD_SWIG )
# function for adding java tests
function( add_test_fmipp_java TEST_NAME )
   message( STATUS "Add test ${TEST_NAME}_JAVA" )
   get_target_property( test_jar_file_path ${TEST_NAME}_java JAR_FILE )
   add_test( NAME fmipp_${TEST_NAME}_java COMMAND ${Java_JAVA_EXECUTABLE} -ea -Djava.library.path=${FMIPPIM_WRAP_DIR} -cp "${test_jar_file_path}${PATH_SEPARATOR}${FMIPPIM_JAR}" ${TEST_NAME} "${FMU_URI_PRE}" "${EPS_TIME}" )
endfunction( add_test_fmipp_java )


# function for adding python tests
function( add_test_fmipp_python TEST_NAME )
   message( STATUS "Add test ${TEST_NAME}_PYTHON" )
   add_test( NAME fmipp_${TEST_NAME}_python COMMAND ${PYTHON_EXECUTABLE} test/${TEST_NAME}.py "${FMU_URI_PRE}" "${EPS_TIME}" )
   set_tests_properties( fmipp_${TEST_NAME}_python PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/import/swig" )
endfunction( add_test_fmipp_python )
endif ( BUILD_SWIG )