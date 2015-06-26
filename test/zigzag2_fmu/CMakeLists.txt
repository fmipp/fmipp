cmake_minimum_required(VERSION 2.8.12)

project(zigzag2_fmu)

find_package(Java REQUIRED)
include(UseJava)

add_library(zigzag2 SHARED zigzag2.c)

target_link_libraries( zigzag2 -lm )

set_target_properties(zigzag2 PROPERTIES PREFIX "")

add_custom_command(TARGET zigzag2 POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory zigzag2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:zigzag2> zigzag2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml zigzag2
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../zigzag2
			  COMMAND ${CMAKE_COMMAND} -E copy_directory zigzag2 ../zigzag2
			  COMMAND ${Java_JAR_EXECUTABLE} cfM zigzag2.fmu -C zigzag2/ .
)
