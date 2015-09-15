cmake_minimum_required(VERSION 2.8.12)

project(vanDerPol_fmu)

find_package(Java REQUIRED)
include(UseJava)

add_library(vanDerPol SHARED vanDerPol.c)

target_link_libraries( vanDerPol -lm )

set_target_properties(vanDerPol PROPERTIES PREFIX "")

add_custom_command(TARGET vanDerPol POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory vanDerPol/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vanDerPol> vanDerPol/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml vanDerPol
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../vanDerPol
			  COMMAND ${CMAKE_COMMAND} -E copy_directory vanDerPol ../vanDerPol
			  COMMAND ${Java_JAR_EXECUTABLE} cfM vanDerPol.fmu -C vanDerPol/ .
)
