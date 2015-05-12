cmake_minimum_required(VERSION 2.8.12)

project(values_fmu)

find_package(Java REQUIRED)
include(UseJava)

add_library(values SHARED values.c)

target_link_libraries( values -lm )

set_target_properties(values PROPERTIES PREFIX "")

add_custom_command(TARGET values POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory values/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:values> values/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml values
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../values
			  COMMAND ${CMAKE_COMMAND} -E copy_directory values ../values
			  COMMAND ${Java_JAR_EXECUTABLE} cfM values.fmu -C values/ .
)
