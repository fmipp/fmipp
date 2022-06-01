# -------------------------------------------------------------------
# Copyright (c) 2013-2022, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required( VERSION 2.8.12 )


project( sine_standalone2_fmu )


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test )


add_library( sine_standalone2 SHARED ${fmipp_export_SOURCE_DIR}/functions/fmi_v2.0/fmi2Functions.cpp )
add_definitions( -DMODEL_IDENTIFIER=sine_standalone2 -DFRONT_END_TYPE=FMIComponentFrontEnd -DFRONT_END_TYPE_INCLUDE="FMIComponentFrontEnd.h" )

# The include is given relative to the parent's project directory instead of 
# CMAKE_SOURCE_DIR. CMAKE_SOURCE_DIR may point to another project if fmipp 
# itself is referenced
include_directories( ${fmipp_SOURCE_DIR}/export/include )
set_target_properties( sine_standalone2 PROPERTIES PREFIX "" )
target_link_libraries( sine_standalone2 fmippex )


add_executable( sine_standalone2_exe sine_standalone_exe.cpp )
target_link_libraries( sine_standalone2_exe fmippex )


if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'sine_standalone2.fmu', regression tests can be run though." )

   add_custom_command( TARGET sine_standalone2 POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone2/resources
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone2> sine_standalone2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml sine_standalone2
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone2_exe> sine_standalone2/resources
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dummy_input_file.txt sine_standalone2/resources
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../sine_standalone2
			  COMMAND ${CMAKE_COMMAND} -E copy_directory sine_standalone2 ../sine_standalone2
   )

else ()

   add_custom_command( TARGET sine_standalone2 POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone2/resources
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone2> sine_standalone2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml sine_standalone2
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone2_exe> sine_standalone2/resources
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dummy_input_file.txt sine_standalone2/resources
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../sine_standalone2
			  COMMAND ${CMAKE_COMMAND} -E copy_directory sine_standalone2 ../sine_standalone2
			  COMMAND ${Java_JAR_EXECUTABLE} cfM sine_standalone2.fmu -C sine_standalone2/ .
   )

endif ()
