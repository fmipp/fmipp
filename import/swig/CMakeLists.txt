cmake_minimum_required(VERSION 2.8)

project(fmippim_wrap)

find_package(SWIG)
if (SWIG_FOUND)
   include(UseSWIG)

   find_package(Java REQUIRED)
   include(UseJava)
   find_package(JNI REQUIRED)
   include_directories(${JAVA_INCLUDE_PATH})
   include_directories(${JAVA_INCLUDE_PATH2})

   set_source_files_properties(libfmippim.i PROPERTIES CPLUSPLUS ON)
   swig_add_module(fmippim_wrap java libfmippim.i)
   swig_link_libraries(fmippim_wrap fmippim)

   add_custom_command(OUTPUT fmippim.jar DEPENDS fmippim_wrap
   			     COMMAND ${CMAKE_COMMAND} -E echo "Compiling Java files..."
			     COMMAND ${Java_JAVAC_EXECUTABLE} *.java
			     COMMAND ${CMAKE_COMMAND} -E echo "Creating jar file..."
			     COMMAND ${Java_JAR_EXECUTABLE} -cf fmippim.jar *.class)

   add_custom_target(fmippim_jar ALL DEPENDS fmippim.jar)

   install(TARGETS fmippim_wrap DESTINATION lib)
   install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fmippim.jar DESTINATION lib)

   set(FMIPPIM_JAR ${CMAKE_CURRENT_BINARY_DIR}/fmippim.jar CACHE INTERNAL "find the created jar. necessary because add_jar and wildcards don't work out here and so it's complicated with dependencies.")
   set(FMIPPIM_WRAP_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "have the directory because we need the path where to look for the dynamic library.")
endif (SWIG_FOUND)
