# -------------------------------------------------------------------
# Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required( VERSION 2.8.12 )


project( sine_standalone_fmu )


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test )


add_library( sine_standalone SHARED ${fmipp_export_SOURCE_DIR}/functions/fmi_v1.0/fmiFunctions.cpp )
add_definitions( -DMODEL_IDENTIFIER=sine_standalone -DFRONT_END_TYPE=FMIComponentFrontEnd -DFRONT_END_TYPE_INCLUDE="FMIComponentFrontEnd.h" )

# The include is given relative to the parent's project directory instead of 
# CMAKE_SOURCE_DIR. CMAKE_SOURCE_DIR may point to another project if fmipp 
# itself is referenced
include_directories( ${fmipp_SOURCE_DIR}/export/include )
set_target_properties( sine_standalone PROPERTIES PREFIX "" )
target_link_libraries( sine_standalone fmippex )


add_executable( sine_standalone_exe sine_standalone_exe.cpp )
target_link_libraries( sine_standalone_exe fmippex )


if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'sine_standalone.fmu', regression tests can be run though." )

   add_custom_command( TARGET sine_standalone POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone> sine_standalone/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone_exe> sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dummy_input_file.txt sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy_directory sine_standalone ../sine_standalone
   )

else ()

   add_custom_command( TARGET sine_standalone POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone> sine_standalone/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone_exe> sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dummy_input_file.txt sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy_directory sine_standalone ../sine_standalone
			  COMMAND ${Java_JAR_EXECUTABLE} cfM sine_standalone.fmu -C sine_standalone/ .
   )

endif ()
