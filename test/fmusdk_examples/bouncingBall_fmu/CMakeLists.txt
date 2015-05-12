cmake_minimum_required(VERSION 2.8.12)

project(bouncingBall_fmu)

find_package(Java REQUIRED)
include(UseJava)

add_library(bouncingBall SHARED bouncingBall.c)

target_link_libraries( bouncingBall -lm )

set_target_properties(bouncingBall PROPERTIES PREFIX "")

add_custom_command(TARGET bouncingBall POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory bouncingBall/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:bouncingBall> bouncingBall/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml bouncingBall
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../bouncingBall
			  COMMAND ${CMAKE_COMMAND} -E copy_directory bouncingBall ../bouncingBall
			  COMMAND ${Java_JAR_EXECUTABLE} cfM bouncingBall.fmu -C bouncingBall/ .
)
