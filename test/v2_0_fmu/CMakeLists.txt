# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(v2_0_fmu)

include(FindJava)

add_library(v2_0 SHARED v2_0.c)

set_target_properties(v2_0 PROPERTIES PREFIX "")


if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'v2_0.fmu', regression tests can be run though." )

   add_custom_command(TARGET v2_0 POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory v2_0/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:v2_0> v2_0/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml v2_0
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../v2_0
			  COMMAND ${CMAKE_COMMAND} -E copy_directory v2_0 ../v2_0
   )

else ()

   add_custom_command(TARGET v2_0 POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory v2_0/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:v2_0> v2_0/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml v2_0
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../v2_0
			  COMMAND ${CMAKE_COMMAND} -E copy_directory v2_0 ../v2_0
			  COMMAND ${Java_JAR_EXECUTABLE} cfM v2_0.fmu -C v2_0/ .
   )

endif ()
