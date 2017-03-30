# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(dxiskx_fmu)

include(FindJava)

add_library(dxiskx SHARED dxiskx.c)

set_target_properties(dxiskx PROPERTIES PREFIX "")


if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'dxiskx.fmu', regression tests can be run though." )

   add_custom_command(TARGET dxiskx POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory dxiskx/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dxiskx> dxiskx/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml dxiskx
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../dxiskx
			  COMMAND ${CMAKE_COMMAND} -E copy_directory dxiskx ../dxiskx
   )

else ()

   add_custom_command(TARGET dxiskx POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory dxiskx/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dxiskx> dxiskx/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml dxiskx
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../dxiskx
			  COMMAND ${CMAKE_COMMAND} -E copy_directory dxiskx ../dxiskx
			  COMMAND ${Java_JAR_EXECUTABLE} cfM dxiskx.fmu -C dxiskx/ .
   )

endif ()
