cmake_minimum_required(VERSION 2.8.12)


project(fmipp_export)


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )


add_library(fmippex SHARED src/FMIComponentFrontEnd.cpp src/FMIComponentBackEnd.cpp src/HelperFunctions.cpp src/SHMMaster.cpp src/SHMSlave.cpp src/SHMManager.cpp src/ScalarVariable.cpp)


find_package(Boost COMPONENTS date_time REQUIRED)


target_link_libraries(fmippex ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} fmippim )


# OS-specific dependencies here
if (WIN32)
   set_target_properties(fmippex PROPERTIES COMPILE_FLAGS "-DBUILD_FMI_DLL")
   target_link_libraries(fmippex Shlwapi)
endif ()


#install( TARGETS fmippex DESTINATION lib )

#install( FILES include/FMIComponentBackEnd.h include/FMIComponentFrontEnd.h include/HelperFunctions.h include/IPCMaster.h include/IPCSlave.h include/SHMManager.h include/SHMMaster.h include/SHMSlave.h include/ScalarVariable.h DESTINATION include/fmipp_export )
