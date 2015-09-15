cmake_minimum_required(VERSION 2.8.12)

project(asymptotic_sine_fmu)

find_package(Java REQUIRED)
include(UseJava)

add_library(asymptotic_sine SHARED asymptotic_sine.c)

target_link_libraries( asymptotic_sine -lm )

set_target_properties(asymptotic_sine PROPERTIES PREFIX "")

add_custom_command(TARGET asymptotic_sine POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory asymptotic_sine/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:asymptotic_sine> asymptotic_sine/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml asymptotic_sine
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../asymptotic_sine
			  COMMAND ${CMAKE_COMMAND} -E copy_directory asymptotic_sine ../asymptotic_sine
			  COMMAND ${Java_JAR_EXECUTABLE} cfM asymptotic_sine.fmu -C asymptotic_sine/ .
)
