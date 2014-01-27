cmake_minimum_required(VERSION 2.8)

project(fmipp)

enable_testing()

if (WIN32)
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMU_BIN_DIR "win64")
   else ()
      set(FMU_BIN_DIR "win32")
   endif ()
elseif (APPLE)
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMU_BIN_DIR "darwin64")
   else ()
      set(FMU_BIN_DIR "darwin32")
   endif ()
elseif (UNIX)
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMU_BIN_DIR "linux64")
   else ()
      set(FMU_BIN_DIR "linux32")
   endif ()
endif ()

if (WIN32)
   if (MINGW)
      set(CMAKE_C_FLAGS "-DMINGW")
      set(CMAKE_CXX_FLAGS "-DMINGW")
   endif ()
endif ()

add_library(fmipp SHARED src/stack.c src/xml_parser.c src/FMU.cpp src/FMUIntegrator.cpp src/FMUIntegratorStepper.cpp src/History.cpp src/IncrementalFMU.cpp src/ModelManager.cpp src/FixedStepSizeFMU.cpp src/InterpolatingFixedStepSizeFMU.cpp src/FMUCoSimulation.cpp)

find_package(Boost REQUIRED)
if (BOOST_FOUND)
   include_directories(${BOOST_INCLUDE_DIRS})
   target_link_libraries(fmipp ${BOOST_LIBRARY_DIRS})
endif (BOOST_FOUND)

find_package(EXPAT REQUIRED)
if (EXPAT_FOUND)
   include_directories(${EXPAT_INCLUDE_DIRS})
   target_link_libraries(fmipp ${EXPAT_LIBRARIES})
endif (EXPAT_FOUND)

target_link_libraries(fmipp ${CMAKE_DL_LIBS})

include_directories(include)

add_subdirectory(swig)

add_subdirectory(test)

install(TARGETS fmipp DESTINATION lib)
install(FILES include/FMU.h include/IncrementalFMU.h include/FixedStepSizeFMU.h include/InterpolatingFixedStepSizeFMU.h include/FMUCoSimulation.h DESTINATION include)

add_test(fmipp_testFMU test/testFMU)
add_test(fmipp_testIncrementalFMU test/testIncrementalFMU)
