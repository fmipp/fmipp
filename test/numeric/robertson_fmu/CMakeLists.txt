cmake_minimum_required(VERSION 2.8.12)

project(robertson_fmu)

find_package(Java REQUIRED)
include(UseJava)

add_library(robertson SHARED robertson.c)

target_link_libraries( robertson -lm )

set_target_properties(robertson PROPERTIES PREFIX "")

add_custom_command(TARGET robertson POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory robertson/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:robertson> robertson/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml robertson
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../robertson
			  COMMAND ${CMAKE_COMMAND} -E copy_directory robertson ../robertson
			  COMMAND ${Java_JAR_EXECUTABLE} cfM robertson.fmu -C robertson/ .
)
