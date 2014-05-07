cmake_minimum_required(VERSION 2.8)

project(test)

add_definitions(-DFMU_URI_PRE="file://${CMAKE_CURRENT_BINARY_DIR}/")
add_definitions(-DEPS_TIME=1e-9)

add_executable(testFMU testFMU.cpp)
add_executable(testIncrementalFMU testIncrementalFMU.cpp)
add_executable(testRollbackFMU testRollbackFMU.cpp)
add_executable(testFMIExportUtilities testFMIExportUtilities.cpp)
add_executable(testFixedStepSizeFMU testFixedStepSizeFMU.cpp)
add_executable(testInterpolatingFixedStepSizeFMU testInterpolatingFixedStepSizeFMU.cpp)
add_executable(testModelDescription testModelDescription.cpp)
add_executable(testModelManager testModelManager.cpp)

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

target_link_libraries(testRollbackFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmipp)

target_link_libraries(testFMIExportUtilities
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmipp
			fmippcs)

target_link_libraries(testFixedStepSizeFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmipp
			fmippcs)

target_link_libraries(testInterpolatingFixedStepSizeFMU
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmipp
			fmippcs)

target_link_libraries(testModelDescription
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmipp)

target_link_libraries(testModelManager
			${Boost_FILESYSTEM_LIBRARY}
			${Boost_SYSTEM_LIBRARY}
			${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
			fmipp)


include_directories(../include)

add_subdirectory(zigzag_fmu)

add_subdirectory(step_t0_fmu)

add_subdirectory(sine_standalone_fmu)
