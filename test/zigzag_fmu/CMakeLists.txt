# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(zigzag_fmu)

include(FindJava)

add_library(zigzag SHARED zigzag.c)
add_library(givezero SHARED givezero.c)
target_link_libraries(zigzag givezero)

set_target_properties(zigzag PROPERTIES PREFIX "")


if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'zigzag.fmu', regression tests can be run though." )

   add_custom_command(TARGET zigzag POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory zigzag/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zigzag> zigzag/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:givezero> zigzag/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zigzag
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../zigzag
			  COMMAND ${CMAKE_COMMAND} -E copy_directory zigzag ../zigzag
   )

else ()

   add_custom_command(TARGET zigzag POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory zigzag/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zigzag> zigzag/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:givezero> zigzag/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zigzag
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../zigzag
			  COMMAND ${CMAKE_COMMAND} -E copy_directory zigzag ../zigzag
			  COMMAND ${Java_JAR_EXECUTABLE} cfM zigzag.fmu -C zigzag/ .
   )

endif ()
