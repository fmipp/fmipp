# --------------------------------------------------------------
# Copyright (c) 2013, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# --------------------------------------------------------------

cmake_minimum_required(VERSION 2.8.12)

project(zerocrossing_fmu)

include(FindJava)

add_library(zerocrossing SHARED zerocrossing.c)
target_link_libraries(zerocrossing)

set_target_properties(zerocrossing PROPERTIES PREFIX "")


if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'zerocrossing.fmu', regression tests can be run though." )

   add_custom_command(TARGET zerocrossing POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory zerocrossing/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zerocrossing> zerocrossing/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:givezero> zerocrossing/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zerocrossing
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../zerocrossing
			  COMMAND ${CMAKE_COMMAND} -E copy_directory zerocrossing ../zerocrossing
   )

else ()

   add_custom_command(TARGET zerocrossing POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory zerocrossing/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zerocrossing> zerocrossing/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:givezero> zerocrossing/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zerocrossing
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../zerocrossing
			  COMMAND ${CMAKE_COMMAND} -E copy_directory zerocrossing ../zerocrossing
			  COMMAND ${Java_JAR_EXECUTABLE} cfM zerocrossing.fmu -C zerocrossing/ .
   )

endif ()
