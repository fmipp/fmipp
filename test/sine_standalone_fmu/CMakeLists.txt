cmake_minimum_required( VERSION 2.8.12 )


project( sine_standalone_fmu )


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test )


add_library( sine_standalone SHARED ${fmipp_export_SOURCE_DIR}/functions/fmiFunctions.cpp )
add_definitions( -DMODEL_IDENTIFIER=sine_standalone -DFRONT_END_TYPE=FMIComponentFrontEnd -DFRONT_END_TYPE_INCLUDE="FMIComponentFrontEnd.h" )
include_directories( ${CMAKE_SOURCE_DIR}/export/include )
set_target_properties( sine_standalone PROPERTIES PREFIX "" )
target_link_libraries( sine_standalone fmippex )


add_executable( sine_standalone_exe sine_standalone_exe.cpp )
target_link_libraries( sine_standalone_exe fmippex )


add_custom_command( TARGET sine_standalone POST_BUILD
			  COMMAND ${CMAKE_COMMAND} -E make_directory sine_standalone/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sine_standalone> sine_standalone/binaries/${FMU_BIN_DIR}
			  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E make_directory ../sine_standalone
			  COMMAND ${CMAKE_COMMAND} -E copy_directory sine_standalone ../sine_standalone
			  COMMAND ${Java_JAR_EXECUTABLE} cfM sine_standalone.fmu -C sine_standalone/ .
)

