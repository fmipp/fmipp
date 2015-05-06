cmake_minimum_required(VERSION 2.8.12)

project(stiff_fmu)

find_package(Java REQUIRED)
include(UseJava)

add_library(stiff SHARED stiff.c)

target_link_libraries( stiff -lm )

set_target_properties(stiff PROPERTIES PREFIX "")

add_custom_command(TARGET stiff POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory stiff/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:stiff> stiff/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml stiff
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../stiff
			  COMMAND ${CMAKE_COMMAND} -E copy_directory stiff ../stiff
			  COMMAND ${Java_JAR_EXECUTABLE} cfM stiff.fmu -C stiff/ .
)
