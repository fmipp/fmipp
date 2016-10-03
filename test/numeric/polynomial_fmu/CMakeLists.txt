# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(polynomial_fmu)

# find_package(Java REQUIRED)
# include(UseJava)

add_library(polynomial SHARED polynomial.c)

set_target_properties(polynomial PROPERTIES PREFIX "")

if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'sine_standalone.fmu', regression tests can be run though." )

   add_custom_command(TARGET polynomial POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory polynomial/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:polynomial> polynomial/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml polynomial
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../polynomial
			  COMMAND ${CMAKE_COMMAND} -E copy_directory polynomial ../polynomial )

else ()

   add_custom_command(TARGET polynomial POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory polynomial/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:polynomial> polynomial/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml polynomial
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../polynomial
			  COMMAND ${CMAKE_COMMAND} -E copy_directory polynomial ../polynomial
			  COMMAND ${Java_JAR_EXECUTABLE} cfM polynomial.fmu -C polynomial/ . )

endif ()