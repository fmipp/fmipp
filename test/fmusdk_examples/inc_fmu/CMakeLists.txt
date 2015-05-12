cmake_minimum_required(VERSION 2.8.12)

project(inc_fmu)

find_package(Java REQUIRED)
include(UseJava)

add_library(inc SHARED inc.c)

target_link_libraries( inc -lm )

set_target_properties(inc PROPERTIES PREFIX "")

add_custom_command(TARGET inc POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory inc/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inc> inc/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml inc
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../inc
			  COMMAND ${CMAKE_COMMAND} -E copy_directory inc ../inc
			  COMMAND ${Java_JAR_EXECUTABLE} cfM inc.fmu -C inc/ .
)
