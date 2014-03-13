cmake_minimum_required( VERSION 2.8 )

project( sine_standalone_fmu )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test )

find_package( Java REQUIRED )
include( UseJava )

include_directories( ${fmippcs_SOURCE_DIR}/include )
include_directories( ${fmippcs_SOURCE_DIR}/fmi_cs_1.0 )
include_directories( ${fmippcs_SOURCE_DIR}/functions )

add_library( sine_standalone SHARED ${fmippcs_SOURCE_DIR}/functions/fmiFunctions.cpp )
set_target_properties( sine_standalone PROPERTIES COMPILE_FLAGS "-DMODEL_IDENTIFIER=sine_standalone" )
set_target_properties( sine_standalone PROPERTIES PREFIX "" )
target_link_libraries( sine_standalone fmippcs )

add_executable( sine_standalone_exe sine_standalone_exe.cpp )
target_link_libraries( sine_standalone_exe fmippcs )


add_custom_command( TARGET sine_standalone POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone> sine_standalone/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy_directory sine_standalone ../sine_standalone
			  COMMAND ${Java_JAR_EXECUTABLE} cfM sine_standalone.fmu -C sine_standalone/ .
)

