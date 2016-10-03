cmake_minimum_required(VERSION 2.8.12)

project(stiff2_fmu)

# find_package(Java REQUIRED)
# include(UseJava)

add_library(stiff2 SHARED stiff2.c)

target_link_libraries( stiff2 -lm )

set_target_properties(stiff2 PROPERTIES PREFIX "")

if ( ${Java_JAR_EXECUTABLE} STREQUAL "Java_JAR_EXECUTABLE-NOTFOUND" )

   message( "Java JAR executable not available! Cannot build complete 'stiff2.fmu', regression tests can be run though." )

   add_custom_command(TARGET stiff2 POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory stiff2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:stiff2> stiff2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml stiff2
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../stiff2
			  COMMAND ${CMAKE_COMMAND} -E copy_directory stiff2 ../stiff2 )

else ()

   add_custom_command(TARGET stiff2 POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory stiff2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:stiff2> stiff2/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml stiff2
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../stiff2
			  COMMAND ${CMAKE_COMMAND} -E copy_directory stiff2 ../stiff2
			  COMMAND ${Java_JAR_EXECUTABLE} cfM stiff2.fmu -C stiff2/ . )

endif ()