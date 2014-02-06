cmake_minimum_required(VERSION 2.8)

project(zigzag_fmu)

find_package(Java REQUIRED)
include(UseJava)

add_library(zigzag SHARED zigzag.c)

set_target_properties(zigzag PROPERTIES PREFIX "")

add_custom_command(TARGET zigzag POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory zigzag/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zigzag> zigzag/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zigzag
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../zigzag
			  COMMAND ${CMAKE_COMMAND} -E copy_directory zigzag ../zigzag
			  COMMAND ${Java_JAR_EXECUTABLE} cfM zigzag.fmu -C zigzag/ .
)
