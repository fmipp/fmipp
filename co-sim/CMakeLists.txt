cmake_minimum_required(VERSION 2.8)

project(fmippcs)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

add_library(fmippcs SHARED src/FMIComponentFrontEnd.cpp src/FMIComponentBackEnd.cpp src/HelperFunctions.cpp src/SHMMaster.cpp src/SHMSlave.cpp src/SHMManager.cpp src/ScalarVariable.cpp)

find_package(Boost COMPONENTS date_time REQUIRED)

target_link_libraries(fmippcs
			${CMAKE_DL_LIBS}
			${Boost_LIBRARIES})

# OS-specific dependencies here
if (WIN32)
   set_target_properties(fmippcs PROPERTIES COMPILE_FLAGS "-DBUILD_FMI_DLL")
   target_link_libraries(fmippcs Shlwapi)
endif ()

include_directories( include )
include_directories( ${fmipp_SOURCE_DIR} )


install(TARGETS fmippcs DESTINATION lib)
install(FILES include/FMIComponentBackEnd.h include/FMIComponentFrontEnd.h include/HelperFunctions.h include/IPCMaster.h include/IPCSlave.h include/SHMManager.h include/SHMMaster.h include/SHMSlave.h include/ScalarVariable.h DESTINATION include)
