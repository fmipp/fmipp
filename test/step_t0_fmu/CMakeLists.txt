cmake_minimum_required(VERSION 2.8.12)

project(step_t0_fmu)

add_library(step_t0 SHARED step_t0.c)

set_target_properties(step_t0 PROPERTIES PREFIX "")


if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'step_t0.fmu', regression tests can be run though." )

   add_custom_command( TARGET step_t0 POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory step_t0/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:step_t0> step_t0/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml step_t0
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../step_t0
			  COMMAND ${CMAKE_COMMAND} -E copy_directory step_t0 ../step_t0
   )

else ()

   add_custom_command( TARGET step_t0 POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory step_t0/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:step_t0> step_t0/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml step_t0
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../step_t0
			  COMMAND ${CMAKE_COMMAND} -E copy_directory step_t0 ../step_t0
			  COMMAND ${Java_JAR_EXECUTABLE} cfM step_t0.fmu -C step_t0/ .
   )

endif ()