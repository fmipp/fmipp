# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required( VERSION 2.8.12 )

include(FindJava)

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


add_executable( testFMUModelExchange              testFMUModelExchange.cpp )
add_executable( testFMUIntegrator                 testFMUIntegrator.cpp )
add_executable( testIncrementalFMU                testIncrementalFMU.cpp )
add_executable( testRollbackFMU                   testRollbackFMU.cpp )
add_executable( testFMIExportUtilities            testFMIExportUtilities.cpp )
add_executable( testFixedStepSizeFMU              testFixedStepSizeFMU.cpp )
add_executable( testVariableStepSizeFMU           testVariableStepSizeFMU.cpp )
add_executable( testInterpolatingFixedStepSizeFMU testInterpolatingFixedStepSizeFMU.cpp )
add_executable( testModelDescription              testModelDescription.cpp )
add_executable( testFMU2SDKImport                 testFMU2SDKImport.cpp )
add_executable( testFMU2Integrator                testFMU2Integrator.cpp )
add_executable( testFMU2ModelExchange             testFMU2ModelExchange.cpp )
add_executable( testModelManager                  testModelManager.cpp )

if ( BUILD_SWIG )
   # build java tests
   find_package( Java REQUIRED )
   include( UseJava )
   set( CMAKE_JAR_CLASSES_PREFIX "" )

   if ( MINGW )
      # This is a WORKAROUND for using Java with CMake under MINGW!
      # PROBLEM: When compiling with javac, the argument for the '-classpath' option is
      #          not put between quotation marks, which causes a syntax error.
      # SOLUTION: Add a dummy directory name including a blank character into the search
      #          path. That causes CMake to put the full expression between quotation marks.
      set( CMAKE_JAVA_INCLUDE_PATH "C:/dummy dir/" )
   endif()

   add_jar( testFMUModelExchange_java testFMUModelExchange.java INCLUDE_JARS ${FMIPPIM_JAR} )
   add_dependencies( testFMUModelExchange_java fmipp_jar )

   add_jar( testIncrementalFMU_java testIncrementalFMU.java INCLUDE_JARS ${FMIPPIM_JAR} )
   add_dependencies( testIncrementalFMU_java fmipp_jar )

   # copy python tests
   add_python( testFMUModelExchange.py )
   add_python( testFMUCoSimulation.py )
   add_python( testIncrementalFMU.py )
   add_python( testRollbackFMU.py )
   add_python( testFixedStepSizeFMU.py )
   add_python( testVariableStepSizeFMU.py )
   add_python( testInterpolatingFixedStepSizeFMU.py )
endif ()

find_package( Boost COMPONENTS system filesystem unit_test_framework REQUIRED )


target_link_libraries( testFMUModelExchange
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )
      
      
target_link_libraries( testFMUIntegrator
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


target_link_libraries( testVariableStepSizeFMU
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


target_link_libraries( testFMU2SDKImport
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )


target_link_libraries( testFMU2Integrator
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmippim )


target_link_libraries( testFMU2ModelExchange
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
add_subdirectory( zigzag2_fmu )
add_subdirectory( step_t0_fmu )
add_subdirectory( sine_standalone_fmu )
add_subdirectory( v2_0_fmu )
add_subdirectory( fmusdk_examples )
add_subdirectory( numeric )
add_subdirectory( dxiskx_fmu )

include( CMakeAddTests.txt )
