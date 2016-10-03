cmake_minimum_required(VERSION 2.8.12)

project(linear_stiff_fmu)

# find_package(Java REQUIRED)
# include(UseJava)

add_library(linear_stiff SHARED linear_stiff.c)

target_link_libraries( linear_stiff -lm )

set_target_properties(linear_stiff PROPERTIES PREFIX "")

if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'linear_stiff.fmu', regression tests can be run though." )

   add_custom_command(TARGET linear_stiff POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory linear_stiff/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:linear_stiff> linear_stiff/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml linear_stiff
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../linear_stiff
			  COMMAND ${CMAKE_COMMAND} -E copy_directory linear_stiff ../linear_stiff )

else ()

   add_custom_command(TARGET linear_stiff POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory linear_stiff/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:linear_stiff> linear_stiff/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml linear_stiff
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../linear_stiff
			  COMMAND ${CMAKE_COMMAND} -E copy_directory linear_stiff ../linear_stiff
			  COMMAND ${Java_JAR_EXECUTABLE} cfM linear_stiff.fmu -C linear_stiff/ . )

endif()
