cmake_minimum_required(VERSION 2.8)

project(fmippcs)

add_library(fmippcs SHARED src/FMIComponentFrontEnd.cpp src/FMIComponentBackEnd.cpp src/HelperFunctions.cpp src/SHMMaster.cpp src/SHMSlave.cpp src/SHMManager.cpp src/ScalarVariable.cpp)

find_package(Boost COMPONENTS date_time REQUIRED)

target_link_libraries(fmippcs
			${CMAKE_DL_LIBS}
			${Boost_LIBRARIES})

# OS-specific dependencies here
if (WIN32)
   target_link_libraries(fmippcs Shlwapi)
endif ()

# windows-specific compiler flags
if (WIN32)
   set_target_properties(fmippcs PROPERTIES COMPILE_FLAGS "-DBUILD_FMI_DLL")

   if (MINGW)
      add_definitions(-DMINGW)
      if ( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0" )
         add_definitions( -std=c++0x )
      else ()
         add_definitions( -std=c++11 )
      endif ()
   endif ()

   if (MSVC)
      # disable auto-linking and use cmake's dependency handling
      add_definitions( -DBOOST_ALL_NO_LIB )
   endif()
   
endif ()

include_directories(include)
include_directories(fmi_cs_1.0)


install(TARGETS fmippcs DESTINATION lib)
install(FILES include/FMIComponentBackEnd.h include/FMIComponentFrontEnd.h include/HelperFunctions.h include/IPCMaster.h include/IPCSlave.h include/SHMManager.h include/SHMMaster.h include/SHMSlave.h include/ScalarVariable.h DESTINATION include)
