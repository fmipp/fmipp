cmake_minimum_required(VERSION 2.8.12)

project(dq_fmu)

# find_package(Java REQUIRED)
# include(UseJava)

add_library(dq SHARED dq.c)

target_link_libraries( dq -lm )

set_target_properties(dq PROPERTIES PREFIX "")

if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'dq.fmu', regression tests can be run though." )
   add_custom_command(TARGET dq POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory dq/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dq> dq/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml dq
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../dq
			  COMMAND ${CMAKE_COMMAND} -E copy_directory dq ../dq)

else ()

   add_custom_command(TARGET dq POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory dq/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dq> dq/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml dq
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../dq
			  COMMAND ${CMAKE_COMMAND} -E copy_directory dq ../dq
			  COMMAND ${Java_JAR_EXECUTABLE} cfM dq.fmu -C dq/ . )

endif()