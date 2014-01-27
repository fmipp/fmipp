cmake_minimum_required(VERSION 2.8)

project(test)

add_definitions(-DFMU_URI_PRE="file://${CMAKE_CURRENT_BINARY_DIR}/")

add_executable(testFMU testFMU.cpp)

add_executable(testIncrementalFMU testIncrementalFMU.cpp)

find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

target_link_libraries(testFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmipp)

target_link_libraries(testIncrementalFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmipp)

include_directories(../include)

add_subdirectory(zigzag_fmu)

add_subdirectory(step_t0_fmu)
